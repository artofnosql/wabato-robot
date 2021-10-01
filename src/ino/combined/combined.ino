//Stepper motor
const int MOTOR_PIN_ENABLE = 6, MOTOR_PIN_STEP = 5, MOTOR_PIN_DIR = 4;
const int STP_DGR_360=200;
const int STP_LOW = 50, STP_MED = 100,STP_HIGH = 1000;
const int DIR_LEFT = 1, DIR_RIGHT = 0, DIR_NONE = -1;
const int SPD_VSLOW = 30000, SPD_SLOW = 10000, SPD_MED = 5000, SPD_FAST = 500;
const char* S_STR[] = {"LEFT", "CENTER_LEFT", "CENTER", "CENTER_RIGHT", "RIGHT"};
const int S_LEFT=0, S_CENTER=2, S_RIGHT=4;

//Ultra-sonic sensors
const int TRIGGER_PIN_L=8, ECHO_PIN_L=9;
const int TRIGGER_PIN_M=12, ECHO_PIN_M=10;
const int TRIGGER_PIN_R=13, ECHO_PIN_R=11;
const int MAX_DIST=120;
const char* POS_STR[] = {"NONE", "LEFT", "CENTER_LEFT", "CENTER", "CENTER_RIGHT", "RIGHT", "ALL", "UNDEFINED"};
const char* MOV_STR[] = {"NONE", "LEFT", "RIGHT"};
const int DIDNT_MOVE=0, MOVED_LEFT=1, MOVED_RIGHT=2;


//Init the motor
void init_motor() {
  pinMode(MOTOR_PIN_ENABLE, OUTPUT);
  pinMode(MOTOR_PIN_STEP, OUTPUT);
  pinMode(MOTOR_PIN_DIR, OUTPUT);
  digitalWrite(MOTOR_PIN_ENABLE, HIGH);
}

//Init a ultra-sonic sensor
void init_sensor(int trig_id, int echo_id) {
  pinMode(trig_id, OUTPUT);
  pinMode(echo_id, INPUT);
}


//Setup
void setup() {
  Serial.begin(9600);
  init_motor();
  init_sensor(TRIGGER_PIN_L, ECHO_PIN_L);
  init_sensor(TRIGGER_PIN_M, ECHO_PIN_M);
  init_sensor(TRIGGER_PIN_R, ECHO_PIN_R);
}

/**
 * Enable the stepper
 * 
 */
void stepper_enable(boolean enabled) {
  if (enabled)
    digitalWrite(MOTOR_PIN_ENABLE, LOW);
  else
    digitalWrite(MOTOR_PIN_ENABLE, HIGH);
}

/**
 * Perform some steps
 */
void step(int steps, int dir, int speed) {

  digitalWrite(MOTOR_PIN_DIR, dir);

  for (int i = 0; i < steps; i++) {

    digitalWrite(MOTOR_PIN_STEP, HIGH);
    delayMicroseconds(speed);
    digitalWrite(MOTOR_PIN_STEP, LOW);
    delayMicroseconds(speed);
  }
}

int degree_to_steps(int degree) {

  // 360 : 200 = degree : steps
  int steps = ( STP_DGR_360 * degree) / 360;
  return steps;
  
}

/**
 * Measure the distance of a specific sensor
 * 
 * Send a sonar signal, wait and receive the echo
 */
long dist(int trig_id, int echo_id) {
  long t,d;
  digitalWrite(trig_id, LOW);
  delayMicroseconds(2);
  digitalWrite(trig_id, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig_id, LOW);
  
  t = pulseIn(echo_id, HIGH);
  d = t / 58.3;

  return d;
}


/**
 * I see you!
 * 
 * Check if a specific sensor sees an observer by checking 
 * if the distance is lower then MAX_D
 */
boolean seen(long dist) {
  
  if ((dist > 0) && (dist < MAX_DIST)) {
    return true;
  }


  return false;
  
}

/**
 * Current position of the observer
 * 
 * It's possible to determine the position of the observer by taking all
 * sensor values into account
 */
int current_pos(int d_l, int d_m, int d_r) {

  int result = 7;
  
  if (seen(d_l)==0 && seen(d_m)==0 && seen(d_r)==0){
    result=0;//NONE
  }

  if (seen(d_l)==1 && seen(d_m)==0 && seen(d_r)==0) {
    result=1;//LEFT
  }
  
  if (seen(d_l)==1 && seen(d_m)==1 && seen(d_r)==0) {
    result=2;//CENTER_LEFT
  }

  if (seen(d_l)==0 && seen(d_m)==1 && seen(d_r)==0) {
    result=3;//CENTER
  }

  if (seen(d_l)==0 && seen(d_m)==1 && seen(d_r)==1) {
    result=4;//CENTER_RIGHT
  }

  if (seen(d_l)==0 && seen(d_m)==0 && seen(d_r)==1) {
    result=5;//RIGHT
  }
 
  if (seen(d_l)==1 && seen(d_m)==1 && seen(d_r)==1) {
    result=6;//ALL
  }

  return result;

}


