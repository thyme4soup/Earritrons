/*
 * This program characterizes the filaments of the numitrons for pulse-based supply. This
 * may solve the problem of the battery not supplying enough voltage at a sufficiently high current
 * This supplies the numitron with a 50% duty square wave at a defined frequency
 * 
 * Numitrons hold steady when blinking at around 90 Hz, or 10 ms per cycle. This is def attainable
 * in the main program, but I'll need to switch from delay() to millis() to account for processing
 * time.
 * Of course, it could also look a bit cool to go as low as 50 Hz for that low-tech effect
 * Also, it's important to note that the numitrons become about half brightness due to the 50% duty
 * cycle. It might help to up the current to each segment, which would be reasonable with the divided
 * sustained current
 * 
 * I tested with a 9V and it seems like the 9V can actually provide all the current, but for some
 * reason the 9V can power them simultaneously. It's weird, I just need to try again at college
 * 
 * 1/6/18
 */


const int NUMITRON_PINS1[] = {A2, A1, 2, 3, 4, 5, 6}; //pin number for numitron 1
const int NUMITRON_PINS2[] = {7, 8, 9, 10, 11, 12, 13}; //pin numbers for numitron 2
const int FSM_DELAY = 1;
const int PULSE_FREQ = 90; //in hertz
const int PIEZO = A0;

int num = 99;

const byte NUMITRON_INT[][7] = {
    {HIGH, HIGH, LOW, HIGH, HIGH, HIGH, HIGH}, //0
    {LOW, LOW, LOW, LOW, LOW, HIGH, HIGH}, //1
    {HIGH, HIGH, HIGH, LOW, HIGH, LOW, HIGH}, //2
    {LOW, HIGH, HIGH, LOW, HIGH, HIGH, HIGH}, //3
    {LOW, LOW, HIGH, HIGH, LOW, HIGH, HIGH}, //4
    {LOW, HIGH, HIGH, HIGH, HIGH, HIGH, LOW}, //5
    {HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, LOW}, //6
    {LOW, LOW, LOW, LOW, HIGH, HIGH, HIGH}, //7
    {HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH}, //8
    {LOW, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH}, //9
  };


int pulse_cycles = (1000 / PULSE_FREQ) / FSM_DELAY / 2; //Hertz to Seconds to Cycles per period to Cycles per 1/2 period

void setup() {
  Serial.begin(9600);
  // setup pins
  for(int i = 0; i < sizeof(NUMITRON_PINS1) / sizeof(int); i++) {
    pinMode(NUMITRON_PINS1[i], OUTPUT);
    digitalWrite(NUMITRON_PINS1[i], LOW);
  }
  for(int i = 0; i < sizeof(NUMITRON_PINS2) / sizeof(int); i++) {
    pinMode(NUMITRON_PINS2[i], OUTPUT);
    digitalWrite(NUMITRON_PINS2[i], LOW);
  }
  Serial.println(pulse_cycles);
  Serial.print(pulse_cycles * FSM_DELAY * 2);
  Serial.println(" ms per cycle");
}

byte cur_mode = LOW;
byte cur_mode_c = HIGH;
int pulse_cntr = 0;
unsigned long dec = millis();
void loop() {
  unsigned long millis_start = millis();
  if(pulse_cntr++ >= pulse_cycles) {
    if(cur_mode == HIGH) {
      cur_mode = LOW;
    }
    else if(cur_mode == LOW) {
      cur_mode = HIGH;
    }
    if(cur_mode == HIGH) {
      for(int i = 0; i < sizeof(NUMITRON_PINS1) / sizeof(int); i++) {
        digitalWrite(NUMITRON_PINS1[i], NUMITRON_INT[(num/10)][i]);
      }
    }
    else {
      for(int i = 0; i < sizeof(NUMITRON_PINS2) / sizeof(int); i++) {
        digitalWrite(NUMITRON_PINS2[i], NUMITRON_INT[(num%10)][i]);
      }
    }
    pulse_cntr = 0;
  }
  if((unsigned long)(millis() - dec) >= 2000) {
    dec = millis();
    if(--num < 0) {
      num = 99;
      tone(PIEZO, 40, 100);
      delay(1000);
    }
  }
  while((unsigned long)(millis() - millis_start) <= FSM_DELAY); //using millis() to get an accurate frequency regardless of loop processing time
}
