int ledPin = 2;
int ditButton = 12; //D12
int dahButton = 25; //D25
int funcButton = 27; //D27

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
    s.lookup   = 2^7;
    s.lastPush = 0;
    s.n_dd     = 0;
}
void send_state(state_t &s){
    if(!s.keySeq.equals("") ) {
       Serial.printf("send ind %d (len %d) == %s\n", 
             s.lookup, s.n_dd, s.keySeq.c_str()); 
    }
}
/*
 change state to account for new dit or dah
*/
void update_state(state_t &s, bool dah, unsigned long t){
      char add;
      if(dah) {
         add = '_';
         s.lookup |= s.lookup << s.n_dd;// _ is 1
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
