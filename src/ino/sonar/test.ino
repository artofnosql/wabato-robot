const int TRIGGER_PIN_L=8;
const int ECHO_PIN_L=9;
const int TRIGGER_PIN_M=12;
const int ECHO_PIN_M=10;
const int TRIGGER_PIN_R=13;
const int ECHO_PIN_R=11;
const int MAX_D=100;
const char* POS_STR[] = {"NONE", "LEFT", "CENTER_LEFT", "CENTER", "CENTER_RIGHT", "RIGHT", "ALL", "UNDEFINED"};
const char* MOV_STR[] = {"NONE", "LEFT", "RIGHT"};

//Init a sensor
void init(int trig_id, int echo_id) {
  pinMode(trig_id, OUTPUT);
  pinMode(echo_id, INPUT);
}


//Setup
void setup() {
  Serial.begin(9600);
  init(TRIGGER_PIN_L, ECHO_PIN_L);
  init(TRIGGER_PIN_M, ECHO_PIN_M);
  init(TRIGGER_PIN_R, ECHO_PIN_R);
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
 * Check if a specific sensor sees an observer by      if ()} {
      }

 * checking if the distance is lower then MAX_D
 */
boolean seen(long dist) {
  
  if ((dist > 0) && (dist < MAX_D)) {
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

  //There is a movement if a state transistions from left to right or right to left within a period of 3 seconds
  
  //Take 30 samples
  long samples[30];

  boolean from_left=false;
  boolean from_right=false;
  int start_idx=0;
  
  for (int i=0; i < 30; i++) {
    long d_l = dist(TRIGGER_PIN_L, ECHO_PIN_L);
    delay(10);
    long d_m = dist(TRIGGER_PIN_M, ECHO_PIN_M);
    delay(10);
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
         
    delay(80);
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





//Main loop
void loop() {

  //Detect the movement and react
  int m = detect_movement();
  Serial.println(MOV_STR[m]);

  
  //Adjust
  int p = detect_pos();
  Serial.println(POS_STR[p]);
    
  delay(500);
}
