#include <IRremote.h>
#include <Wire.h>
#include <VL53L0X.h>

// CONFIGURATION
#define CONSISTENCY_THRESHOLD 1
#define CONSISTENT_DISTANCE_DIFFERENCE 500
#define TOP_SPEED 100
#define CNYTHRESHOLD 700
#define SEEN 900
#define FAR 500
#define NEAR 200
#define MOTORDELAY 10

// NAMES
#define LEFT 0
#define RIGHT 1
#define NONE 2

// SENSOR PINS
#define CNY_BL A0
#define CNY_FL A1
#define CNY_FR A2
#define CNY_BR A3
#define IR_RECEIVER 9
#define BUTTON 10
#define DISTSENSOR_L_E 4
#define DISTSENSOR_FL_E 7
#define DISTSENSOR_FF_E 2
#define DISTSENSOR_FR_E 12
#define DISTSENSOR_R_E 8
#define DISTSENSOR_SCL A5
#define DISTSENSOR_SDA A4

// ACTUATOR PINS
#define LED 13
#define RIGHT_FORWARD 3
#define RIGHT_BACKWARD 5
#define LEFT_FORWARD 11
#define LEFT_BACKWARD 6

// FREE PINS
#define FREE_1 A6
#define FREE_2 A7

// DISTANCE SENSORS
VL53L0X distSensor_L;
VL53L0X distSensor_FL;
VL53L0X distSensor_FF;
VL53L0X distSensor_FR;
VL53L0X distSensor_R;

// IR RECEIVER
IRrecv irrecv(IR_RECEIVER);
decode_results results;

// STATE VARIABLES
int leftS = 0; 
int rightS = 0;
int enemyPosition = NONE;
int distanceNow[5] = {0, 0, 0, 0, 0};
int distanceBefore[5] = {0, 0, 0, 0, 0};
unsigned long consistency[5] = {0, 0, 0, 0, 0}; 

// UTILS
int i;
void copy(int* src, int* dst, int len) {
    memcpy(dst, src, sizeof(src[0])*len);
}

// SETUP FUNCTIONS
void distanceSensorsSetup() {
  pinMode(DISTSENSOR_L_E, OUTPUT);
  pinMode(DISTSENSOR_FL_E, OUTPUT);
  pinMode(DISTSENSOR_FF_E, OUTPUT);
  pinMode(DISTSENSOR_FR_E, OUTPUT);
  pinMode(DISTSENSOR_R_E, OUTPUT);
  digitalWrite(DISTSENSOR_L_E, LOW);
  digitalWrite(DISTSENSOR_FL_E, LOW);
  digitalWrite(DISTSENSOR_FF_E, LOW);
  digitalWrite(DISTSENSOR_FR_E, LOW);
  digitalWrite(DISTSENSOR_R_E, LOW);
  delay(500);
  Wire.begin();

  Serial.begin (9600);
  
  pinMode(DISTSENSOR_L_E, INPUT);
  delay(150);
  distSensor_L.init(true);
  delay(100);
  distSensor_L.setAddress((uint8_t)22);
  distSensor_L.startContinuous();
  
  pinMode(DISTSENSOR_FL_E, INPUT);
  delay(150);
  distSensor_FL.init(true);
  delay(100);
  distSensor_FL.setAddress((uint8_t)23);
  distSensor_FL.startContinuous();
  
  pinMode(DISTSENSOR_FF_E, INPUT);
  delay(150);
  distSensor_FF.init(true);
  delay(100);
  distSensor_FF.setAddress((uint8_t)24);
  distSensor_FF.startContinuous();

  pinMode(DISTSENSOR_FR_E, INPUT);
  delay(150);
  distSensor_FR.init(true);
  delay(100);
  distSensor_FR.setAddress((uint8_t)25);
  distSensor_FR.startContinuous();
  
  pinMode(DISTSENSOR_R_E, INPUT);
  delay(150);
  distSensor_R.init(true);
  delay(100);
  distSensor_R.setAddress((uint8_t)26);
  distSensor_R.startContinuous();
}

void motorsSetup() {
  setMotorsSpeed(0, 0);
}

// TEST FUNCTIONS
void testLED() {
  digitalWrite(LED, HIGH);
  delay(1000);
  digitalWrite(LED, LOW);
  delay(1000);
}

void testButton() {
  digitalWrite(LED, digitalRead(BUTTON));
  delay(100);
}

