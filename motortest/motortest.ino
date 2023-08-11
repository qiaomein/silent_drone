
//DShot esc1(DShot::Mode::DSHOT300);

const int motor1pin = 5;  // not on pins 5,6


void setup() {
  // put your setup code here, to run once:
  pinMode(motor1pin,OUTPUT);
  analogWrite(motor1pin,0);
  delay(5000);

  Serial.begin(9600);
  Serial.println("Starting...");



}

void loop() {
  // put your main code here, to run repeatedly:
  // M4 at pin 5 (higher pwm freq) below
  for (int i = 30; i < 60; i++){
    analogWrite(motor1pin,i);
    Serial.println(i);
    delay(2000);
  }
  

}
