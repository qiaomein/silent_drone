
// constants
const int motor2pin = 9;
const int motor4pin = 10;
const int samplefreq = 250;
const int pwmfreq = 600;
const int tachpin2 = 2; //interruptpin
const int tachpin4 = 3; // interruptpin
const int timeout = 1;  // seconds before rpm = 0
const float pi = PI;
const int startuppwm = 31936;

// declarations
int refpwm = startuppwm+6;

int pwm2, pwmphasereading, tpwm;
float error, cumerror, errorp, cumerrorp, derror, preverror;
unsigned long t, tprior, dt; // for tracking loop time

// volatile variables
volatile unsigned long dt2, d02, dt24, d024, time_shift;
volatile float rpm, rpm4;

volatile int reading2, reading4;
volatile float phase, phase4;


// CONTROLLER GAINS BELOW
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Speed control
float kp = .0015;
float ki = .00025;
float kd = .00000;

// Phase control
float kpp = 10;
float kip = .002;
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void setup() {

  attachInterrupt(digitalPinToInterrupt(tachpin2), tach, RISING);
  attachInterrupt(digitalPinToInterrupt(tachpin4), tach4, RISING);

  startMotors();

  Serial.begin(115200);
  
}

void loop() {

  // serial input sets the reference pwm to power the reference motor
  while (Serial.available() > 0)
  {
    int inputpwm = Serial.parseInt();
    if (Serial.read() == '\n') {
      refpwm = inputpwm;
    }
  }


  t = micros();


  if ((t - tprior) >= 1e6 / samplefreq) { // set constant sampling time for the controller
    drive(motor2pin, refpwm, pwmfreq); // drive the reference motor

    if ((micros() - d02) > timeout * 1e6) {
      rpm = 0;
    }
    if ((micros() - d024) > timeout * 1e6) {
      rpm4 = 0;
    }

    error = rpm - rpm4;
    cumerror = cumerror + error;
    derror = error - preverror;

    pwm2 = refpwm + kp * error + ki * cumerror + kd * derror;


    errorp = phase - phase4;
    cumerrorp = cumerrorp + errorp;
    pwmphase = kp * errorp + cumerrorp * ki;

    pwm2 = constrain(pwm2, startuppwm, startuppwm + 60);
    drive(motor4pin, pwm2, pwmfreq); // drive the following motor


    dt = t - tprior;

    phase = phase + (dt * 1e-6 * rpm) / 60.0 * 360.0;
    phase = fmod(phase, 360.0);
    phase4 = phase4 + (dt * 1e-6 * rpm4) / 60.0 * 360.0;
    phase4 = fmod(phase4, 360.0);


    Serial.print(t);
    Serial.print(",");
    Serial.print(digitalRead(tachpin2));
    Serial.print(',');
    Serial.print(digitalRead(tachpin4));
    Serial.print(',');
    Serial.print(pwm2);
    Serial.print(",");
    Serial.print(1.0e6 / dt);
    Serial.print(",");
    Serial.print(error);
    Serial.print(',');    
    Serial.println(errorp);
    // phases are in degrees


    tprior = t;
    preverror = error;
  }
}

void tach() {
  //Serial.println("sensed!");

  dt2 = micros() - d02;
  d02 = micros();
  rpm = 1.0 * 60 / (dt2 * 1e-6);
  //reading2 = 1;
  phase = 0;
}

void tach4() {
  dt24 = micros() - d024;
  d024 = micros();
  rpm4 = 1.0 * 60 / (dt24 * 1e-6);
  phase4 = 0;
}

void drive(int pin, int duty, int freq) { // duty is really pwm and is an artifact; pwm ranges from 0-65000 (16 bit) on the nucleo
  analogWrite(pin,duty);
  analogWriteFrequency(freq);
}

void startMotors(){
  drive(motor2pin,startuppwm, pwmfreq);
  drive(motor4pin,startuppwm, pwmfreq);
  delay(1000);
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
