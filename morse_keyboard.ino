// add bluetooth hid/keybaord
// https://github.com/nkolban/esp32-snippets/issues/230
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>
#include <keycodes.h> //ASCII_TO_KEYCODE, SHIFT_FLAG

// pin values
int ledPin = 2;
int ditButton = 12; //D12
int dahButton = 25; //D25
int funcButton = 27; //D27

// initialize function
char morse_to_char(String keySeq);
void setupBLECharacteristics();


//bluetooth initialize

bool BTEconnected = false;
uint8_t v[] = {0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0};
BLEService *pService;
BLEService *pService1;
BLECharacteristic *reportChar1;
BLECharacteristic *reportChar2;
BLECharacteristic *reportChar3;
class BLEConnectCB : public BLEServerCallbacks {
  void onConnect(BLEServer* pServer){
    BTEconnected=true;
  }

  void onDisconnect(BLEServer* pServer){
    BTEconnected=false;
  }
};


// push type has a count and button down time
struct push_t { 
  push_t(): cnt(0), down(0), last(0) {};
  int cnt;
  unsigned long down, last; 
} ditKey, dahKey, funcKey;  

// update push, return true if use as keypress
// make shift debouncing
bool push_it(push_t &b){
  unsigned long t = millis();
  if(t - b.last > 3*20){
    b.cnt = 0;
    //Serial.printf("time between pushes too long. reset count: %d\n", t-b.down);
  }
  if(b.cnt > 6) { 
    //Serial.println("held for long enough to restar count");
    b.cnt=0;
  }

  // increase count
  // update last time of press
  // set down time if this is the first interval button is down
  b.cnt++;
  b.last = t;
  if(b.cnt == 1) { b.down = t; }
  //Serial.printf("cnt: %d\n",b.cnt);
  
  // had for 3 intervals, count as push
  // counts above 3 accumuated until hitting thres above
  // stops repeat sends from fat fingers
  if(b.cnt == 3 ){
    digitalWrite(ledPin, HIGH);
    return(true);
  }


  // no button push on this interval
  return(false);
}


// track keyboard state
struct state_t {
  String keySeq, prevSent;
  unsigned int n_dd;
  long unsigned lastPush;
} state;
void reset_state(state_t &s){
    s.prevSent = s.keySeq;
    s.keySeq   = "";
    s.lastPush = 0;
    s.n_dd     = 0;
}
void send_state(state_t &s){
    // do nothing if we have no morse code sequence
    if(s.keySeq.equals("") ) { return; }
    // figure out what character to send
    char key = morse_to_char(s.keySeq);
    int keycode = ASCII_TO_KEYCODE[key];
    Serial.printf("send %c (len %d) == %s == %d\n", 
          key, s.n_dd, s.keySeq.c_str(), keycode); 

    // send it
    if(BTEconnected){
     uint8_t a[] = {0x0, 0x0, keycode, 0x0,0x0,0x0,0x0,0x0};
     reportChar1->setValue(a, sizeof(a));
     reportChar1->notify();    

     reportChar1->setValue(v, sizeof(v));
     reportChar1->notify();
   } else {
    Serial.println("not connected to bt"); 
   }
}
/*
 change state to account for new dit or dah
*/
void update_state(state_t &s, bool dah, unsigned long t){
      char add;
      if(dah) {
         add = '_';
      } else {
         add = '.';
      }
      
      s.lastPush = t;
      s.n_dd++;
      s.keySeq += add;
      Serial.printf("\t(%d) add %c @ %d\n", s.n_dd, add, t % 10000);
}

// setup keyboard settings
void setup() {
    Serial.begin(115200);
    //Serial.begin(9600); // matches seriel out?

    pinMode(ledPin, OUTPUT);
    pinMode(ditButton, INPUT);
    pinMode(dahButton, INPUT);
    pinMode(funcButton, INPUT);
    reset_state(state);


    //BLE HID
    BLEDevice::init("ESP32");
    BLEServer *pServer = BLEDevice::createServer();
    pServer->setCallbacks(new BLEConnectCB());
    pService = pServer->createService((uint16_t)0x180a);
    pService1 = pServer->createService((uint16_t)0x1812, 30);
    setupBLECharacteristics();
    
    pService->start();
    pService1->start();
    BLEAdvertising *pAdvertising = pServer->getAdvertising();
    pAdvertising->setAppearance(961);
    pAdvertising->addServiceUUID((uint16_t)0x1812);
    pAdvertising->start();
}

