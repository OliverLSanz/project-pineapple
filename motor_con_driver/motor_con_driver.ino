#define ENA 10
#define ENB 11
#define N1 4 
#define N2 3
#define N3 5
#define N4 2

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(N1, OUTPUT);
  pinMode(N2, OUTPUT);
  pinMode(N3, OUTPUT);
  pinMode(N4, OUTPUT);
// inicio 128
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);

  digitalWrite(N1, 1);
  digitalWrite(N2, 0);
  digitalWrite(N3, 1);
  digitalWrite(N4, 1);
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(N1, 1);
  digitalWrite(N2, 0);
  for(int i = 0; i < 255; i+=5){
    analogWrite(ENA, i);
    delay(100);
  }
  delay(500);
  digitalWrite(N1, 0);
  digitalWrite(N2, 1);
  for(int i = 0; i < 255; i+=5){
    analogWrite(ENA, i);
    delay(100);
  }
  delay(500);
}
