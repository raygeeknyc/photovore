/***
 * Copyright (c) 2020 by Raymond Blum <raymond@insanegiantrobots.com>
 *
 * This software is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */
#define sensorLPin A0  // AKA Digital pin 1
#define sensorRPin A2  // AKA Digital pin 0
#define speakerPin 3

// Don't put these on 2 PWM pins that share the same timer
#define L_SERVO_PIN 2
#define R_SERVO_PIN 4

#include <Servo.h>
Servo LEFT_SERVO, RIGHT_SERVO;

#include <Adafruit_DotStar.h>
Adafruit_DotStar strip = Adafruit_DotStar(1, INTERNAL_DS_DATA, INTERNAL_DS_CLK, DOTSTAR_BGR);

// Define these based on your servos and controller, the value to cause your servos 
// to come to a full stop.
#define SERVO_STOP 92

#define SERVO_DELTA 10

// If the CW and CCW speeds are not close to the same, you will have to change these to values that
// cause the servos to rotate in opposite directions at the same speed
#define CW SERVO_STOP-SERVO_DELTA
#define CCW SERVO_STOP+SERVO_DELTA 

#define SERVO_L_FWD CW
#define SERVO_R_FWD CCW

#define SERVO_L_BWD CCW
#define SERVO_R_BWD CW

#define SERVO_L_STOP SERVO_STOP
#define SERVO_R_STOP SERVO_STOP

#define SENSOR_DELTA_THRESHOLD_PCT 30.0  // Must be a float

#define SENSOR_SAMPLES 5

#define MAX_SENSOR_READING 1023  // Used to seed sensor pair normalization

// How long to spin while callibrating the sensor pair
#define DUR_CALLIBRATION_MSECS 2000

// How long to pause between steps while spinning to normalize the sensor pair
#define SPIN_STEP_DELAY_MSECS 15

#define DIR_STOP 0
#define DIR_RIGHT 1
#define DIR_LEFT 2
#define DIR_FWD 3

int sensor_right, sensor_left;
int s_max, s_highest;
int s_min, s_lowest;
int s_delta;
int current_dir, last_dir;
int sensor_normalization_delta;
int sensor_delta_threshold;

void testRGBLED() {
  strip.setPixelColor(0, 0, 0, 0);  //off
  strip.show();
  delay(1000);
  strip.show();
  delay(500);
  strip.setPixelColor(0, 127, 127, 0);  //yellow
  strip.show();
  delay(500);
  strip.setPixelColor(0, 127, 0, 127);  //magenta
  strip.show();
  delay(500);
  strip.setPixelColor(0, 0, 0, 0);  //off
  strip.show();
  delay(1000);
}

void testServos() {
  RIGHT_SERVO.write(SERVO_R_STOP+SERVO_DELTA);
  LEFT_SERVO.write(SERVO_L_STOP+SERVO_DELTA);
  delay(1000);

  RIGHT_SERVO.write(SERVO_R_STOP-SERVO_DELTA);
  LEFT_SERVO.write(SERVO_L_STOP-SERVO_DELTA);
  delay(1000);

  RIGHT_SERVO.write(SERVO_R_STOP);
  LEFT_SERVO.write(SERVO_L_STOP);
  delay(2000);
}

void setup() {
  strip.begin();
  testRGBLED();

  pinMode(speakerPin, OUTPUT);
  burp();
  
  RIGHT_SERVO.attach(R_SERVO_PIN);
  LEFT_SERVO.attach(L_SERVO_PIN);
  testServos();
  
  sensor_normalization_delta = 0;
  callibrateSensors();
}

void recordDirection(int dir) {
  last_dir = current_dir;
  current_dir = dir;
}

