//Stepper motor
const int MOTOR_PIN_ENABLE = 6, MOTOR_PIN_STEP = 5, MOTOR_PIN_DIR = 4;
const int STP_DGR_360 = 200;
const int STP_LOW = 50, STP_MED = 100, STP_HIGH = 1000;
const int DIR_LEFT = 0, DIR_RIGHT = 1, DIR_NONE = -1;
const int SPD_VVSLOW = 50000, SPD_VSLOW = 30000, SPD_SLOW = 10000, SPD_MED = 5000, SPD_FAST = 500;
const char* S_STR[] = {"LEFT", "CENTER_LEFT", "CENTER", "CENTER_RIGHT", "RIGHT"};
const int S_LEFT = 0, S_CENTER_LEFT = 1, S_CENTER = 2, S_CENTER_RIGHT = 3, S_RIGHT = 4;

//Ultra-sonic sensors
const int TRIGGER_PIN_L = 8, ECHO_PIN_L = 9;
const int TRIGGER_PIN_M = 12, ECHO_PIN_M = 10;
const int TRIGGER_PIN_R = 13, ECHO_PIN_R = 11;
const int MAX_DIST = 150;
const char* POS_STR[] = {"NONE", "LEFT", "CENTER_LEFT", "CENTER", "CENTER_RIGHT", "RIGHT", "ALL", "UNDEFINED"};
const int POS_NONE = 0, POS_LEFT = 1, POS_CENTER_LEFT = 2, POS_CENTER = 3, POS_CENTER_RIGHT = 4, POS_RIGHT = 5, POS_ALL = 6, POS_UNDEFINED = 7;
const char* MOV_STR[] = {"NONE", "LEFT", "RIGHT"};
const int DIDNT_MOVE = 0, MOVED_LEFT = 1, MOVED_RIGHT = 2;

//Sampling
const int NUM_SAMPLES_LOW = 10, NUM_SAMPLES_MED = 20, NUM_SAMPLES_HIGH = 30;

//Button for calibrating to 0
const int BUTTON_PIN_L = 2;
const int BUTTON_PIN_R = 3;


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

void init_button(int id) {
  pinMode(id, INPUT);
}


//Setup
void setup() {
  Serial.begin(9600);
  init_motor();
  init_sensor(TRIGGER_PIN_L, ECHO_PIN_L);
  init_sensor(TRIGGER_PIN_M, ECHO_PIN_M);
  init_sensor(TRIGGER_PIN_R, ECHO_PIN_R);
  init_button(BUTTON_PIN_L);
  init_button(BUTTON_PIN_R);
}

/**
   Enable the stepper

*/
void stepper_enable(boolean enabled) {
  if (enabled)
    digitalWrite(MOTOR_PIN_ENABLE, LOW);
  else
    digitalWrite(MOTOR_PIN_ENABLE, HIGH);
}

