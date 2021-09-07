//Pins to use
int PIN_SENSOR_M_TRIG=2;
int PIN_SENSOR_M_ECHO=3;

//Some other constants
int SONAR_DUR_US=10;
float SOUND_SPEED_FACTOR=0.017;

//Log output via the serial interface
void log(String msg) {
  Serial.println(msg);
}

//Logs a float value
void log_float(String name, float value) {
   String str = String(value,3);
   String msg = name + " = " + str;
   log(msg);
}

//Logs a long value
void log_long(String name, unsigned long value) {
   String str = String(value);
   String msg = name + " = " + str;
   log(msg);
}

//Setup
void setup() {
  Serial.begin(9600);
  log("-- sonar");
  pinMode(PIN_SENSOR_M_TRIG, OUTPUT);
  pinMode(PIN_SENSOR_M_ECHO, INPUT);
}


//Send a signal on the sensor on pin $id with a given duration
void sonar_send(int id, int dur) {
  log("Sending sonar signal ...");
  digitalWrite(id, HIGH);
  delayMicroseconds(dur);
  digitalWrite(id, LOW);
}

//Measure the echo 
float sonar_measure_dist_cm(int id) {
  unsigned long duration_us = pulseIn(id, HIGH);
  log_long("dur", duration_us);
  float dist = SOUND_SPEED_FACTOR * duration_us;
  log_float("dist", dist);
  return dist;
}

//Main event loop
void loop() {
  sonar_send(PIN_SENSOR_M_TRIG, SONAR_DUR_US);
  sonar_measure_dist_cm(PIN_SENSOR_M_ECHO);
  delay(500);
}
