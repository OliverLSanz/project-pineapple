#include <IRremote.h>
#include <Wire.h>
#include <VL53L0X.h>

// SENSORS
#define CNY_UL A0
#define CNY_UR A1
#define CNY_DL A2
#define CNY_DR A3
#define IR_RECEIVER 9
#define BUTTON 10
#define DISTSENSOR_SCL A5
#define DISTSENSOR_SDA A4
#define DISTSENSOR_L_E 4
#define DISTSENSOR_FL_E 5
#define DISTSENSOR_FF_E 6
#define DISTSENSOR_FR_E 7
#define DISTSENSOR_R_E 8

// ACTUATORS
#define LED 13
#define MOTORDRIVER_L_EN A7 
#define MOTORDRIVER_L_N1 2
#define MOTORDRIVER_L_N2 3
#define MOTORDRIVER_R_EN A6 
#define MOTORDRIVER_R_N1 11
#define MOTORDRIVER_R_N2 12

// DISTANCE SENSORS
VL53L0X distSensor_L;
VL53L0X distSensor_FL;
VL53L0X distSensor_FF;
VL53L0X distSensor_FR;
VL53L0X distSensor_R;

// IR REVEIVER
IRrecv irrecv(IR_RECEIVER);
decode_results results;

// MOTOR CONTROL FUNCTIONS
void driversOutput(uint8_t l_en, uint8_t l_n1, uint8_t l_n2, uint8_t r_en, uint8_t r_n1, uint8_t r_n2) {
  analogWrite(MOTORDRIVER_L_EN, l_en);
  digitalWrite(MOTORDRIVER_L_N1, l_n1);
  digitalWrite(MOTORDRIVER_L_N2, l_n2);
  analogWrite(MOTORDRIVER_R_EN, r_en);
  digitalWrite(MOTORDRIVER_R_N1, r_n1);
  digitalWrite(MOTORDRIVER_R_N2, r_n2);
}

void rest() {
  driversOutput(0, HIGH, HIGH, 0, HIGH, HIGH);
}

void forward(uint8_t motors_speed) {
  driversOutput(motors_speed, HIGH, LOW, motors_speed, LOW, HIGH);
}

void reverse(uint8_t motors_speed) {
  driversOutput(motors_speed, LOW, HIGH, motors_speed, HIGH, LOW);
}

void turnForward(uint8_t left_speed, uint8_t right_speed) {
  driversOutput(left_speed, HIGH, LOW, right_speed, HIGH, LOW);
}

void turnReverse(uint8_t left_speed, uint8_t right_speed) {
  driversOutput(left_speed, LOW, HIGH, right_speed, LOW, HIGH);
}

void brake() {
  driversOutput(0, LOW, LOW, 0, LOW, LOW);
}

// SETUP FUNCTIONS
void distanceSensorsSetup() {
  Serial.println("Setting up distance sensors");
  int distanceSensorEnables[5] = { DISTSENSOR_L_E, DISTSENSOR_FL_E,DISTSENSOR_FF_E, DISTSENSOR_FR_E, DISTSENSOR_L_E };
  VL53L0X distSensors[5] = { distSensor_L, distSensor_FL, distSensor_FF, distSensor_FR, distSensor_R };

  for(int i = 0; i < 5; i++) {
    pinMode(distanceSensorEnables[i], OUTPUT);
    digitalWrite(distanceSensorEnables[i], LOW);
  }

  delay(500);
  Serial.println("Begginning Wire");
  Wire.begin();
  Serial.println("Wire initialised");

  for(int i = 0; i < 5; i++) {
      pinMode(distanceSensorEnables[i], INPUT);
      delay(150);
      Serial.println("Starting ditance sensor");
      distSensors[i].init(true);
      Serial.println("Distance sensor started");
      delay(100);
      Serial.println("Setting distance sensor address");
      distSensors[i].setAddress((uint8_t)10+i);
  }
}

void motorsSetup() {
  pinMode(MOTORDRIVER_L_N1, OUTPUT);
  pinMode(MOTORDRIVER_L_N2, OUTPUT);
  pinMode(MOTORDRIVER_R_N1, OUTPUT);
  pinMode(MOTORDRIVER_R_N2, OUTPUT);
  rest();
}

// TEST FUNCTIONS
void testMotors() {
  forward(150);
  delay(1000);
  rest();
  delay(1000);
  reverse(150);
  delay(1000);
  rest();
  delay(5000);
}

void testIR() {
  if (irrecv.decode(&results)) {
    Serial.println(results.value, HEX);
    irrecv.resume(); // Receive the next value
  }
  delay(100);
}

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
  int read1 = distSensor_L.readRangeSingleMillimeters();
  int read2 = distSensor_FL.readRangeSingleMillimeters();
  int read3 = distSensor_FF.readRangeSingleMillimeters();
  int read4 = distSensor_FR.readRangeSingleMillimeters();
  int read5 = distSensor_R.readRangeSingleMillimeters();

  int THRESHOLD = 500;
  bool USETHRESHOLD = 1;
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
  int readcny1 = analogRead(CNY_UL);
  int readcny2 = analogRead(CNY_UR);
  int readcny3 = analogRead(CNY_DL);
  int readcny4 = analogRead(CNY_DR);
  Serial.print(readcny1);
  Serial.print(" -- ");
  Serial.print(readcny2);
  Serial.print(" -- ");
  Serial.print(readcny3);
  Serial.print(" -- ");
  Serial.print(readcny4);
  Serial.println("");
}

// SETUP
void setup() {
  // Setups
  Serial.begin(9600);            // Serial setup
  pinMode(LED, OUTPUT);          // Led setup
  pinMode(BUTTON, INPUT_PULLUP); // Button setup
  irrecv.enableIRIn();           // IR reveiver setup
  //distanceSensorsSetup();
  motorsSetup();
  
  // Wait for the Ready Player One button to be pressed
//  while(digitalRead(BUTTON) == HIGH){
//    delay(10);
//  }
  
  // Wait n secs for the combat to begin
//  delay(3000);
  
  // FIGHT!!
}

// LOOP
void loop() {
  testMotors();
  // Check sensors and set up the necessary flags
  // Act in consecuence of flags and current state
}