/**
   Perform some steps
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
   Measure the distance of a specific sensor

   Send a sonar signal, wait and receive the echo
*/
long dist(int trig_id, int echo_id) {
  long t, d;
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
   I see you!

   Check if a specific sensor sees an observer by checking
   if the distance is lower then MAX_D
*/
boolean seen(long dist) {

  if ((dist > 0) && (dist < MAX_DIST)) {
    return true;
  }


  return false;

}

/**
   Current position of the observer

   It's possible to determine the position of the observer by taking all
   sensor values into account
*/
int current_pos(int d_l, int d_m, int d_r) {

  int result = 7;

  if (seen(d_l) == 0 && seen(d_m) == 0 && seen(d_r) == 0) {
    result = POS_NONE;
  }

  if (seen(d_l) == 1 && seen(d_m) == 0 && seen(d_r) == 0) {
    result = POS_LEFT;
  }

  if (seen(d_l) == 1 && seen(d_m) == 1 && seen(d_r) == 0) {
    result = POS_CENTER_LEFT;
  }

  if (seen(d_l) == 0 && seen(d_m) == 1 && seen(d_r) == 0) {
    result = POS_CENTER;
  }

  if (seen(d_l) == 0 && seen(d_m) == 1 && seen(d_r) == 1) {
    result = POS_CENTER_RIGHT;
  }

  if (seen(d_l) == 0 && seen(d_m) == 0 && seen(d_r) == 1) {
    result = POS_RIGHT;
  }

  if (seen(d_l) == 1 && seen(d_m) == 1 && seen(d_r) == 1) {
    result = POS_ALL;
  }

  return result;

}


/**
   Try to detect a movement
*/
int detect_movement() {

  //There is a movement if a state transistions from left to right or right to left within a period of n seconds

  //Take some samples
  int num_samples = NUM_SAMPLES_MED;
  long samples[num_samples];

  boolean from_left = false;
  boolean from_right = false;
  int start_idx = 0;

  for (int i = 0; i < num_samples; i++) {
    long d_l = dist(TRIGGER_PIN_L, ECHO_PIN_L);
    delay(1);
    long d_m = dist(TRIGGER_PIN_M, ECHO_PIN_M);
    delay(1);
    long d_r = dist(TRIGGER_PIN_R, ECHO_PIN_R);
    delay(1);

    Serial.println("[ " + String(d_l) + "," + String(d_m) + "," + String(d_r) + " ]");

    int cpos = current_pos(d_l, d_m, d_r);
    samples[i] = cpos;

    if (from_left == false && from_right == false) {

      if (cpos == 1 || cpos == 2) {
        from_left = true;
        start_idx = i;
      }

      if (cpos == 4 || cpos == 5) {
        from_right = true;
        start_idx = i;
      }
    }
  }

  //Start to scan for the opposite direction
  boolean to_right = false;
  boolean to_left = false;

  for (int i = start_idx; i < 30; i++) {
    long sample = samples[i];
    if (from_left == true && to_right == false) {
      if (sample >= 4 && sample < 7) {
        to_right = true;
      }
    }

    if (from_right == true && to_left == false) {
      if (sample <= 4 && sample > 0) {
        to_left = true;
      }
    }
  }

  if (from_right == true && to_left == true) return 1;
  if (from_left == true && to_right == true) return 2;

  return 0;
}


/**
   Detect the last position of the observer
*/
int detect_pos_old() {
  long num_samples = NUM_SAMPLES_LOW;
  long samples[num_samples];
  int cpos = 0;

  for (int i = 0; i < num_samples; i++) {

    long d_l = dist(TRIGGER_PIN_L, ECHO_PIN_L);
    delay(1);
    long d_m = dist(TRIGGER_PIN_M, ECHO_PIN_M);
    delay(1);
    long d_r = dist(TRIGGER_PIN_R, ECHO_PIN_R);
    delay(1);

    Serial.println("[ " + String(d_l) + "," + String(d_m) + "," + String(d_r) + " ]");
    cpos = current_pos(d_l, d_m, d_r);
    samples[i] = cpos;
  }

  return cpos;

}


/**
   Detect the last position of the observer
*/
int detect_pos() {
  long d_l = dist(TRIGGER_PIN_L, ECHO_PIN_L);
  long d_m = dist(TRIGGER_PIN_M, ECHO_PIN_M);
  long d_r = dist(TRIGGER_PIN_R, ECHO_PIN_R);

  Serial.println("[ " + String(d_l) + "," + String(d_m) + "," + String(d_r) + " ]");

  return current_pos(d_l, d_m, d_r);
}


/**
   Use a state machine to follow the movement by taking the current motor position into account
*/
int follow_me(int s, int m) {

  //Default value = undefined
  int s_o = -1;
  int BASE_DEGREE = 30;
  int MAX_DEGREE = 2 * BASE_DEGREE;

  stepper_enable(true);

  //Ignore the intermediate states
  if (s == S_CENTER_LEFT) {
    step(degree_to_steps(BASE_DEGREE), DIR_RIGHT, SPD_VSLOW);
    s = S_CENTER;
  }
  if (s == S_CENTER_RIGHT) {
    step(degree_to_steps(BASE_DEGREE), DIR_LEFT, SPD_VSLOW);
    s = S_CENTER;
  }

  //TODO: Simplify
  if (s == S_LEFT) {
    if (m == MOVED_LEFT) {
      s_o = S_LEFT;
    }
    if (m == DIDNT_MOVE) {
      step(degree_to_steps(MAX_DEGREE), DIR_RIGHT, SPD_VSLOW);
      s_o = S_CENTER;
    }
    if (m == MOVED_RIGHT) {
      step(degree_to_steps(2 * MAX_DEGREE), DIR_RIGHT, SPD_VSLOW);
      s_o = S_RIGHT;
    }
  }

  if (s == S_CENTER) {
    if (m == MOVED_LEFT) {
      step(degree_to_steps(MAX_DEGREE), DIR_LEFT, SPD_VSLOW);
      s_o = S_LEFT;
    }
    if (m == DIDNT_MOVE) {
      s_o = S_CENTER;
    }
    if (m == MOVED_RIGHT) {
      step(degree_to_steps(MAX_DEGREE), DIR_RIGHT, SPD_VSLOW);
      s_o = S_RIGHT;
    }
  }

  if (s == S_RIGHT) {
    if (m == MOVED_LEFT) {
      step(degree_to_steps(2 * MAX_DEGREE), DIR_LEFT, SPD_VSLOW);
      s_o = S_LEFT;
    }
    if (m == DIDNT_MOVE) {
      step(degree_to_steps(MAX_DEGREE), DIR_LEFT, SPD_VSLOW);
      s_o = S_CENTER;
    }
    if (m == MOVED_RIGHT) {
      s_o = S_RIGHT;
    }
  }

  stepper_enable(false);
  return s_o;
}


/**
   Adapt the position by staring at the observer
*/
int stare_at_me(int s, int p) {

  int s_o = s;
  int BASE_DEGREE = 30;

  //DEBUG
  //Serial.println(p);
  //Serial.println(s);

  stepper_enable(true);

  //TODO: Simplify
  if (s == S_LEFT) {
    if (p == POS_LEFT) {
      s_o = S_LEFT;
    }
    if (p == POS_CENTER_LEFT) {
      step(degree_to_steps(BASE_DEGREE), DIR_RIGHT, SPD_VSLOW);
      s_o = S_CENTER_LEFT;
    }
    if (p == POS_CENTER) {
      step(degree_to_steps(2 * BASE_DEGREE), DIR_RIGHT, SPD_VSLOW);
      s_o = S_CENTER;
    }
    if (p == POS_CENTER_RIGHT) {
      step(degree_to_steps(3 * BASE_DEGREE), DIR_RIGHT, SPD_VSLOW);
      s_o = S_CENTER_RIGHT;
    }
    if (p == POS_RIGHT) {
      step(degree_to_steps(4 * BASE_DEGREE), DIR_RIGHT, SPD_VSLOW), s_o = S_RIGHT;
    }
  }

  if (s == S_CENTER_LEFT) {
    if (p == POS_LEFT) {
      step(degree_to_steps(BASE_DEGREE), DIR_LEFT, SPD_VSLOW), s_o = S_LEFT;
    }
    if (p == POS_CENTER_LEFT) {
      s_o = S_CENTER_LEFT;
    }
    if (p == POS_CENTER) {
      step(degree_to_steps(BASE_DEGREE), DIR_RIGHT, SPD_VSLOW);
      s_o = S_CENTER;
    }
    if (p == POS_CENTER_RIGHT) {
      step(degree_to_steps(2 * BASE_DEGREE), DIR_RIGHT, SPD_VSLOW);
      s_o = S_CENTER_RIGHT;
    }
    if (p == POS_RIGHT) {
      step(degree_to_steps(3 * BASE_DEGREE), DIR_RIGHT, SPD_VSLOW);
      s_o = S_RIGHT;
    }
  }

  if (s == S_CENTER) {
    if (p == POS_LEFT) {
      step(degree_to_steps(2 * BASE_DEGREE), DIR_LEFT, SPD_VSLOW), s_o = S_LEFT;
    }
    if (p == POS_CENTER_LEFT) {
      step(degree_to_steps(BASE_DEGREE), DIR_LEFT, SPD_VSLOW);
      s_o = S_CENTER_LEFT;
    }
    if (p == POS_CENTER) {
      s_o = S_CENTER;
    }
    if (p == POS_CENTER_RIGHT) {
      step(degree_to_steps(BASE_DEGREE), DIR_RIGHT, SPD_VSLOW);
      s_o = S_CENTER_RIGHT;
    }
    if (p == POS_RIGHT) {
      step(degree_to_steps(2 * BASE_DEGREE), DIR_RIGHT, SPD_VSLOW);
      s_o = S_RIGHT;
    }
  }

  if (s == S_CENTER_RIGHT) {
    if (p == POS_LEFT) {
      step(degree_to_steps(3 * BASE_DEGREE), DIR_LEFT, SPD_VSLOW);
      s_o = S_LEFT;
    }
    if (p == POS_CENTER_LEFT) {
      step(degree_to_steps(2 * BASE_DEGREE), DIR_LEFT, SPD_VSLOW);
      s_o = S_CENTER_LEFT;
    }
    if (p == POS_CENTER) {
      step(degree_to_steps(BASE_DEGREE), DIR_LEFT, SPD_VSLOW);
      s_o = S_CENTER;
    }
    if (p == POS_CENTER_RIGHT) {
      s_o = S_CENTER_RIGHT;
    }
    if (p == POS_RIGHT) {
      step(degree_to_steps(BASE_DEGREE), DIR_RIGHT, SPD_VSLOW);
      s_o = S_RIGHT;
    }
  }

  if (s == S_RIGHT) {
    if (p == POS_LEFT) {
      step(degree_to_steps(4 * BASE_DEGREE), DIR_LEFT, SPD_VSLOW);
      s_o = S_LEFT;
    }
    if (p == POS_CENTER_LEFT) {
      step(degree_to_steps(3 * BASE_DEGREE), DIR_LEFT, SPD_VSLOW);
      s_o = S_CENTER_LEFT;
    }
    if (p == POS_CENTER) {
      step(degree_to_steps(2 * BASE_DEGREE), DIR_LEFT, SPD_VSLOW);
      s_o = S_CENTER;
    }
    if (p == POS_CENTER_RIGHT) {
      step(degree_to_steps(BASE_DEGREE), DIR_LEFT, SPD_VSLOW);
      s_o = S_CENTER_RIGHT;
    }
    if (p == POS_RIGHT) {
      s_o = S_RIGHT;   
    }
  }

  stepper_enable(false);

  return s_o;
}

/**
   Calibrate back to the center
*/
int calibrate(int dir) {

  int CENTER_DEGREE = 90;
  
  Serial.println("Calibrating ...");
  //Serial.println(digitalRead(BUTTON_PIN_L));

  if (dir == S_LEFT) {
    //Go until you hit the left button
    while (digitalRead(BUTTON_PIN_L) == HIGH) {
      stepper_enable(true);
      step(10,DIR_LEFT,SPD_VSLOW);
      stepper_enable(false);
    }

    //Turn back 90 degree
    if (digitalRead(BUTTON_PIN_L) == LOW) {
      stepper_enable(true);
      step(degree_to_steps(CENTER_DEGREE),DIR_RIGHT,SPD_VSLOW);
      stepper_enable(false);
    }    
  }

  if (dir == S_RIGHT) {
    //Go until you hit the left button
    while (digitalRead(BUTTON_PIN_R) == HIGH) {
      stepper_enable(true);
      step(10,DIR_RIGHT,SPD_VSLOW);
      stepper_enable(false);
    }

    //Turn back 90 degree
    if (digitalRead(BUTTON_PIN_R) == LOW) {
      stepper_enable(true);
      step(degree_to_steps(CENTER_DEGREE),DIR_LEFT,SPD_VSLOW);
      stepper_enable(false);
    }    
  }

  return S_CENTER;  stepper_enable(false);
}

//Main loop
int s_curr=-1;

void loop() {
  
  //Calibrate
  if (s_curr == -1) {
    s_curr = calibrate(S_LEFT);
  }

  //Just position
  if (digitalRead(BUTTON_PIN_R) == HIGH && digitalRead(BUTTON_PIN_L) == HIGH) {
    
    //Detect the movement and react
    //int m = detect_movement();
    //Serial.println(MOV_STR[m]);
    //s_curr = follow_me(s_curr,m);
                   
    //Detect position
    int p = detect_pos();
    Serial.println(POS_STR[p]);
    s_curr = stare_at_me(s_curr,p);
  
  } else {
    if (digitalRead(BUTTON_PIN_R) == LOW) {
      Serial.println("Right OFF button pressed.");
      s_curr=calibrate(S_RIGHT);
    }
    if (digitalRead(BUTTON_PIN_L) == LOW) {
      Serial.println("Left OFF button pressed.");
      s_curr=calibrate(S_LEFT);
    }
    
    delay(1000);
  }
}
