
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

  // serial input sets the reference pwm to power the reference motor
  while (Serial.available() > 0)
  {
    int inputpwm = Serial.parseInt();
    if (Serial.read() == '\n') {
      refpwm = refpwm + inputpwm;
    }
  }


  t = micros();
  reading2 = digitalRead(tachpin2);
  reading4 = digitalRead(tachpin4);
  reading1 = digitalRead(tachpin1);


  if ((t - tprior) >= 1e6 / samplefreq) { // set constant sampling time for the controller
    drive(motor2pin, refpwm, pwmfreq); // drive the reference motor

    if ((micros() - d02) > timeout * 1e6) {
      rpm2 = 0;
    }
    if ((micros() - d04) > timeout * 1e6) {
      rpm4 = 0;
    }
    if ((micros() - d01) > timeout * 1e6) {
      rpm1 = 0;
    }
    if ((micros() - d0xor) > timeout * 1e6) {
      xor_rate = 0;
    }

    error = rpm2 - rpm1;
    cumerror = constrain(cumerror + error, -5e5, 5e5);
    derror = error - preverror;

    pwm2 = refpwm + kp * error + ki * cumerror + kd * derror;


    errorp = phase - phase4;
    cumerrorp = cumerrorp + errorp;
    pwmphase = kp * errorp + cumerrorp * ki;

    pwm2 = constrain(pwm2, startuppwm-60, startuppwm + 60);
    drive(motor4pin, pwm2, pwmfreq); // drive the following motor
    //drive(motor4pin, refpwm, pwmfreq); // drive the following motor


    

    phase = phase + (dt * 1e-6 * rpm2) / 60.0 * 360.0;
    phase = fmod(phase, 360.0);
    phase4 = phase4 + (dt * 1e-6 * rpm4) / 60.0 * 360.0;
    phase4 = fmod(phase4, 360.0);


    xornow = XOR(reading2, reading1);
    if (xorprior == 0 && xornow == 1) // if xor is rising
    {
      dtxor = micros()-d0xor;
      d0xor = micros();
      xor_rate = 60.0/(dtxor*1e-6);
    }


    // phases are in degrees

    dt = t - tprior;
    tprior = t;
    preverror = error;
    xorprior = xornow;
  }

  Serial.print(t);
  Serial.print(",");

  Serial.print(reading1);
  Serial.print(',');
  Serial.print(reading2);
  Serial.print(',');
  Serial.print(pwm2);
  Serial.print(',');
  Serial.print(xornow);
  Serial.print(",");
  Serial.print(xor_rate);
  Serial.print(",");
  Serial.print(rpm1);
  Serial.print(",");
  Serial.print(rpm2);
  Serial.print(",");
  Serial.print(rpm4);
  Serial.print(",");
  Serial.print(1.0e6 / dt);
  Serial.print(",");
  Serial.println(error);
}

void tach2() {
  //Serial.println("sensed!");

  dt2 = micros() - d02;
  d02 = micros();
  rpm2 = 1.0 * 60 / (dt2 * 1e-6);
  phase = 0;
}

void tach4() {
  dt4 = micros() - d04;
  d04 = micros();
  rpm4 = 1.0 * 60 / (dt4 * 1e-6);
  phase4 = 0;
}

void tach1() {
  dt1 = micros() - d01;
  d01 = micros();
  rpm1 = 1.0 * 60 / (dt1 * 1e-6);
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

int XOR(int x, int y){
  if (x == 1 && y == 1){
    return 0;
  }
  else if (x == 1 || y == 1){
    return 1;
  }
  return 0;
}



