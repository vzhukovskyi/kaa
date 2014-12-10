/*
 * Copyright 2014 CyberVision, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
 * Sonar.hpp
 *
 *  Created on: Nov 21, 2014
 *      Author: Andriy Panasenko <apanasenko@cybervisiontech.com>
 */

#ifndef SONAR_HPP_
#define SONAR_HPP_

#include <Arduino.h>
#include "Servo.h"
#include "EEPROMex.h"

#define SERVO_MIN 544
#define SERVO_MAX 2400
// speed = 340 m/s 0.034 cm/micros
#define SONAR_SOUND_VELOCITY  0.017//*1.0546
#define SONAR_TIMEOUT      15000
#define SONAR_MAX_DISTANCE 254.0

#define SONAR_MAX_PROBE 5
#define SONAR_AVERAGE_THRESHOLD 10.0 //sm

#define SONAR_SERVO_LEFT_ANGEL 180
#define SONAR_SERVO_FRONT_ANGEL 85
#define SONAR_SERVO_RIGHT_ANGEL 0
#define SONAR_SERVO_COMPLETE_TIMEOUT 1000

#define SONAR_CALIBRATE_DISTANCE 50.0 //cm
#define SONAR_CALIBRATE_PRECISION 1.0  //cm
#define SONAR_CALIBRATE_HIT_ENTER_TIMEOUT 10000 //milliseconds

typedef void Sonar_distance_measure_t(float distance);

class Sonar {
	public:
		enum SONAR_DIRECTION {
			LEFT,
			FRONT,
			RIGHT
		};
	private:


		float D[SONAR_MAX_PROBE];
		int servoMinAddress;
		int servoMaxAddress;
		int sonarSoundVAddress;
		int servoMin;
		int servoMax;
		Servo servo;
		long servoStart;
		bool servoComplete;
		int ServoPin;
		int SonarEchoPin;
		int SonarTrigerPin;
		float soundVelocity;
		Sonar_distance_measure_t * callback;
	public:
		Sonar(int servoPin, int echoPin, int trigerPin);

		void clearMeasureArray();

		void init();

		void measure(Sonar_distance_measure_t * measureCallback, SONAR_DIRECTION direction);

		void Compute();

		void print();

		void calibrateServo();

		void calibrateSoundVelocity();

		void writeSoundVelocityToEEPROM();

		void writeServoToEEPROM();

		void printCalibratedValues();

	private:
		float getDistance();

		long getDistDuration();

		void setServoMin(int min);

		void setServoMax(int max);

		void setSoundVelocity(float vel);
};


#endif /* SONAR_HPP_ */
