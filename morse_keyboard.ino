// add bluetooth hid/keybaord
// https://github.com/nkolban/esp32-snippets/issues/230
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>
#include <keycodes.h> // ASCII_TO_KEYCODE, SHIFT_FLAG, funcOn_to_keycode, morse_to_char
#include <BTHID.h>    // setupBLECharacteristics, BTSend

// pin values
int ledPin = 2;
int ditButton = 12; //D12
int dahButton = 25; //D25
int funcButton = 27; //D27
bool funcOn = false;
bool shiftOn = false;

//bluetooth initialize

bool BTEconnected = false;
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
  String keySeq;
  unsigned int n_dd;
  long unsigned lastPush;
  char key;
  int keycode, prevSent;
} state;
void reset_state(state_t &s){
    s.prevSent = s.keycode;
    s.keycode = 0;
    s.keySeq   = "";
    s.lastPush = 0;
    s.n_dd     = 0;
    s.key = '\0';
}
void send_state(state_t &s){
    // do nothing if we have no morse code sequence
    if(s.keySeq.equals("") ) { return; }
    // figure out what character to send
    if(funcOn) {
       // can toggle shift if we are overwriting normal shift behavior
       s.key = funcOn_to_key(s.keySeq, shiftOn);
    } else {
       s.key = morse_to_char(s.keySeq);
    }

    s.keycode = ASCII_TO_KEYCODE[s.key];
    // shift key
    if(shiftOn) {s.keycode|=SHIFT_FLAG;}

    BTSend(s.keycode, BTEconnected, reportChar1);

    Serial.printf("send %c (len %d) == %s == %d (shift %d)\n",
          s.key, s.n_dd, s.keySeq.c_str(), s.keycode, shiftOn);

    // after any send, remove func
    funcOn = false;
    shiftOn = false;
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
    setupBLECharacteristics(pService, pService1, reportChar1, reportChar2, reportChar3);

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

    // turn on func
    //  repeat char if already on
    if (func) {
       if(funcOn){
          // todo: if func already on, repeat last?
          BTSend(state.prevSent, BTEconnected, reportChar1);
          Serial.printf("send prevous keycode %d\n", state.prevSent);
       }
       Serial.printf("func on\n");
       funcOn = true;
    }

    // append pushes onto morse queue
    if(dit && dah) {
      if(funcOn){
         Serial.println("both down with func set shift");
         shiftOn = true;
         funcOn = false;
      } else {
         Serial.println("both down, send BS");
         int BS = 0x2a;
         BTSend(BS, BTEconnected, reportChar1);
      }
      reset_state(state);
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


