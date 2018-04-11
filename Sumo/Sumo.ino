#include <Sumo.h>
PinLayout ly;


Sumo miSumo;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  miSumo.dist_sensor_enables(4,5,6);
  ly.dist_sensor_enables[0] = 4;
}

void loop() {
  // put your main code here, to run repeatedly:
  miSumo.salute();
  delay(1000);
}
