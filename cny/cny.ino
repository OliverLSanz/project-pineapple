#define CNY A0

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(5, OUTPUT);
  pinMode(CNY, INPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
//  Serial.println(analogRead(CNY));
//  delay(200);
   if(digitalRead(CNY)){
    digitalWrite(5, HIGH);
   }else{
    digitalWrite(5, LOW);
   }
}
