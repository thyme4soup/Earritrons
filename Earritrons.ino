
/*
 * For use with the numitron earrings
 * 
 * Design Notes:
 *  +Need to switch over to millis()
 *  +Using OE pin to switch between numitrons may fix battery current issues, but cuts brightness in half
 *  +OE square wave method needs ~90 Hz to look smooth, may go as low as 70. See test program
 *  +Max computation time for these seems to be 284 microseconds, 408 with Serial usage. Might be more like
 *   2 millis if the feedback isn't working properly but is still negligible with an FSM delay closer to 5
 */

const bool DEBUG = true;

/*
//PINOUT SETUP
const int CLOCK_PIN = 6;
const int SDO_PIN = 5;
const int LE_PIN = 4;
const int DRIVER_POWER = 7;
const int LED_PIN = 13;
const int OE_PIN = 8;
*/
const int MODE_BUTTON = 2;

//FSM SETUP
const int FSM_DELAY = 20;
int curr_state = 0;
int state_ctr = 0;

//SEGMENT DICT
const int NUM_SEGMENTS[][8]=
  { // ORIENTED UPSIDEDOWN (earring usage): decimal, bottom left, top left, bottom, bottom right, middle, top, top right
    {LOW, HIGH, HIGH, HIGH, HIGH, LOW, HIGH, HIGH}, //0
    {LOW, LOW, LOW, LOW, HIGH, LOW, LOW, HIGH}, //1
    {LOW, HIGH, LOW, HIGH, LOW, HIGH, HIGH, HIGH}, //2
    {LOW, LOW, LOW, HIGH, HIGH, HIGH, HIGH, HIGH}, //3
    {LOW, LOW, HIGH, LOW, HIGH, HIGH, LOW, HIGH}, //4
    {LOW, LOW, HIGH, HIGH, HIGH, HIGH, HIGH, LOW}, //5
    {LOW, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, LOW}, //6
    {LOW, LOW, LOW, LOW, HIGH, LOW, HIGH, HIGH}, //7
    {LOW, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH}, //8
    {LOW, LOW, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH}, //9
    {LOW, HIGH, LOW, LOW, LOW, LOW, LOW, LOW}, //bottom left 10
    {LOW, LOW, LOW, LOW, LOW, HIGH, LOW, LOW}, //middle 11
    {LOW, LOW, LOW, LOW, LOW, LOW, LOW, HIGH}, //top right 12
    {LOW, LOW, LOW, LOW, LOW, LOW, HIGH, LOW}, //top 13
    {LOW, LOW, HIGH, LOW, LOW, LOW, LOW, LOW}, //top left 14
    {LOW, LOW, LOW, LOW, LOW, HIGH, LOW, LOW}, //middle 15
    {LOW, LOW, LOW, LOW, HIGH, LOW, LOW, LOW}, //bottom right 16
    {LOW, LOW, LOW, HIGH, LOW, LOW, LOW, LOW}, //bottom 17
    {LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW}, // OFF 18
    {LOW, LOW, LOW, LOW, LOW, HIGH, LOW, LOW} // - 19
  };
const int SEGMENTS_START_INDEX = 10;
const int SEGMENTS_END_INDEX = 17;
const int OFF_INDEX = 18;
const int DASH_INDEX = 19;

// ORIENTED UPSIDEDOWN (earring usage): decimal, bottom left, top left, bottom, bottom right, middle, top, top right
const int NUMITRON_PINS1[] = //right
  {A0, A5, A4, A3, A2, A1, 11, 13};
const int NUMITRON_PINS2[] = //left
  {10, 3, 4, 5, 6, 7, 8, 9};


//MODE SWITCHING
volatile byte modeFlag = LOW;
int curr_mode = 0;
const int MODES = 4;

//Global vars
int leftNum = 0;
int rightNum = 0;

