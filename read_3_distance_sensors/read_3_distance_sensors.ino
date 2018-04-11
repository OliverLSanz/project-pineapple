#include <Wire.h>
#include <VL53L0X.h>

#define THRESHOLD 9000
#define USETHRESHOLD 0
#define DISTSENSOR_SCL A5
#define DISTSENSOR_SDA A4
#define DISTSENSOR_L_E 4
#define DISTSENSOR_FL_E 5
#define DISTSENSOR_FF_E 6
#define DISTSENSOR_FR_E 7
#define DISTSENSOR_R_E 8
VL53L0X sensor1;
VL53L0X sensor2;
VL53L0X sensor3;
VL53L0X sensor4;
VL53L0X sensor5;


void setup()
{
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);
  digitalWrite(4, LOW);
  digitalWrite(5, LOW);
  digitalWrite(6, LOW);
  digitalWrite(7, LOW);
  digitalWrite(8, LOW);
  delay(500);
  Wire.begin();

  Serial.begin (9600);
  
  pinMode(4, INPUT);
  delay(150);
  sensor1.init(true);
  delay(100);
  sensor1.setAddress((uint8_t)22);
  
  pinMode(5, INPUT);
  delay(150);
  sensor2.init(true);
  delay(100);
  sensor2.setAddress((uint8_t)23);
  
  pinMode(6, INPUT);
  delay(150);
  sensor3.init(true);
  delay(100);
  sensor3.setAddress((uint8_t)24);
  Serial.println("setting up sensor 4");

  pinMode(7, INPUT);
  delay(150);
  sensor4.init(true);
  delay(100);
  sensor4.setAddress((uint8_t)25);
  
  pinMode(8, INPUT);
  delay(150);
  sensor5.init(true);
  delay(100);
  sensor5.setAddress((uint8_t)26);
  Serial.println("done");

}

void loop()
{
  int time = millis();
  int read1 = sensor1.readRangeSingleMillimeters();
  int read2 = sensor2.readRangeSingleMillimeters();
  int read3 = sensor3.readRangeSingleMillimeters();
  int read4 = sensor4.readRangeSingleMillimeters();
  int read5 = sensor5.readRangeSingleMillimeters();
  time = millis() - time;
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
    if (sensor1.timeoutOccurred() || sensor2.timeoutOccurred() || sensor3.timeoutOccurred() || sensor4.timeoutOccurred() || sensor5.timeoutOccurred()) { Serial.print(" TIMEOUT"); }
 
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
    if (sensor1.timeoutOccurred() || sensor2.timeoutOccurred() || sensor3.timeoutOccurred() || sensor4.timeoutOccurred() || sensor5.timeoutOccurred()) { Serial.print(" TIMEOUT"); }
    
  }
  Serial.print("   TIME: ");
  Serial.print(time);
  Serial.println();
}
