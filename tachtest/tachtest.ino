


const int motor1pin = 10;  // not on pins 5,6
const int tachpin = 3;



const int timeout = 1.5; // in s before omega is 0
const long pi = PI;

int reading, pwm;
unsigned long t0;

volatile unsigned long dt, d0;
volatile float rpm;

bool flag = false;

void setup() {
  // put your setup code here, to run once:
  //pinMode(tachpin, INPUT);
  attachInterrupt(digitalPinToInterrupt(tachpin),tach,RISING);
  pinMode(motor1pin,OUTPUT);
  analogWrite(motor1pin,0);

  delay(1000);
  //analogWrite(motor1pin,150);
  delay(3000);
  Serial.begin(115200);
  pwm = 30;
}

void loop() {
  // put your main code here, to run repeatedly:
  t0 = millis()/1000;

  if (t0 % 5 == 0 && flag){
    pwm++;
    Serial.println("CHANGE");
    flag = false;
  }
  if (t0 % 5 != 0){
    flag = true;
  }

  

  analogWrite(motor1pin, pwm);


  reading = digitalRead(tachpin);

  
  if ((micros()-d0) > timeout*1e6){
    rpm = 0;
  }

  //Serial.print(d0); Serial.print(','); Serial.print(reading); Serial.print(','); Serial.print(dt); Serial.print(','); 
  Serial.print(t0);
  Serial.print(','); 
  Serial.print(dt);
  Serial.print(','); 
  Serial.print(pwm);
  Serial.print(','); 
  Serial.println(rpm);


}

void tach(){
  //Serial.println("sensed!");
  
  dt = micros()-d0;
  d0 = micros();
  rpm = 1.0*60/(dt*1e-6);
  

}
