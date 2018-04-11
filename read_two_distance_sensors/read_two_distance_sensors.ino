#include <Wire.h>
#include <VL53L0X.h>

VL53L0X sensor;
VL53L0X sensor2;

void setup()
{

  pinMode(5, OUTPUT);
  pinMode(4, OUTPUT);
  digitalWrite(4, LOW);
  digitalWrite(5, LOW);

  delay(500);
  Wire.begin();

  Serial.begin (9600);

  pinMode(4, INPUT);
  delay(150);
  Serial.println("trying to init");
  sensor.init(true);
  Serial.println("init1");

  delay(100);
  sensor.setAddress((uint8_t)22);

  pinMode(5, INPUT);
    delay(150);
  sensor2.init(true);
  delay(100);
  sensor2.setAddress((uint8_t)25);

  Serial.println("addresses set");



}

void loop()
{
  int read1 = sensor.readRangeSingleMillimeters();
  int read2 = sensor2.readRangeSingleMillimeters();

 // Serial.print("Readings: ");
 // Serial.print(read1);
 // Serial.print(" -- ");
  Serial.print(read2);
  if (sensor.timeoutOccurred() || sensor2.timeoutOccurred()) { Serial.print(" TIMEOUT"); }

  Serial.println();
}
