
// constants
const int motor2pin = 9;
const int motor4pin = 10;
const int samplefreq = 700;
const int pwmfreq = 600;
const int tachpin2 = 2; //interruptpin
const int tachpin4 = 3; // interruptpin
const int tachpin1 = 4; // interruptpin
const int timeout = 1;  // seconds before rpm = 0
const float pi = PI;
const int startuppwm = 31933;

// declarations
int refpwm = startuppwm+4;


int pwm2, pwmphasereading, pwmphase, tpwm, xornow, xorprior;
float error, cumerror, errorp, cumerrorp, derror, preverror, xor_rate;
float theta;
unsigned long t, tprior, dt, dtxor, d0xor; // for tracking loop time

// volatile variables
volatile unsigned long dt2, d02, dt1, d01, dt4, d04, time_shift;
volatile float rpm1, rpm2, rpm4;

volatile int reading2, reading4, reading1;
volatile float phase, phase4;


// CONTROLLER GAINS BELOW
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Speed control
float kp = .00015;
float ki = .00005;
float kd = .00000;

// Phase control
float kpp = 10;
float kip = .002;
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void setup() {

  attachInterrupt(digitalPinToInterrupt(tachpin2), tach2, RISING);
  attachInterrupt(digitalPinToInterrupt(tachpin4), tach4, RISING);
  attachInterrupt(digitalPinToInterrupt(tachpin1), tach1, RISING);

  startMotors();

  Serial.begin(2000000);
  
}

void loop() {

  // // serial input sets the reference pwm to power the reference motor
  // while (Serial.available() > 0)
  // {
  //   int inputpwm = Serial.parseInt();
  //   if (Serial.read() == '\n') {
  //     refpwm = refpwm + inputpwm;
  //   }
  // }


  t = micros();
  reading4 = digitalRead(tachpin4);



  if ((t - tprior) >= 1e6 / samplefreq) { // set constant sampling time for the controller
    drive(motor2pin, refpwm, pwmfreq); // drive the reference motor

    if ((micros() - d04) > timeout * 1e6) {
      rpm4 = 0;
    }

  }

  sine_sweep();

}

void tach4() {
  dt4 = micros() - d04;
  d04 = micros();
  rpm4 = 1.0 * 60 / (dt4 * 1e-6);
  phase4 = 0;
}

void drive(int pin, int duty, int freq) { // duty is really pwm and is an artifact; pwm ranges from 0-65000 (16 bit) on the nucleo
  analogWrite(pin,duty);
  analogWriteFrequency(freq);
}

void startMotors(){
  drive(motor2pin,startuppwm, pwmfreq);
  drive(motor4pin,startuppwm, pwmfreq);
  delay(1500);
  drive(motor2pin,startuppwm, pwmfreq);
  drive(motor4pin,startuppwm, pwmfreq);
  delay(4000);
}

void sine_sweep() {
  for (int pwm = refpwm; pwm <= refpwm + 25; pwm + 6) {
    analogWrite(motor4pin, pwm);
    theta = fmod(theta + dt*1e-6*rpm4*6, 36000); // theta fmod 100 revolutions

    Serial.print(t);
    Serial.print(',');
    Serial.print(reading4);
    Serial.print(",");
    Serial.print(rpm4);
    Serial.print(",");
    Serial.print(theta);
    Serial.print(",");
    Serial.print(1.0e6 / dt);

    delay(5000);
  }
  
}