void testDistanceSensor() {
    // Distance sensors test
  int read1 = distSensor_L.readRangeContinuousMillimeters();
  int read2 = distSensor_FL.readRangeContinuousMillimeters();
  int read3 = distSensor_FF.readRangeContinuousMillimeters();
  int read4 = distSensor_FR.readRangeContinuousMillimeters();
  int read5 = distSensor_R.readRangeContinuousMillimeters();

  int THRESHOLD = 500;
  bool USETHRESHOLD = 0;
  if(USETHRESHOLD){
    bool t1 = read1 < THRESHOLD;
    bool t2 = read2 < THRESHOLD;
    bool t3 = read3 < THRESHOLD;
    bool t4 = read4 < THRESHOLD;
    bool t5 = read5 < THRESHOLD;
  
    Serial.print("Readings: ");
    Serial.print(t1);
    Serial.print(" -- ");
    Serial.print(t2);
    Serial.print(" -- ");
    Serial.print(t3);
    Serial.print(" -- ");
    Serial.print(t4);
    Serial.print(" -- ");
    Serial.print(t5);
    if (distSensor_L.timeoutOccurred() || distSensor_FL.timeoutOccurred() || distSensor_FF.timeoutOccurred() || distSensor_FR.timeoutOccurred() || distSensor_R.timeoutOccurred()) { Serial.print(" TIMEOUT"); }
  
    Serial.println();
  }else{
    Serial.print("Readings: ");
    Serial.print(read1);
    Serial.print(" -- ");
    Serial.print(read2);
    Serial.print(" -- ");
    Serial.print(read3);
    Serial.print(" -- ");
    Serial.print(read4);
    Serial.print(" -- ");
    Serial.print(read5);
    if (distSensor_L.timeoutOccurred() || distSensor_FL.timeoutOccurred() || distSensor_FF.timeoutOccurred() || distSensor_FR.timeoutOccurred() || distSensor_R.timeoutOccurred()) { Serial.print(" TIMEOUT"); }
    Serial.println();
  }  
}

void testCNYs() {
  int readcny1 = analogRead(CNY_FL);
  int readcny2 = analogRead(CNY_FR);
  int readcny3 = analogRead(CNY_BL);
  int readcny4 = analogRead(CNY_BR);
  Serial.print(readcny1);
  Serial.print(" -- ");
  Serial.print(readcny2);
  Serial.print(" -- ");
  Serial.print(readcny3);
  Serial.print(" -- ");
  Serial.print(readcny4);
  Serial.println("");
}

// MOTOR CONTROL FUNCTIONS
void setMotorsSpeed(int left, int right) {
  if(right < 0){
    analogWrite(RIGHT_FORWARD, 0);
  }else{
    analogWrite(RIGHT_BACKWARD, 0);
  }

  if(left < 0){
    analogWrite(LEFT_FORWARD, 0);
  }else{
    analogWrite(LEFT_BACKWARD, 0);
  }

  delay(MOTORDELAY);

  if(right < 0){
    analogWrite(RIGHT_BACKWARD, abs(right));
  }else{
    analogWrite(RIGHT_FORWARD, right);
  }

  if(left < 0){
    analogWrite(LEFT_BACKWARD, abs(left));
  }else{
    analogWrite(LEFT_FORWARD, left);
  }

  rightS = right;
  leftS = left;
}

// SENSOR FUNCTIONS
void checkIR() {
  if (irrecv.decode(&results)) {
    if(results.value == 0xF62 || results.value == 0x762) {
      setMotorsSpeed(0, 0);
      while(true) delay(1000);
    }
    irrecv.resume(); // Receive the next value
  }
}

bool cnySeesWhite(uint8_t CNY) {
   int r = analogRead(CNY);
   if (r >= CNYTHRESHOLD){
    return true;
   }
   return false;  
}

void updateDistances(){
  copy(distanceNow, distanceBefore, 5);
  
  distanceNow[0] = distSensor_L.readRangeContinuousMillimeters();
  distanceNow[1] = distSensor_FL.readRangeContinuousMillimeters();
  distanceNow[2] = distSensor_FF.readRangeContinuousMillimeters();
  distanceNow[3] = distSensor_FR.readRangeContinuousMillimeters();
  distanceNow[4] = distSensor_R.readRangeContinuousMillimeters();

  for(i = 0; i < 5; i++){
    if(distanceNow[i] < SEEN
    && distanceNow[i] < distanceBefore[i] + CONSISTENT_DISTANCE_DIFFERENCE
    && distanceNow[i] > distanceBefore[i] - CONSISTENT_DISTANCE_DIFFERENCE)
      consistency[i] += 1;
    else
      consistency[i] = 0;
  }
}

