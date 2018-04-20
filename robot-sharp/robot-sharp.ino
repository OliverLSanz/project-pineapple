#include <IRremote.h>
#include <Wire.h>
#include <SharpIR.h>
#include "FastRunningMedian.h"

// CONFIGURATION
#define CONSISTENCY_THRESHOLD 7
#define TOP_SPEED 100
#define CNYTHRESHOLD 500
#define MOTORDELAY 10
#define SHARP_WINDOW_SIZE 3
#define SEEN 70

// NAMES
#define LEFT 0
#define RIGHT 1
#define FRONT 2

// SENSOR PINS
#define CNY_BL A0
#define CNY_FL A1
#define CNY_FR A2
#define CNY_BR A3
#define IR_RECEIVER 9
#define BUTTON 10
#define SHARP_L A6 
#define SHARP_FL A7
#define SHARP_FR A4
#define SHARP_R A5
 
// ACTUATOR PINS
#define LED 13
#define RIGHT_FORWARD 3
#define RIGHT_BACKWARD 5
#define LEFT_FORWARD 11
#define LEFT_BACKWARD 6

// DISTANCE SENSORS
SharpIR sharp_L(GP2Y0A21YK0F, SHARP_L);
SharpIR sharp_FL(GP2Y0A21YK0F, SHARP_FL);
SharpIR sharp_FR(GP2Y0A21YK0F, SHARP_FR);
SharpIR sharp_R(GP2Y0A21YK0F, SHARP_R);
FastRunningMedian<unsigned int, SHARP_WINDOW_SIZE, 81> median_L;
FastRunningMedian<unsigned int, SHARP_WINDOW_SIZE, 81> median_FL;
FastRunningMedian<unsigned int, SHARP_WINDOW_SIZE, 81> median_FR;
FastRunningMedian<unsigned int, SHARP_WINDOW_SIZE, 81> median_R;

// IR RECEIVER
IRrecv irrecv(IR_RECEIVER);
decode_results results;

// STATE VARIABLES
int leftS = 0; 
int rightS = 0;
int distanceNow[4] = {0, 0, 0, 0};
int distanceBefore[4] = {0, 0, 0, 0};
unsigned long consistency[4] = {0, 0, 0, 0}; 
int enemyPosition;

// UTILS
int i;

void copy(int* src, int* dst, int len) {
    memcpy(dst, src, sizeof(src[0])*len);
}

int max_index(unsigned long *a, int n)
    {
        if(n <= 0) return -1;
        int i, max_i = 0;
        float maxvalue = a[0];
        for(i = 1; i < n; ++i){
            if(a[i] > maxvalue){
                maxvalue = a[i];
                max_i = i;
            }
        }
        if(maxvalue < CONSISTENCY_THRESHOLD)
          return -1;
        return max_i;
    }

// SETUP FUNCTIONS
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
  copy(distanceNow, distanceBefore, 4);

//  median_L.addValue(sharp_L.getDistance());
//  median_FL.addValue(sharp_FL.getDistance());
//  median_FR.addValue(sharp_FR.getDistance());
//  median_R.addValue(sharp_R.getDistance());
  
//  distanceNow[0] = median_L.getMedian();
//  distanceNow[1] = median_FL.getMedian();
//  distanceNow[2] = median_FR.getMedian();
//  distanceNow[3] = median_R.getMedian();

  distanceNow[0] = sharp_L.getDistance();
  distanceNow[1] = sharp_FL.getDistance();
  distanceNow[2] = sharp_FR.getDistance();
  distanceNow[3] = sharp_R.getDistance();

    // Consistency update
  for(i = 0; i < 4; i++){
    if(distanceNow[i] < SEEN)
      consistency[i] += 1;
    else
      consistency[i] = 0;
  }
}

int timer;
// SETUP
void setup() {
  // Setups
  enemyPosition = RIGHT;
  Serial.begin(9600);
  pinMode(LED, OUTPUT);
  pinMode(BUTTON, INPUT_PULLUP);
  irrecv.enableIRIn();
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
  timer = millis();
}

