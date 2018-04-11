#define pinLED PC13

void setup() {
  Serial.begin(9600);

  delay(1000);
  Serial.println("Hello wwwww");  
  pinMode(pinLED, OUTPUT);
  Serial.println("START");  
}

void loop() {
  digitalWrite(pinLED, HIGH);
  delay(1000);
  digitalWrite(pinLED, LOW);
  Serial.println("Hello asdasdasdas");  
}
