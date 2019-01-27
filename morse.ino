int ledPin = 2;
int ditButton = 12; //D12
int dahButton = 25; //D25
int funcButton = 27; //D27

// push type has a count and button down time
struct push_t { 
  push_t(): cnt(0), down(0) {};
  int cnt;
  unsigned long down; 
} ditKey, dahKey, funcKey;  

// update push, return true if use as keypress
// make shift debouncing
bool push_it(push_t &b){
  unsigned long t = millis();
  if(t - b.down > 3*20){
    b.cnt = 0;
    b.down = t;
    //Serial.printf("time between pushes too long. reset count: %d\n", t-b.down);
  }
  if(b.cnt > 6) { 
    //Serial.println("held for long enough to restar count");
    b.cnt=0;
    b.down = t;
  }

  // increase count
  b.cnt++;
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

// setup keyboard settings
String keySeq    = "";
String lastSent = "";

void setup() {
    pinMode(ledPin, OUTPUT);
    Serial.begin(115200);
    //Serial.begin(9600); // matches seriel out?
    pinMode(ditButton, INPUT);
    pinMode(dahButton, INPUT);
    pinMode(funcButton, INPUT);
}

long unsigned lastPush = 0;
void loop() {

    long unsigned t = millis(); 
    if( lastPush > 0 &&
        t - lastPush > 100 && 
        ! keySeq.equals("")) {
      Serial.printf("send %s\n", keySeq.c_str()); 
      keySeq = "";
    }
    
    // any keys currently down?
    bool dit, dah, func;
    dit  = digitalRead(ditButton)  == HIGH && push_it(ditKey);
    dah  = digitalRead(dahButton)  == HIGH && push_it(dahKey);
    func = digitalRead(funcButton) == HIGH && push_it(funcKey);

    bool ditAndDah = (dit || dah) && abs(ditKey.down - dahKey.down) < 3*30;
    
    // append pushes onto morse queue
    if( dit && dah) {
      Serial.println("both");
    } else if(dit){
      keySeq += ".";
      lastPush = t;
      Serial.printf("add . @ %f\n", t);
    } else if(dah) {
      keySeq += "_";
      lastPush=t;
      Serial.printf("add _ @ %f\n", t);
    }
    // else: no button push in last 20ish ms

    
    // wait and reset led
    delay(20);
    digitalWrite(ledPin, LOW);

}