void printConsistency(){
for(i = 0; i < 4; i++){
    Serial.print(distanceNow[i]);
    Serial.print(" ");
  }

  Serial.print(" - ");
  
  for(i = 0; i < 4; i++){
    Serial.print(distanceNow[i] < 78);
    Serial.print(" ");
  }

  Serial.print(" - ");

  for(i = 0; i < 4; i++){
    Serial.print(consistency[i]);
    Serial.print(" ");
  }

  Serial.print(" - ");

  Serial.print(max_index(consistency, 4));
}

bool seen = false;
int best_read;
bool charge = false;
long sharp_disable = 0;
unsigned long elapsed = 0;
// LOOP
void loop() {
  checkIR();
  updateDistances();
  printConsistency();
  Serial.print(" - ");
  Serial.println(enemyPosition);
  best_read = max_index(consistency, 4);
  elapsed = millis() - timer;
  timer = millis();
  seen = false;
  if(leftS > 0 && rightS > 0 && cnySeesWhite(CNY_FL)){
    setMotorsSpeed(-100, -210);
    sharp_disable = 400;
    charge = false;
    enemyPosition = RIGHT;
  }else if(leftS > 0 && rightS > 0 && cnySeesWhite(CNY_FR)){
    setMotorsSpeed(-210, -100);
    charge = false;
    sharp_disable = 400;
    enemyPosition = LEFT;
  }else if(leftS > 0 && rightS > 0 && (cnySeesWhite(CNY_BL) || cnySeesWhite(CNY_BR))){
    setMotorsSpeed(210, 210);
    charge = false;
  }else if(leftS < 0 && rightS < 0 && cnySeesWhite(CNY_BL)){
    setMotorsSpeed(210, 100);
    charge = false;
    sharp_disable = 400;
    enemyPosition = LEFT;
  }else if(leftS < 0 && rightS < 0 && cnySeesWhite(CNY_BR)){
    setMotorsSpeed(100, 210);
    charge = false;
    sharp_disable = 400;
    enemyPosition = RIGHT;
  }else if(leftS < 0 && rightS < 0 && (cnySeesWhite(CNY_FL) || cnySeesWhite(CNY_FR))){
    setMotorsSpeed(-200, -200);
    charge = false;
  }else if(sharp_disable > 0){
    sharp_disable -= elapsed;
  }else if(charge){
    if(consistency[1] > 1 && consistency[2] > 1){
      enemyPosition = FRONT;
      setMotorsSpeed(210, 210);
    }else if(consistency[1] > 1 && consistency[2] < 1){
      setMotorsSpeed(100, 210);
      enemyPosition = LEFT;
    }else if(consistency[2] > 1 && consistency[1] < 1){
      setMotorsSpeed(210, 100);
      enemyPosition = RIGHT;
    }else if(enemyPosition == LEFT){
      setMotorsSpeed(100, 210);
    }else if(enemyPosition == RIGHT){
      setMotorsSpeed(210, 100);
    }else{
      setMotorsSpeed(210, 210);
    }
  }else if(consistency[1] > 2 && consistency[2] > 2){
    setMotorsSpeed(210, 210);
    charge = true;
    enemyPosition = FRONT;
  }else if(consistency[1] > 2){
    setMotorsSpeed(100, 210);
    charge = true;
    enemyPosition = LEFT;
  }else if(consistency[2] > 2){
    setMotorsSpeed(210, 100);
    charge = true;
    enemyPosition = RIGHT;
  }else if(consistency[0] > 3 && consistency[0] > consistency[3]){
    setMotorsSpeed(-200, 200);
    enemyPosition = LEFT;
  }else if(consistency[3] > 3){
    setMotorsSpeed(200, -200);
    enemyPosition = RIGHT;
  }else if(enemyPosition == RIGHT){
    setMotorsSpeed(200, -200);
  }else if(enemyPosition == LEFT){
    setMotorsSpeed(-200, 200);
  }
  
  if(seen){
    digitalWrite(LED, HIGH);
  }else{
    digitalWrite(LED, LOW);
  }
}
