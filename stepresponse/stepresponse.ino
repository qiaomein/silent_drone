// constants
const int motor2pin = 9;
const int motor1pin = 10;
const int samplefreq = 1000;
const int pwmfreq = 600;
const int tachpin2 = 2; //interruptpin
const int tachpin4 = 3; // interruptpin
const int tachpin1 = 4; // interruptpin
const int timeout = 1;  // seconds before rpm = 0
const int startuppwm = 31933;
const float alpha = 0.3;


// declarations
int refpwm = startuppwm+1;


int pwm2, pwmphasereading, pwmphase, tpwm, xornow, xorprior;
float theta;
unsigned long t, tprior, dt, dtxor, d0xor; // for tracking loop time

// volatile variables
volatile unsigned long dt2, d02, dt1, d01, dt4, d04, time_shift;
volatile float rpm1, rpm2, rpm4;

volatile int reading2, reading4, reading1;

void setup() {

  attachInterrupt(digitalPinToInterrupt(tachpin1), tach1, RISING);

  startMotors();

  Serial.begin(2000000);
  
}

void loop() {

  // serial input sets the reference pwm to power the reference motor
  while (Serial.available() > 0)
  {
    int inputpwm = Serial.parseInt();
    if (Serial.read() == '\n') {
      refpwm = refpwm + inputpwm;
    }
  }


  t = micros();
  reading1 = digitalRead(tachpin1);


  if ((t - tprior) >= 1e6 / samplefreq) { // set constant sampling time for the controller
    drive(motor1pin, refpwm, pwmfreq); // drive the reference motor

    if ((micros() - d01) > timeout * 1e6) {
      rpm1 = 0;
    }
    if (t >= 10e6){
      refpwm = startuppwm + 15;
    }
    if (t >= 15e6){
      refpwm = startuppwm + 30;
    }
    if (t >= 24e6){
      refpwm = startuppwm - 30;
    }

    dt = t - tprior;
    tprior = t;
    theta = theta + dt*1e-6*rpm1*6; // theta fmod 100 revolutions
    Serial.print(t);
    Serial.print(",");
    Serial.print(reading1);
    Serial.print(',');
    Serial.print(theta);
    Serial.print(",");
    Serial.print(rpm1);
    Serial.print(",");
    Serial.print(refpwm);
    Serial.print(",");
    Serial.println(1.0e6 / dt); // sr frequency
  }

  
}


void tach1() {
  dt1 = micros() - d01;
  d01 = micros();
  
  rpm1 = alpha* rpm1 + (1-alpha)*1.0 * 60 / (dt1 * 1e-6);
}

void drive(int pin, int duty, int freq) { // duty is really pwm and is an artifact; pwm ranges from 0-65000 (16 bit) on the nucleo
  analogWrite(pin,duty);
  analogWriteFrequency(freq);
}

void startMotors(){
  drive(motor1pin,startuppwm, pwmfreq);
  delay(1500);
  drive(motor1pin,startuppwm, pwmfreq);
  delay(4000);
}

