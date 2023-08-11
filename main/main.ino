
const int motor2pin = 9;  // not on pins 5,6
const int motor4pin = 10;

const int samplefreq = 250;
const int pwmfreq = 600;

// tach pins are interrupts
const int tachpin2 = 2;
const int tachpin4 = 3;


const int timeout = 1;  // in s before omega is 0
const float pi = PI;

const int startuppwm = 31936;
int refpwm = startuppwm+6; ///////////////////////////////////////////////////////////////////////////////////////////////

int pwm2, pwmphase;
float error, cumerror, errorp, cumerrorp, derror, preverror;

float kp = .0015;
float ki = .00025;
float kd = .00000;

float kpp = 10;
float kip = .002;


int reading, tpwm;
unsigned long t, tprior, dt;


// tachometer variables
volatile unsigned long dt2, d02, dt24, d024, time_shift;
volatile float rpm, rpm4;

volatile int reading2, reading4;

volatile float phase, phase4;


void setup() {

  // put your setup code here, to run once:
  //pinMode(tachpin, INPUT);


  attachInterrupt(digitalPinToInterrupt(tachpin2), tach, RISING);
  attachInterrupt(digitalPinToInterrupt(tachpin4), tach4, RISING);

  drive(motor2pin,startuppwm, pwmfreq);
  delay(3000);
  drive(motor4pin,startuppwm, pwmfreq);
  delay(3000);

  Serial.begin(115200);
}

void loop() {
  // put your main code here, to run repeatedly:
  while (Serial.available() > 0) {
    // look for the next valid integer in the incoming serial stream:
    int inputpwm = Serial.parseInt();

    // look for the newline. That's the end of your sentence:
    if (Serial.read() == '\n') {
      refpwm = inputpwm;
    }
  }


  t = micros();


  if ((t - tprior) >= 1e6 / samplefreq) {
    //analogWrite(motor2pin, refpwm);  // drive the reference motor
    drive(motor2pin, refpwm, pwmfreq);

    if ((micros() - d02) > timeout * 1e6) {
      rpm = 0;
    }
    if ((micros() - d024) > timeout * 1e6) {
      rpm4 = 0;
    }

    //error = phase - phase4;
    error = rpm - rpm4;
    cumerror = cumerror + error;
    derror = error - preverror;

    pwm2 = refpwm + kp * error + ki * cumerror + kd * derror;


    errorp = phase - phase4;
    cumerrorp = cumerrorp + errorp;

    pwmphase = kp * errorp + cumerrorp * ki;

    pwm2 = constrain(pwm2, startuppwm, startuppwm + 60);
    drive(motor4pin, pwm2, pwmfreq);


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

void drive(int pin, int duty, int freq) {
  /*
  int dtpwm = 1e6/freq; // in microseconds
  int highdutydt = pwm*dtpwm/200;
  int lowdutydt = dtpwm - highdutydt;
  Serial.println(t-tpwm);

  if ((t-tpwm) >= highdutydt){
    digitalWrite(pin,HIGH);
    tpwm = t;
  }
  else{
    digitalWrite(pin,LOW);
  }
  

  TIM_TypeDef *Instance4 = (TIM_TypeDef *)pinmap_peripheral(digitalPinToPinName(pin), PinMap_PWM);
  uint32_t channel4 = STM_PIN_CHANNEL(pinmap_function(digitalPinToPinName(pin), PinMap_PWM));
  HardwareTimer *MyTim4 = new HardwareTimer(Instance4);
  MyTim4->setPWM(channel4, pin, freq, duty); 
  */
  analogWrite(pin,duty);
  analogWriteFrequency(freq);
}