/**
 * Try to detect a movement
 */
int detect_movement(){

  //There is a movement if a state transistions from left to right or right to left within a period of n seconds
  
  //Take some samples
  int NUM_SAMPLES = 10;
  
  long samples[NUM_SAMPLES];

  boolean from_left=false;
  boolean from_right=false;
  int start_idx=0;
  
  for (int i=0; i < NUM_SAMPLES; i++) {
    long d_l = dist(TRIGGER_PIN_L, ECHO_PIN_L);
    delay(5);
    long d_m = dist(TRIGGER_PIN_M, ECHO_PIN_M);
    delay(5);
    long d_r = dist(TRIGGER_PIN_R, ECHO_PIN_R);
    Serial.println("[ " + String(d_l) + "," + String(d_m) + "," + String(d_r) + " ]");
    
    int cpos=current_pos(d_l,d_m,d_r);
    samples[i]=cpos;

    if (from_left == false && from_right == false) {

      if (cpos == 1 || cpos == 2) {
        from_left = true;
        start_idx=i;
      }

      if (cpos == 4 || cpos == 5) {
        from_right = true;
        start_idx=i;
      }
    }
         
    delay(50);
  }

  //Start to scan for the opposite direction
  boolean to_right=false;
  boolean to_left=false;

  for (int i=start_idx; i < 30; i++) {
    long sample = samples[i];
    if (from_left==true && to_right==false) {
      if (sample >= 4 && sample < 7) {
        to_right=true;  
      }
    }

    if(from_right==true && to_left==false) {
      if (sample<=4 && sample > 0) {
        to_left=true;
      }
    }
  }

  if (from_right == true && to_left == true) return 1;
  if (from_left == true && to_right == true) return 2;

  return 0;
}


/**
 * Detect the last position of the observer
 */
int detect_pos(){
 //Take 10 samples -- We might do more fancy stuff with them
 long samples[10];
 int cpos=0;

 for (int i=0; i < 10; i++) {
    
    long d_l = dist(TRIGGER_PIN_L, ECHO_PIN_L);
    delay(10);
    long d_m = dist(TRIGGER_PIN_M, ECHO_PIN_M);
    delay(10);
    long d_r = dist(TRIGGER_PIN_R, ECHO_PIN_R);
    
    Serial.println("[ " + String(d_l) + "," + String(d_m) + "," + String(d_r) + " ]");
    cpos=current_pos(d_l,d_m,d_r);
    samples[i] = cpos;

    delay(80);  
 }

 return cpos;

}

/**
 * Use a state machine to follow the movement by taking the current motor position into account 
 */
int follow_me(int s, int m) {

  //Default value = undefined
  int s_o = -1;
  int MAX_DEGREE=60;

  stepper_enable(true);

  if (s == S_LEFT) {
    if (m == MOVED_LEFT) {s_o = S_LEFT;}
    if (m == DIDNT_MOVE) {step(degree_to_steps(MAX_DEGREE),DIR_RIGHT,SPD_VSLOW);s_o = S_CENTER;}
    if (m == MOVED_RIGHT) {step(degree_to_steps(2*MAX_DEGREE),DIR_RIGHT,SPD_VSLOW);s_o = S_RIGHT;}
  }

  if (s == S_CENTER) {
    if (m == MOVED_LEFT) {step(degree_to_steps(MAX_DEGREE),DIR_LEFT,SPD_VSLOW);s_o = S_LEFT;}
    if (m == DIDNT_MOVE) {s_o = S_CENTER;}
    if (m == MOVED_RIGHT) {step(degree_to_steps(MAX_DEGREE),DIR_RIGHT,SPD_VSLOW);s_o = S_RIGHT;}
  }

  if (s == S_RIGHT) {
    if (m == MOVED_LEFT) {step(degree_to_steps(2*MAX_DEGREE),DIR_LEFT,SPD_VSLOW);s_o = S_LEFT;}
    if (m == DIDNT_MOVE) {step(degree_to_steps(MAX_DEGREE),DIR_LEFT,SPD_VSLOW);s_o = S_CENTER;}
    if (m == MOVED_RIGHT) {s_o = S_RIGHT;}
  }

  stepper_enable(false);
  return s_o;
}

//Main loop
int s_curr=S_CENTER;

void loop() {
    
  //Detect the movement and react
  int m = detect_movement();
  Serial.println(MOV_STR[m]);
  s_curr = follow_me(s_curr,m);
  
  //Adjust
  //int p = detect_pos();
  //Serial.println(POS_STR[p]);
    
  delay(50);
}
