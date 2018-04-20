#include <IRremote.h>
#include <Wire.h>
#include <SharpDistSensor.h>

// CONFIGURATION
#define CONSISTENCY_THRESHOLD 1
#define CONSISTENT_DISTANCE_DIFFERENCE 500
#define TOP_SPEED 100
#define CNYTHRESHOLD 700
#define MOTORDELAY 10
#define SHARP_WINDOW_SIZE 5

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
#define SHARP_L A5 
#define SHARP_FL A4
#define SHARP_FR A7
#define SHARP_R A6
 
// ACTUATOR PINS
#define LED 13
#define RIGHT_FORWARD 3
#define RIGHT_BACKWARD 5
#define LEFT_FORWARD 11
#define LEFT_BACKWARD 6

// DISTANCE SENSORS
SharpDistSensor sharp_L(SHARP_L, SHARP_WINDOW_SIZE);
SharpDistSensor sharp_FL(SHARP_FL, SHARP_WINDOW_SIZE);
SharpDistSensor sharp_FR(SHARP_FR, SHARP_WINDOW_SIZE);
SharpDistSensor sharp_R(SHARP_R, SHARP_WINDOW_SIZE);

// IR RECEIVER
IRrecv irrecv(IR_RECEIVER);
decode_results results;

// STATE VARIABLES
int leftS = 0; 
int rightS = 0;
int enemyPosition = NONE;
int distanceNow[4] = {0, 0, 0, 0};
int distanceBefore[4] = {0, 0, 0, 0};
unsigned long consistency[4] = {0, 0, 0, 0}; 

// UTILS
int i;
void copy(int* src, int* dst, int len) {
    memcpy(dst, src, sizeof(src[0])*len);
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
  
  distanceNow[0] = sharp_L.getDist();
  distanceNow[1] = sharp_FL.getDist();
  distanceNow[2] = sharp_FR.getDist();
  distanceNow[3] = sharp_R.getDist();

    // Consistency update
//  for(i = 0; i < 4; i++){
//    if(distanceNow[i] < SEEN
//    && distanceNow[i] < distanceBefore[i] + CONSISTENT_DISTANCE_DIFFERENCE
//    && distanceNow[i] > distanceBefore[i] - CONSISTENT_DISTANCE_DIFFERENCE)
//      consistency[i] += 1;
//    else
//      consistency[i] = 0;
//  }
}

int timer;
// SETUP
void setup() {
  // Setups
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
    Serial.print(distanceNow[i] > 400 || distanceNow[i] < 200);
    Serial.print(" ");
  }
}

