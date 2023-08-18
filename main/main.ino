
// constants
const int motor2pin = 9;
const int motor4pin = 10;
const int samplefreq = 500;
const int pwmfreq = 600;
const int tachpin2 = 2;  //interruptpin
const int tachpin4 = 3;  // interruptpin
const int tachpin1 = 4;  // interruptpin
const int timeout = 1;   // seconds before rpm = 0
const int startuppwm = 31933;
const float alpha = .3;
const int maxrotations = 5000;
const int maxcum = 5e8;

// declarations
int pwm2, pwmphasereading, pwmphase, tpwm;
float error, cumerror, preverror, derror, theta1, theta2;
unsigned long t, tprior, dt, dtxor, d0xor;  // for tracking loop time
bool reset1, reset2;

int refpwm = startuppwm + 21;
bool control = true;

// volatile variables
volatile unsigned long dt2, d02, dt1, d01, dt4, d04, time_shift;
volatile float rpm1, rpm2, rpm4;

int reading2, reading4, reading1;


// CONTROLLER GAINS
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Speed control
float kp = 1;
float ki = .0;
float kd = .5;

float k = .00045;  // overall gain

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void setup() {

  attachInterrupt(digitalPinToInterrupt(tachpin2), tach2, RISING);
  attachInterrupt(digitalPinToInterrupt(tachpin4), tach4, RISING);
  attachInterrupt(digitalPinToInterrupt(tachpin1), tach1, RISING);

  startMotors();

  Serial.begin(2000000);
}

void loop() {

  // serial input sets the reference pwm to power the reference motor
  while (Serial.available() > 0) {
    int inputpwm = Serial.parseInt();
    if (Serial.read() == '\n') {
      refpwm = refpwm + inputpwm;
    }
  }


  t = micros();
  reading2 = digitalRead(tachpin2);
  reading4 = digitalRead(tachpin4);
  reading1 = digitalRead(tachpin1);


  if ((t - tprior) >= 1e6 / samplefreq) {  // set constant sampling time for the controller
    drive(motor2pin, refpwm, pwmfreq);     // drive the reference motor

    if ((micros() - d02) > timeout * 1e6) {
      rpm2 = 0;
    }
    if ((micros() - d04) > timeout * 1e6) {
      rpm4 = 0;
    }
    if ((micros() - d01) > timeout * 1e6) {
      rpm1 = 0;
    }
    
    if (reset1 != reset2){
      control = false;
    }
    else{
      control = true;
    }


    if (control){
      error = theta2 - theta1;
      //error = rpm2 - rpm1;
      cumerror = constrain(cumerror + error,-maxcum,maxcum);
      derror = error - preverror;
      pwm2 = refpwm + k*(kp * error + ki*dt*1e-6*cumerror + kd * derror * 1e6 / dt);
    }
    


    pwm2 = constrain(pwm2, startuppwm - 10, startuppwm + 50);
    drive(motor4pin, pwm2, pwmfreq);  // drive the following motor
    //drive(motor4pin, refpwm, pwmfreq); // drive the following motor


    dt = t - tprior;
    tprior = t;
    preverror = error;
    theta2 = theta2 + rpm2*6*dt*1e-6;
    theta1 = theta1 + rpm1*6*dt*1e-6;

    


    if (theta2 >= maxrotations*360){
      reset1 = !reset1;
      theta2 = 0;
    }


    if (theta1 > maxrotations*360){
      reset2 = !reset2;
      theta1 = 0;
    }

    Serial.print(t);
    Serial.print(",");
    Serial.print(reading1);
    Serial.print(',');
    Serial.print(reading2);
    Serial.print(',');
    Serial.print(pwm2);
    Serial.print(',');
    Serial.print(rpm1);
    Serial.print(",");
    Serial.print(rpm2);
    Serial.print(",");
    Serial.print(1.0e6 / dt);
    Serial.print(",");
    Serial.println(error);
  }
}

void tach2() {
  dt2 = micros() - d02;
  d02 = micros();
  rpm2 = alpha * rpm2 + (1 - alpha) * 1.0 * 60 / (dt2 * 1e-6);
}

void tach4() {
  dt4 = micros() - d04;
  d04 = micros();
  rpm4 = alpha * rpm4 + (1 - alpha) * 1.0 * 60 / (dt4 * 1e-6);
}

void tach1() {
  dt1 = micros() - d01;
  d01 = micros();
  rpm1 = alpha * rpm1 + (1 - alpha) * 1.0 * 60 / (dt1 * 1e-6);
}

void drive(int pin, int duty, int freq) {  // duty is really pwm and is an artifact; pwm ranges from 0-65000 (16 bit) on the nucleo
  analogWrite(pin, duty);
  analogWriteFrequency(freq);
}

void startMotors() {
  drive(motor2pin, startuppwm, pwmfreq);
  drive(motor4pin, startuppwm, pwmfreq);
  delay(1500);
  drive(motor2pin, startuppwm, pwmfreq);
  drive(motor4pin, startuppwm, pwmfreq);
  delay(4000);
}


