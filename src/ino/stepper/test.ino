//Pins to use
int PIN_ENABLE = 6;
int PIN_STEP = 5;
int PIN_DIR = 4;

//Some predefined stepper constants
int STP_LOW = 50;
int STP_MED = 100;
int STP_HIGH = 1000;
int DIR_LEFT = 1;
int DIR_RIGHT = 0;
int SPD_VSLOW = 30000;
int SPD_SLOW = 10000;
int SPD_MED = 5000;
int SPD_FAST = 500;


//Setup
void setup() {

  pinMode(PIN_ENABLE, OUTPUT);
  pinMode(PIN_STEP, OUTPUT);
  pinMode(PIN_DIR, OUTPUT);
  digitalWrite(PIN_ENABLE,//Pins to use
int PIN_ENABLE = 6;
int PIN_STEP = 5;
int PIN_DIR = 4;

//Some predefined stepper constants
int STP_LOW = 50;
int STP_MED = 100;
int STP_HIGH = 1000;
int DIR_LEFT = 1;
int DIR_RIGHT = 0;
int SPD_VSLOW = 30000;
int SPD_SLOW = 10000;
int SPD_MED = 5000;
int SPD_FAST = 500;


//Setup
void setup() {

  pinMode(PIN_ENABLE, OUTPUT);
  pinMode(PIN_STEP, OUTPUT);
  pinMode(PIN_DIR, OUTPUT);
  digitalWrite(PIN_ENABLE, HIGH);
}

//Enable the stepper
void stepper_enable(boolean enabled) {
  if (enabled)
    digitalWrite(PIN_ENABLE, LOW);
  else
    digitalWrite(PIN_ENABLE, HIGH);
}

//Perform some steps
void step(int steps, int dir, int speed) {

  digitalWrite(PIN_DIR, dir);

  for (int i = 0; i < steps; i++) {

    digitalWrite(PIN_STEP, HIGH);
    delayMicroseconds(speed);
    digitalWrite(PIN_STEP, LOW);
    delayMicroseconds(speed);
  }
}

//Main event loop
void loop() {

  //Ensure the stepper is enabled
  stepper_enable(true);

  //Some steps right, then some steps left
  step(STP_LOW, DIR_RIGHT, SPD_VSLOW);
  delay(1000);
  step(STP_LOW, DIR_LEFT, SPD_VSLOW);
  delay(1000);
} HIGH);
}

//Enable the stepper
void stepper_enable(boolean enabled) {
  if (enabled)
    digitalWrite(PIN_ENABLE, LOW);
  else
    digitalWrite(PIN_ENABLE, HIGH);
}

//Perform some steps
void step(int steps, int dir, int speed) {

  digitalWrite(PIN_DIR, dir);

  for (int i = 0; i < steps; i++) {

    digitalWrite(PIN_STEP, HIGH);
    delayMicroseconds(speed);
    digitalWrite(PIN_STEP, LOW);
    delayMicroseconds(speed);
  }
}

//Main event loop
void loop() {

  //Ensure the stepper is enabled
  stepper_enable(true);

  //Some steps right, then some steps left
  step(STP_LOW, DIR_RIGHT, SPD_VSLOW);
  delay(1000);
  step(STP_LOW, DIR_LEFT, SPD_VSLOW);
  delay(1000);
}