void loop() {

    long unsigned t = millis(); 
    if( state.lastPush > 0 &&
        t - state.lastPush > 250 && 
        ! state.keySeq.equals("")
      ) {
      send_state(state);
      reset_state(state);
    }
    
    // any keys currently down?
    bool dit, dah, func;
    dit  = digitalRead(ditButton)  == HIGH && push_it(ditKey);
    dah  = digitalRead(dahButton)  == HIGH && push_it(dahKey);
    func = digitalRead(funcButton) == HIGH && push_it(funcKey);

    bool ditAndDah = (dit || dah) &&
                      abs(ditKey.down - dahKey.down) < 3*30;
    
    // append pushes onto morse queue
    if( dit && dah) {
      Serial.println("both");
    } else if(dit){
      update_state(state, false, t);
    } else if(dah) {
      update_state(state, true, t);
    }
    // else: no button push in last 20ish ms

    
    // wait and reset led
    delay(20);
    digitalWrite(ledPin, LOW);

}




char morse_to_char(String keySeq){
   // not actually morse code
   if(keySeq.equals("._._"))    {return('\n');}

   else if(keySeq.equals("._"))      {return('a');}
   else if(keySeq.equals("_..."))    {return('b');}
   else if(keySeq.equals("_._."))    {return('c');}
   else if(keySeq.equals("_.."))     {return('d');}
   else if(keySeq.equals("."))       {return('e');}
   else if(keySeq.equals(".._."))    {return('f');}
   else if(keySeq.equals("__."))     {return('g');}
   else if(keySeq.equals("...."))    {return('h');}
   else if(keySeq.equals(".."))      {return('i');}
   else if(keySeq.equals(".___"))    {return('j');}
   else if(keySeq.equals("_._"))     {return('k');}
   else if(keySeq.equals("._.."))    {return('l');}
   else if(keySeq.equals("__"))      {return('m');}
   else if(keySeq.equals("_."))      {return('n');}
   else if(keySeq.equals("___"))     {return('o');}
   else if(keySeq.equals(".__."))    {return('p');}
   else if(keySeq.equals("__._"))    {return('q');}
   else if(keySeq.equals("._."))     {return('r');}
   else if(keySeq.equals("..."))     {return('s');}
   else if(keySeq.equals("_"))       {return('t');}
   else if(keySeq.equals(".._"))     {return('u');}
   else if(keySeq.equals("..._"))    {return('v');}
   else if(keySeq.equals(".__"))     {return('w');}
   else if(keySeq.equals("_.._"))    {return('x');}
   else if(keySeq.equals("_.__"))    {return('y');}
   else if(keySeq.equals("__.."))    {return('z');}
   else if(keySeq.equals(".____"))   {return('1');}
   else if(keySeq.equals("..___"))   {return('2');}
   else if(keySeq.equals("...__"))   {return('3');}
   else if(keySeq.equals("...._"))   {return('4');}
   else if(keySeq.equals("....."))   {return('5');}
   else if(keySeq.equals("_...."))   {return('6');}
   else if(keySeq.equals("__..."))   {return('7');}
   else if(keySeq.equals("___.."))   {return('8');}
   else if(keySeq.equals("____."))   {return('9');}
   else if(keySeq.equals("_____"))   {return('0');}
   else if(keySeq.equals("._._._"))  {return('.');}
   else if(keySeq.equals("__..__"))  {return(',');}
   else if(keySeq.equals("..__.."))  {return('?');}
   else if(keySeq.equals(".____."))  {return('\'');}
   else if(keySeq.equals("_._.__"))  {return('!');}
   else if(keySeq.equals("_.._."))   {return('/');}
   else if(keySeq.equals("_.__."))   {return('(');}
   else if(keySeq.equals("_.__._"))  {return(')');}
   else if(keySeq.equals("._..."))   {return('&');}
   else if(keySeq.equals("___..."))  {return(':');}
   else if(keySeq.equals("_._._."))  {return(';');}
   else if(keySeq.equals("_..._"))   {return('=');}
   else if(keySeq.equals("._._."))   {return('+');}
   else if(keySeq.equals("_...._"))  {return('-');}
   else if(keySeq.equals("..__._"))  {return('_');}
   else if(keySeq.equals("._.._."))  {return('"');}
   else if(keySeq.equals("..._.._")) {return('$');}
   else if(keySeq.equals(".__._."))  {return('@');}
   else {return(' ');}
}