//LIGHT HARDWARE INTERFACE METHODS
/*
void shiftSegments(int num) {
  for(int i = 0; i < 8; i++) {
    digitalWrite(SDO_PIN, NUM_SEGMENTS[num][i]);
    digitalWrite(CLOCK_PIN, LOW);
    digitalWrite(CLOCK_PIN, HIGH);
  }
}
*/
void lightSegments(int left, int right)
{
  for(int i = 0; i < 8; i++) {
    digitalWrite(NUMITRON_PINS1[i], NUM_SEGMENTS[right][i]);
    digitalWrite(NUMITRON_PINS2[i], NUM_SEGMENTS[left][i]);
  }
  if(DEBUG) {
    Serial.print(left);
    Serial.print(" ");
    Serial.println(right);
  }
  /*
  //turn display on
  digitalWrite(DRIVER_POWER,LOW);
  
  //latch display for shifting
  digitalWrite(LE_PIN, LOW);
  
  //shift segments in (right goes first, so right side goes to secondary TLC5916)
  shiftSegments(right);
  shiftSegments(left);
  
  //clock latch
  digitalWrite(LE_PIN, HIGH);
  digitalWrite(LE_PIN, LOW);
  
  if(DEBUG) {
    Serial.print(rightNum);
    Serial.println(leftNum);
  }
  */
}

//BASIC LIGHT SOFTWARE METHODS
void updateLights() {
  lightSegments(leftNum, rightNum);
}
int incL() {
  if(++leftNum % 10 == 0) {
    leftNum = 0;
    return 1;
  }
  return 0;
}
int incR() {
  if(++rightNum % 10 == 0) {
    rightNum = 0;
    return 1;
  }
  return 0;
}
//increment left, carry digit increments right
void inc() {
  if(incL()) incR();
}
void randomizeL() {
  leftNum = random(0, 10);
}
void randomizeR() {
  rightNum = random(0, 10);
}
void randomize() {
  randomizeL();
  randomizeR();
}

void wake() {
  if(DEBUG) {
    Serial.println("WAKE");
  }
  /*
  digitalWrite(DRIVER_POWER, LOW);
  digitalWrite(OE_PIN, LOW);
  */
  lightSegments(leftNum, rightNum);
}
void sleep() {
  if(DEBUG) {
    Serial.println("SLEEP");
  }
  lightSegments(OFF_INDEX, OFF_INDEX);
  /*
  digitalWrite(OE_PIN, HIGH);
  digitalWrite(DRIVER_POWER, HIGH);
  */
}
void mode() {
  modeFlag = HIGH;
}


/******************FSM METHODS******************/

void new_state(int new_state_num) {
  curr_state = new_state_num;
  state_ctr = 0;
}

//A single segment lights up and travels around the display
const int SEGMENTS[][8]=
  { // ORIENTED UPSIDEDOWN (earring usage): decimal, bottom left, top left, bottom, bottom right, middle, top, top right
    
  };
const int CRAWL_TIME = 750 / FSM_DELAY;
int curr_segment = SEGMENTS_START_INDEX;
void segmentCrawl() {
  switch(curr_state) {
    case 0:
      lightSegments(curr_segment, curr_segment);
      if(state_ctr++ >= CRAWL_TIME) {
        new_state(1);
      }
      break;
    case 1: //inc segment
      if(++curr_segment > SEGMENTS_END_INDEX) {
        curr_segment = SEGMENTS_START_INDEX;
        new_state(2);
      } else {
        new_state(0);
      }
      break;
    case 2:
      lightSegments(curr_segment, curr_segment);
      if(state_ctr++ >= CRAWL_TIME) {
        new_state(3);
      }
      break;
    case 3: //inc segment
      if(++curr_segment > SEGMENTS_END_INDEX) {
        curr_segment = SEGMENTS_START_INDEX;
        new_state(0);
      } else {
        new_state(2);
      }
      break;
    default:
      new_state(0);
  }
}