// SETUP
void setup() {
  // Setups
  Serial.begin(9600);
  pinMode(LED, OUTPUT);
  pinMode(BUTTON, INPUT_PULLUP);
  irrecv.enableIRIn();
  distanceSensorsSetup();
  motorsSetup();
  
  // Wait for the Ready Player One button to be pressed
  digitalWrite(LED, HIGH);
  while(digitalRead(BUTTON) == HIGH){
    delay(10);
  }
  digitalWrite(LED, LOW); 
  delay(1000);

  // INITIAL STATE
  setMotorsSpeed(0, 0);
}

bool seen;
// LOOP
void loop() {
  checkIR();
  updateDistances();
  seen = false;
  if(leftS > 0 && rightS > 0 && (cnySeesWhite(CNY_FL) || cnySeesWhite(CNY_FR))){
    setMotorsSpeed(-60, -60);
  }else if(leftS < 0 && rightS < 0 && (cnySeesWhite(CNY_BL) || cnySeesWhite(CNY_BR))){
    setMotorsSpeed(60, 60);
  }else if(distanceNow[2] < SEEN && consistency[2] > CONSISTENCY_THRESHOLD){
    setMotorsSpeed(0, 0);
    seen = true;
    enemyPosition = NONE;
  }else if(distanceNow[1] < NEAR && consistency[1] > CONSISTENCY_THRESHOLD){
    setMotorsSpeed(-TOP_SPEED, TOP_SPEED);
    enemyPosition = LEFT;
  }else if(distanceNow[1] < FAR && consistency[1] > CONSISTENCY_THRESHOLD){
    setMotorsSpeed(-TOP_SPEED*0.75, TOP_SPEED*0.75);
    enemyPosition = LEFT;
  }else if(distanceNow[1] < SEEN && consistency[1] > CONSISTENCY_THRESHOLD){
    setMotorsSpeed(-TOP_SPEED/2, TOP_SPEED/2);
    enemyPosition = LEFT;
  }else if(distanceNow[3] < NEAR && consistency[3] > CONSISTENCY_THRESHOLD){
    setMotorsSpeed(TOP_SPEED, -TOP_SPEED);
    enemyPosition = RIGHT;
  }else if(distanceNow[3] < FAR && consistency[3] > CONSISTENCY_THRESHOLD){
    setMotorsSpeed(TOP_SPEED*0.75, -TOP_SPEED*0.75);
    enemyPosition = RIGHT;
  }else if(distanceNow[3] < SEEN && consistency[3] > CONSISTENCY_THRESHOLD){
    setMotorsSpeed(TOP_SPEED/2, -TOP_SPEED/2);
    enemyPosition = RIGHT;
  }else if(distanceNow[4] < NEAR && consistency[4] > CONSISTENCY_THRESHOLD){
    setMotorsSpeed(TOP_SPEED, -TOP_SPEED);
    enemyPosition = RIGHT;
  }else if(distanceNow[4] < FAR && consistency[4] > CONSISTENCY_THRESHOLD){
    setMotorsSpeed(TOP_SPEED, -TOP_SPEED);
    enemyPosition = RIGHT;
  }else if(distanceNow[4] < SEEN && consistency[4] > CONSISTENCY_THRESHOLD){
    setMotorsSpeed(TOP_SPEED, -TOP_SPEED);
    enemyPosition = RIGHT;
  }else if(distanceNow[0] < NEAR && consistency[0] > CONSISTENCY_THRESHOLD){
    setMotorsSpeed(-TOP_SPEED, TOP_SPEED);
    enemyPosition = LEFT;
  }else if(distanceNow[0] < FAR && consistency[0] > CONSISTENCY_THRESHOLD){
    setMotorsSpeed(-TOP_SPEED, TOP_SPEED);
    enemyPosition = LEFT;
  }else if(distanceNow[0] < SEEN && consistency[0] > CONSISTENCY_THRESHOLD){
    setMotorsSpeed(-TOP_SPEED, TOP_SPEED);
    enemyPosition = LEFT;
  }else if(enemyPosition == RIGHT){
    setMotorsSpeed(TOP_SPEED, -TOP_SPEED);
  }else if(enemyPosition == LEFT){
    setMotorsSpeed(-TOP_SPEED, TOP_SPEED);
  }

  if(seen){
    digitalWrite(LED, HIGH);
  }else{
    digitalWrite(LED, LOW);
  }
}
