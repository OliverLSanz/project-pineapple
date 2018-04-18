#include <Wire.h>
#include <VL53L0X.h>

#define DISTSENSOR_L_E 4
#define DISTSENSOR_FL_E A6
#define DISTSENSOR_FF_E A7
#define DISTSENSOR_FR_E 7
#define DISTSENSOR_R_E 8

VL53L0X sensor;

void setup()
{
  pinMode(DISTSENSOR_L_E, OUTPUT);
  pinMode(DISTSENSOR_FL_E, INPUT);
  pinMode(DISTSENSOR_FF_E, INPUT);
  pinMode(DISTSENSOR_FR_E, OUTPUT);
  pinMode(DISTSENSOR_R_E, OUTPUT);
  digitalWrite(DISTSENSOR_L_E, LOW);
  digitalWrite(DISTSENSOR_FR_E, LOW);
  digitalWrite(DISTSENSOR_R_E, LOW);
  

  delay(500);
  Wire.begin();

  Serial.begin (9600);
  delay(150);
  Serial.println("trying to init");
  sensor.init(true);
  Serial.println("done");

  delay(100);
  sensor.setAddress((uint8_t)23);


  Serial.println("addresses set");



}

void loop()
{
  int distread = sensor.readRangeSingleMillimeters();
  Serial.print(distread);
  if (sensor.timeoutOccurred()) { Serial.print(" TIMEOUT"); }

  Serial.println();
}