//Rolls a random number, displays for an amount of time, then sleeps for an amount of time
const int ROLL_HYBER = 15000 / FSM_DELAY; //15 seconds to FSM cycles
const int ROLL_TIME = 1500 / FSM_DELAY; //1.5 seconds to FSM cycles
const int ROLL_DISPLAY_TIME = 10000 / FSM_DELAY; //10 seconds to FSM cycles
void randomRoll() {
  switch(curr_state) {
    case 0: //roll both
      incL();
      incR();
      updateLights();
      if(state_ctr++ >= ROLL_TIME) { //if time is up, switch to rolling least sig digit
        new_state(1);
      }
      break;
    case 1: //roll one
      incR();
      updateLights();
      if(state_ctr++ >= ROLL_TIME) { //if time is up, switch to displaying the roll
        new_state(2);
      }
      break;
    case 2: //dislay roll
      if(state_ctr++ >= ROLL_DISPLAY_TIME) { //if time is up, switch to sleep state and sleep displays
        new_state(3);
        sleep();
      }
      break;
    case 3: //sleep between rolls
      if(state_ctr++ >= ROLL_HYBER) { //if time is up, randomize numbers and go to state 0
        randomize();
        new_state(0);
        wake();
      }
      break;
  }
}

//Displays random numbers that switch after a short delay, has a hybernation time after a certain amount of switches
const int SWITCH_CYCLES = 500 / FSM_DELAY; //1 second to FSM cycles
const int SWITCHES = 15000 / (SWITCH_CYCLES * FSM_DELAY); //15 seconds before sleeping (~20 switches total)
const int SWITCH_HYBER = 15000 / FSM_DELAY; //15 seconds to FSM cycles
int switch_counter = 0;
void randomSwitching() {
  switch(curr_state) {
    case 0:
      if(state_ctr++ >= SWITCH_CYCLES) {
        randomize();
        updateLights();
        state_ctr = 0;
        if(switch_counter++ >= SWITCHES) {
          new_state(1);
          switch_counter = 0;
          sleep();
        }
      }
      break;
    case 1:
      if(state_ctr++ >= SWITCH_HYBER) {
        new_state(0);
        wake();
      }
      break;
  }
}

/******************END FSM METHODS******************/

void setup() {
  // put your setup code here, to run once:
  /*
  pinMode(CLOCK_PIN, OUTPUT);
  pinMode(SDO_PIN, OUTPUT);
  pinMode(LE_PIN, OUTPUT);
  pinMode(DRIVER_POWER, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(OE_PIN, OUTPUT);
  */
  for(int i = 0; i < 8; i++) {
    pinMode(NUMITRON_PINS1[i], OUTPUT);
    pinMode(NUMITRON_PINS2[i], OUTPUT);
  }
  
  pinMode(MODE_BUTTON, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(MODE_BUTTON), mode, FALLING);

  pinMode(A6, OUTPUT);
  pinMode(A6, HIGH);
  
  Serial.begin(9600);
  Serial.print(SWITCH_CYCLES);
  wake();
}

//main loop, runs as an FSM to allow for mode change interrupt
int max_loop_time = 0;
void loop() {
  unsigned long currTime = millis();
  //handle mode interrupt
  if(modeFlag) {
    curr_mode = (++curr_mode % MODES);
    new_state(0);
    if(DEBUG) {
      Serial.print("button pressed...\n");
      delay(50);
      Serial.print("new mode: " + curr_mode);
      delay(50);
      Serial.print("\nmax loop time was: ");
      Serial.print(max_loop_time);
      Serial.print(" microseconds\n");
      delay(50);
      max_loop_time = 0;
    }

    //provide brief visual feedback
    wake();
    lightSegments(DASH_INDEX, curr_mode);
    delay(1250);
    modeFlag = LOW;
    //reset flag
    updateLights();
    
  } else {
    unsigned long FSM_start = micros();
    switch(curr_mode) { //Handle current mode FSM
      case 0:
        randomRoll();
        break;
      case 1:
        randomSwitching();
        break;
      case 2:
        segmentCrawl();
        break;
      case 3:
        sleep();
        break;
      default:
        sleep();
        break;
    }
    unsigned long FSM_end = micros();
    max_loop_time = max(max_loop_time, (int)(FSM_end - FSM_start));
    delay(FSM_DELAY); //delay loop
  }
  /*
  shiftSegments(10);
  //clock latch
  digitalWrite(LE_PIN, HIGH);
  digitalWrite(LE_PIN, LOW);
  delay(100);
  */
}