// BLE HID from https://github.com/nkolban/esp32-snippets/issues/230#issuecomment-352197634
void setupBLECharacteristics() {

  BLECharacteristic *manufacturer = pService->createCharacteristic(
                                       (uint16_t)0x2a29,
                                       BLECharacteristic::PROPERTY_READ
                                     );
  std::string name = "espressif";
  manufacturer->setValue(name);

  BLECharacteristic *pnpIDChar = pService->createCharacteristic(
                                       (uint16_t)0x2a50,
                                       BLECharacteristic::PROPERTY_READ
                                     );
  const uint8_t pnp[] = {0x01,0xe5,0x02,0xcd,0xab,0x01,0x00};
  pnpIDChar->setValue((uint8_t*)pnp, sizeof(pnp));

  BLECharacteristic *hidInfoChar = pService1->createCharacteristic(
                                       (uint16_t)0x2a4a,
                                       BLECharacteristic::PROPERTY_READ
                                     );
  const uint8_t val1[] = {0x00,0x01,0x00,0x02};
  hidInfoChar->setValue((uint8_t*)val1, 4);

  BLECharacteristic *reportMapChar = pService1->createCharacteristic(
                                       (uint16_t)0x2a4b,
                                       BLECharacteristic::PROPERTY_READ
                                     );
  const uint8_t val2[] = {
    0x05,0x01,0x09,0x06,0xA1,0x01,0x05,0x07,
    0x19,0xE0,0x29,0xE7,0x15,0x00,0x25,0x01,
    0x75,0x01,0x95,0x08,0x81,0x02,0x95,0x01,
    0x75,0x08,0x81,0x01,0x95,0x05,0x75,0x01,
    0x05,0x08,0x19,0x01,0x29,0x05,0x91,0x02,
    0x95,0x01,0x75,0x03,0x91,0x01,0x95,0x06,
    0x75,0x08,0x15,0x00,0x25,0x65,0x05,0x07,
    0x19,0x00,0x29,0x65,0x81,0x00,0xC0}; //TODO
  reportMapChar->setValue((uint8_t*)val2, sizeof(val2));

  BLECharacteristic *hidControlChar = pService1->createCharacteristic(
                                       (uint16_t)0x2a4c,
                                       BLECharacteristic::PROPERTY_WRITE_NR
                                     );

  reportChar1 = pService1->createCharacteristic(
                                       (uint16_t)0x2a4d,
                                       BLECharacteristic::PROPERTY_READ   |
                                       BLECharacteristic::PROPERTY_NOTIFY
                                     );
  BLEDescriptor *desc1 = new BLEDescriptor(BLEUUID((uint16_t)0x2908));
  const uint8_t desc1_val[] = {0x01, 0};
  desc1->setValue((uint8_t*)desc1_val, 1);
  reportChar1->addDescriptor(desc1);
  reportChar1->addDescriptor(new BLE2902());

  reportChar2 = pService1->createCharacteristic(
                                       (uint16_t)0x2a4d,
                                       BLECharacteristic::PROPERTY_READ   |
                                       BLECharacteristic::PROPERTY_WRITE
                                     );
  BLEDescriptor *desc2 = new BLEDescriptor(BLEUUID((uint16_t)0x2908));
  const uint8_t desc2_val[] = {0x02, 0};
  desc2->setValue((uint8_t*)desc2_val, 1);
  reportChar2->addDescriptor(desc2);

  reportChar3 = pService1->createCharacteristic(
                                       (uint16_t)0x2a4d,
                                       BLECharacteristic::PROPERTY_READ     |
                                       BLECharacteristic::PROPERTY_WRITE    |
                                       BLECharacteristic::PROPERTY_WRITE_NR
                                     );
  BLEDescriptor *desc3 = new BLEDescriptor(BLEUUID((uint16_t)0x2908));
  const uint8_t desc3_val[] = {0x03, 0};
  desc3->setValue((uint8_t*)desc3_val, 1);
  reportChar3->addDescriptor(desc3);

  BLECharacteristic *protocolModeChar = pService1->createCharacteristic(
                                       (uint16_t)0x2a4e,
                                       BLECharacteristic::PROPERTY_WRITE_NR
                                     );

  BLECharacteristic *bootInputChar = pService1->createCharacteristic(
                                       (uint16_t)0x2a22,
                                       BLECharacteristic::PROPERTY_NOTIFY
                                     );
  bootInputChar->addDescriptor(new BLE2902());


  BLECharacteristic *bootOutputChar = pService1->createCharacteristic(
                                       (uint16_t)0x2a32,
                                       BLECharacteristic::PROPERTY_READ     |
                                       BLECharacteristic::PROPERTY_WRITE    |
                                       BLECharacteristic::PROPERTY_WRITE_NR
                                     );



}
