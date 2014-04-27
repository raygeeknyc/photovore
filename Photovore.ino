/**
 * Take Your Children To Work Day 2014 @ Google.NYC
 * (c) 2014 by Raymond Blum <raygeeknyc@gmail.com>
 */
int sensorlPin = 1;
int sensorrPin = 3;
int speakerPin = 4;
int servoLPin = 1;
int servoRPin = 0;

// Define these based on your servos and controller, the values to cause your servos 
// to spin in opposite directions at approx the same speed.
#define CW 30
#define CCW 10

#define SERVO_L_FWD CW
#define SERVO_R_FWD CCW

#define SERVO_L_BWD CCW
#define SERVO_R_BWD CW

#define SERVO_L_STOP 0
#define SERVO_R_STOP 0

#define SENSOR_DELTA_THRESHOLD_PCT 25
#define SENSOR_DELTA_THRESHOLD 40

#define HIGHEST_THRESHOLD 4

#define SENSOR_SAMPLES 5

#define MAX_SENSOR_READING 1023  // Used to seed sensor pair normalization

// How long to spin while callibrating the sensor pair
#define DUR_CALLIBRATION 1000

// How long to pause when sensors are equal
#define DUR_FWD 2500

// How long to spin when sensors are equal
#define DUR_SPIN 500

// How long to pause between steps while spinning to normalize the sensor pair
#define SPIN_STEP_DELAY_MS 15
#define DIR_STOP 0
#define DIR_RIGHT 1
#define DIR_LEFT 2
#define DIR_FWD 3

int sr, sl;
int s_max, s_highest;
int s_delta, s_change_pct;
int current_dir, last_dir;
int sensor_nornalization_delta;
unsigned long stopped_until;

void setup() {
  beep(speakerPin, 500, 333);
  pinMode(speakerPin, OUTPUT);
  pinMode(servoLPin, OUTPUT);
  pinMode(servoRPin, OUTPUT);
  analogWrite(servoLPin, SERVO_L_STOP);
  analogWrite(servoRPin, SERVO_R_STOP);
  sensor_nornalization_delta = 0;
  callibrateSensors();
}

int smooth(int array[], int len) {
  /**
  Take the average of the array without the highest and lowest
  values
  **/
  int low = MAX_SENSOR_READING;
  int high = -1;
  int total = 0;
  for (int s=0; s< len; s++) {
    total += array[s];
    low = min(array[s], low);
    high = max(array[s], high);
  }
  total -= low;
  total -= high;
  return total / (len -2);
}

void callibrateSensors() {
  /* Find the closest concurrent values of our two sensors while spinning in
     a random direction and use that as a sensor normalizing value.
   **/
  int min_delta = MAX_SENSOR_READING;
  float dir = random(2);
  unsigned long callibration_until = millis() + DUR_CALLIBRATION;
  int spin_dir = (dir > 1.5)?DIR_LEFT:DIR_RIGHT;
  while (millis() < callibration_until) {
    spin(spin_dir);
    delay(SPIN_STEP_DELAY_MS);
    spin(DIR_STOP);
    delay(SPIN_STEP_DELAY_MS);
    readSensors();
    if (abs(s_delta) < abs(min_delta)) {
      min_delta = s_delta;
    }
  }
  drive(DIR_STOP);
  sensor_nornalization_delta = min_delta;
}

void readSensors() {
  /***
  The multiple reads and delay are recommended to allow the shared ADC to properly
  read multiple pins in succession.
  ***/
  int s_array[SENSOR_SAMPLES];
  
  analogRead(sensorlPin);delay(10);  
  for (int s=0; s<SENSOR_SAMPLES; s++) {
    s_array[s] = analogRead(sensorlPin);
  }
  sl = smooth(s_array, SENSOR_SAMPLES);
  analogRead(sensorrPin);delay(10);
  sr=MAX_SENSOR_READING;
  for (int s=0; s<SENSOR_SAMPLES; s++) {
    s_array[s] = analogRead(sensorrPin);
  }
  sr = smooth(s_array, SENSOR_SAMPLES);
  s_max = max(sl,sr);
  s_delta = (sl - sensor_nornalization_delta) - sr;
  s_change_pct = (float)abs(s_delta) / s_max * 100;
}

void loop() {  
  readSensors();

  if ((abs(s_delta) > SENSOR_DELTA_THRESHOLD) && (s_change_pct > SENSOR_DELTA_THRESHOLD_PCT)) {
    drive((sl < sr)? DIR_RIGHT:DIR_LEFT);
    stopped_until = 0;
  } else {
    if (s_max < (s_highest - HIGHEST_THRESHOLD)) {
      drive(DIR_FWD);
    } else {
      drive(DIR_STOP);
    }
    s_highest = max(s_max, s_highest);
  }
}

void drive(int direction) {
  current_dir = direction;
  switch (direction) {
    case DIR_LEFT:
      analogWrite(servoLPin, SERVO_L_STOP);
      analogWrite(servoRPin, SERVO_R_FWD);
      break;
    case DIR_RIGHT:
      analogWrite(servoLPin, SERVO_L_FWD);
      analogWrite(servoRPin, SERVO_R_STOP);
      break;
    case DIR_FWD:
      analogWrite(servoLPin, SERVO_L_FWD);
      analogWrite(servoRPin, SERVO_R_FWD);
      break;
    case DIR_STOP:
      analogWrite(servoLPin, SERVO_L_STOP);
      analogWrite(servoRPin, SERVO_R_STOP);
      break;
  }
}

void spin(int direction) {
  current_dir = direction;
  switch (direction) {
    case DIR_LEFT:
      analogWrite(servoLPin, SERVO_L_BWD);
      analogWrite(servoRPin, SERVO_R_FWD);
      break;
    case DIR_RIGHT:
      analogWrite(servoLPin, SERVO_L_FWD);
      analogWrite(servoRPin, SERVO_R_BWD);
      break;
    case DIR_STOP:
      analogWrite(servoLPin, SERVO_L_STOP);
      analogWrite(servoRPin, SERVO_R_STOP);
      break;
  }
}

void burp() {
  beep(speakerPin, 125, 50);
  beep(speakerPin, 250, 75);
}

// the sound producing function
void beep (unsigned char speakerPin, int frequencyInHertz, long timeInMilliseconds)
{	 // http://web.media.mit.edu/~leah/LilyPad/07_sound_code.html
          int x;	 
          long delayAmount = (long)(1000000/frequencyInHertz);
          long loopTime = (long)((timeInMilliseconds*1000)/(delayAmount*2));
          for (x=0;x<loopTime;x++)	 
          {	 
              digitalWrite(speakerPin,HIGH);
              delayMicroseconds(delayAmount);
              digitalWrite(speakerPin,LOW);
              delayMicroseconds(delayAmount);
          }	 
}