int smooth(int array[], int len) {
  /**
  Return the average of the array without the highest and lowest values.
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
  /* Spin and find the smallest delta between the two sensors at min or max, and use that as
   * a built-in difference between the two.
   **/
  int left_min = MAX_SENSOR_READING;
  int right_min = MAX_SENSOR_READING;
  int left_max = 0;
  int right_max = 0;
  
  float dir = random(3);
  unsigned long callibrate_until = millis() + DUR_CALLIBRATION_MSECS;
  int spin_dir = (dir > 1.0)?DIR_LEFT:DIR_RIGHT;
  while (millis() < callibrate_until) {
    spin(spin_dir);
    delay(SPIN_STEP_DELAY_MSECS);
    drive(DIR_STOP);
    delay(SPIN_STEP_DELAY_MSECS);
    readSensors();
    left_min = min(left_min, sensor_left);
    left_max = max(left_max, sensor_left);
    right_min = min(left_min, sensor_right);
    right_max = max(left_max, sensor_right);
  }
  sensor_normalization_delta = min(abs(right_min - left_min), abs(right_max - left_max));
  sensor_delta_threshold = s_max * (SENSOR_DELTA_THRESHOLD_PCT / 100) - sensor_normalization_delta;
}

void readSensors() {
  /***
  The multiple reads and delay are recommended to allow the shared ADC to properly
  read multiple pins in succession.
  ***/
  int samples[SENSOR_SAMPLES];
  
  analogRead(sensorLPin);delay(10);  
  for (int s=0; s<SENSOR_SAMPLES; s++) {
    samples[s] = analogRead(sensorLPin);
  }
  sensor_left = smooth(samples, SENSOR_SAMPLES);

  analogRead(sensorRPin);delay(10);
  for (int s=0; s<SENSOR_SAMPLES; s++) {
    samples[s] = analogRead(sensorRPin);
  }
  sensor_right = smooth(samples, SENSOR_SAMPLES);

  s_max = max(sensor_left, sensor_right);
  s_min = min(sensor_left, sensor_right);
  s_delta = abs((s_max - s_min) - sensor_normalization_delta);
}

void showHappyLED() {
  strip.setPixelColor(0, 127, 127, 127);  // light gray / pale white
  strip.show();
}

void showSadLED() {
  strip.setPixelColor(0, 127, 0, 0);  // pale red
  strip.show();
}

void showWaitingLED() {
  strip.setPixelColor(0, 0, 0, 0);  // off
  strip.show();
}

void showSeekingLED() {
  strip.setPixelColor(0, 20, 20, 127);  // light blue
  strip.show();
}

void loop() {  
  readSensors();

  if (s_delta > sensor_delta_threshold) {
    drive((sensor_left < sensor_right)? DIR_RIGHT:DIR_LEFT);
    showSeekingLED();
  } else {
    if (s_max < (s_highest - sensor_delta_threshold)) {
      drive(DIR_FWD);
      showSadLED();
    } else {
      drive(DIR_STOP);
      if (last_dir == DIR_FWD) {
        burp();
        showHappyLED();
      } else {
        showWaitingLED();
      }
    }
  }
  s_highest = max(s_max, s_highest);
  s_lowest = min(s_min, s_lowest);
}

void drive(int direction) {
  recordDirection(direction);
  switch (direction) {
    case DIR_LEFT:
      LEFT_SERVO.write(SERVO_L_FWD);
      RIGHT_SERVO.write(SERVO_R_STOP);
      break;
    case DIR_RIGHT:
      LEFT_SERVO.write(SERVO_L_STOP);
      RIGHT_SERVO.write(SERVO_R_FWD);
      break;
    case DIR_FWD:
      LEFT_SERVO.write(SERVO_L_FWD);
      RIGHT_SERVO.write(SERVO_R_FWD);
      break;
    case DIR_STOP:
      LEFT_SERVO.write(SERVO_L_STOP);
      RIGHT_SERVO.write(SERVO_R_STOP);
      break;
  }
}

// Emit a fairly rude noise
void burp() {
  tone(speakerPin, 250, 100);
  delay(100);
  tone(speakerPin, 125, 75);
  delay(75);
  tone(speakerPin, 350, 100);
}

void spin(int direction) {
  recordDirection(direction);
  switch (direction) {
    case DIR_LEFT:
      LEFT_SERVO.write(SERVO_L_BWD);
      RIGHT_SERVO.write(SERVO_R_FWD);
      break;
    case DIR_RIGHT:
      LEFT_SERVO.write(SERVO_L_FWD);
      RIGHT_SERVO.write(SERVO_R_BWD);
      break;
    case DIR_STOP:
      LEFT_SERVO.write(SERVO_L_STOP);
      RIGHT_SERVO.write(SERVO_R_STOP);
      break;
  }
}