bool seen = false;
// LOOP
void loop() {
  checkIR();
  updateDistances();
  printConsistency();
  Serial.println("");

//  seen = false;
//  if(leftS > 0 && rightS > 0 && (cnySeesWhite(CNY_FL) || cnySeesWhite(CNY_FR))){
//    setMotorsSpeed(-100, -100);
//    Serial.println("CNY1");
//  }else if(leftS < 0 && rightS < 0 && (cnySeesWhite(CNY_BL) || cnySeesWhite(CNY_BR))){
//    setMotorsSpeed(100, 100);
//    Serial.println("CNY2");
//  }else if(distanceNow[2] < SEEN && consistency[2] > CONSISTENCY_THRESHOLD){
//    setMotorsSpeed(0, 0);
//    seen = true;
//    enemyPosition = NONE;
//    Serial.println("F->Me quedo quieto y reseteo su posición");
//  }else if(distanceNow[1] < NEAR && consistency[1] > CONSISTENCY_THRESHOLD){
//    setMotorsSpeed(-TOP_SPEED, TOP_SPEED);
//    enemyPosition = LEFT;
//    Serial.println("FI c->Giro hacia allí y posicion = izquierda");
//  }else if(distanceNow[1] < FAR && consistency[1] > CONSISTENCY_THRESHOLD){
//    setMotorsSpeed(-TOP_SPEED*0.75, TOP_SPEED*0.75);
//    enemyPosition = LEFT;
//    Serial.println("FI m->Giro hacia allí y posición = izquierda");
//  }else if(distanceNow[1] < SEEN && consistency[1] > CONSISTENCY_THRESHOLD){
//    setMotorsSpeed(-TOP_SPEED/2, TOP_SPEED/2);
//    enemyPosition = LEFT;
//    Serial.println("FI l->Giro hacia allí y posición izquierda");
//  }else if(distanceNow[3] < NEAR && consistency[3] > CONSISTENCY_THRESHOLD){
//    setMotorsSpeed(TOP_SPEED, -TOP_SPEED);
//    enemyPosition = RIGHT;
//    Serial.println("FD c->Giro hacia allí y posición derecha");
//  }else if(distanceNow[3] < FAR && consistency[3] > CONSISTENCY_THRESHOLD){
//    setMotorsSpeed(TOP_SPEED*0.75, -TOP_SPEED*0.75);
//    enemyPosition = RIGHT;
//    Serial.println("FD m->Giro hacia allí y posición derecha");
//  }else if(distanceNow[3] < SEEN && consistency[3] > CONSISTENCY_THRESHOLD){
//    setMotorsSpeed(TOP_SPEED/2, -TOP_SPEED/2);
//    enemyPosition = RIGHT;
//    Serial.println("FD l->Giro hacia allí y posición derecha");
//  }else if(distanceNow[4] < NEAR && consistency[4] > CONSISTENCY_THRESHOLD){
//    setMotorsSpeed(TOP_SPEED, -TOP_SPEED);
//    enemyPosition = RIGHT;
//    Serial.println("D  c->Giro hacia allí y posición derecha");
//  }else if(distanceNow[4] < FAR && consistency[4] > CONSISTENCY_THRESHOLD){
//    setMotorsSpeed(TOP_SPEED, -TOP_SPEED);
//    enemyPosition = RIGHT;
//    Serial.println("D  m->giro hacia allí y posición derecha");
//  }else if(distanceNow[4] < SEEN && consistency[4] > CONSISTENCY_THRESHOLD){
//    setMotorsSpeed(TOP_SPEED, -TOP_SPEED);
//    enemyPosition = RIGHT;
//    Serial.println("D  l->Giro hacia allí y posición derecha");
//  }else if(distanceNow[0] < NEAR && consistency[0] > CONSISTENCY_THRESHOLD){
//    setMotorsSpeed(-TOP_SPEED, TOP_SPEED);
//    enemyPosition = LEFT;
//    Serial.println("I  c->Giro hacia allí y posición izquierda");
//  }else if(distanceNow[0] < FAR && consistency[0] > CONSISTENCY_THRESHOLD){
//    setMotorsSpeed(-TOP_SPEED, TOP_SPEED);
//    enemyPosition = LEFT;
//    Serial.println("I  m->Giro hacia allí y posición izquierda");
//  }else if(distanceNow[0] < SEEN && consistency[0] > CONSISTENCY_THRESHOLD){
//    setMotorsSpeed(-TOP_SPEED, TOP_SPEED);
//    enemyPosition = LEFT;
//    Serial.println("I  l->Giro hacia allí y posición izquierda");
//  }else if(enemyPosition == RIGHT){
//    setMotorsSpeed(TOP_SPEED, -TOP_SPEED);
//    Serial.println("XD->Giro hacia allí");
//  }else if(enemyPosition == LEFT){
//    setMotorsSpeed(-TOP_SPEED, TOP_SPEED);
//    Serial.println("XI->Giro hacia allí");
//  }else{
//    Serial.println("No veo nada y no sé donde está->No cambio nada");
//  }
//
//  if(seen){
//    digitalWrite(LED, HIGH);
//  }else{
//    digitalWrite(LED, LOW);
//  }
}
