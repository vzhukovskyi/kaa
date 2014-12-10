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
 * Sonar.cpp
 *
 *  Created on: Dec 1, 2014
 *      Author: Andriy Panasenko <apanasenko@cybervisiontech.com>
 */

#include "Sonar.hpp"

Sonar::Sonar(int servoPin, int echoPin, int trigerPin) :
	servoMinAddress(0),
	servoMaxAddress(0),
	sonarSoundVAddress(0),
	servoMin(SERVO_MIN),
	servoMax(SERVO_MAX),
	servoStart(0),
	servoComplete(false),
	ServoPin(servoPin),
	SonarEchoPin(echoPin),
	SonarTrigerPin(trigerPin),
	soundVelocity(SONAR_SOUND_VELOCITY),
	callback(NULL)
{
	clearMeasureArray();
};

void Sonar::clearMeasureArray() {
	for (int i = 0; i < SONAR_MAX_PROBE; ++i) {
		D[i]=0;
	}
};

void Sonar::init() {
	servoMinAddress = EEPROM.getAddress(sizeof(servoMin));
	servoMaxAddress = EEPROM.getAddress(sizeof(servoMax));
	sonarSoundVAddress = EEPROM.getAddress(sizeof(soundVelocity));

	setServoMin(EEPROM.readInt(servoMinAddress));
	setServoMax(EEPROM.readInt(servoMaxAddress));
	setSoundVelocity(EEPROM.readFloat(sonarSoundVAddress));

	servo.attach(ServoPin,servoMin,servoMax);
	pinMode(SonarEchoPin, INPUT);
	pinMode(SonarTrigerPin, OUTPUT);
	digitalWrite(SonarTrigerPin, LOW);
	servo.write(SONAR_SERVO_FRONT_ANGEL);
};

void Sonar::measure(Sonar_distance_measure_t * measureCallback, SONAR_DIRECTION direction) {
	switch (direction) {
		case LEFT:
			servo.write(SONAR_SERVO_LEFT_ANGEL);
			break;
		case FRONT:
			servo.write(SONAR_SERVO_FRONT_ANGEL);
			break;
		case RIGHT:
			servo.write(SONAR_SERVO_RIGHT_ANGEL);
			break;
	}
	callback = measureCallback;
	servoStart = millis();
	servoComplete = false;
};

void Sonar::Compute() {
	if (!servoComplete) {
		if ((millis() - servoStart) >= SONAR_SERVO_COMPLETE_TIMEOUT) {
			servoComplete = true;
			servoStart = 0;

			clearMeasureArray();
			float min=254;
			float max=0;
			for (int i = 0; i < SONAR_MAX_PROBE; ++i) {
				D[i] = getDistance();
				if (D[i] > max) {
					max = D[i];
				}
				if (D[i] < min) {
					min = D[i];
				}
			}
			if (callback) {
				callback(max);
			}
		}
	}
};

void Sonar::print() {
	Serial.print("Servo min="); Serial.print(servoMin); Serial.print(" max="); Serial.println(servoMax);
	Serial.print("Sound Velocity: "); Serial.println(soundVelocity);
	Serial.println("Probes:");
	for (int i = 0; i < SONAR_MAX_PROBE; ++i) {
		Serial.println(D[i]);
	}
};

void Sonar::calibrateServo() {
	servo.detach();
	Serial.println("Set min value:");
	Serial.setTimeout(SONAR_CALIBRATE_HIT_ENTER_TIMEOUT);
	String minS = Serial.readStringUntil('\r');
	setServoMin(minS.toInt());

	Serial.println("Set max value: ");
//			Serial.println("Enter max value 4 digits, end <enter>");
	String maxS = Serial.readStringUntil('\r');
	setServoMax(maxS.toInt());

	Serial.print(servoMin); Serial.print(","); Serial.println(servoMax);
	servo.attach(ServoPin,servoMin,servoMax);
};

void Sonar::calibrateSoundVelocity() {
	Serial.print("Target on "); Serial.print(SONAR_CALIBRATE_DISTANCE); Serial.println("cm, hit enter:");
	Serial.setTimeout(10000);
	String minS = Serial.readStringUntil('\r');
	float d = abs(SONAR_CALIBRATE_DISTANCE - getDistance());
	if (d <= SONAR_CALIBRATE_PRECISION) {
		Serial.print("Value inside "); Serial.print(SONAR_CALIBRATE_PRECISION); Serial.println("cm range");
		return;
	}
	soundVelocity =  SONAR_CALIBRATE_DISTANCE / (float)getDistDuration();
	Serial.print("New "); Serial.println(soundVelocity);
};

void Sonar::writeSoundVelocityToEEPROM() {
	EEPROM.writeFloat(sonarSoundVAddress,soundVelocity);
	while(!EEPROM.isReady()) { delay(1); }
};

void Sonar::writeServoToEEPROM() {
	EEPROM.writeInt(servoMinAddress,servoMin);
	while(!EEPROM.isReady()) { delay(1); }
	EEPROM.writeInt(servoMaxAddress,servoMax);
	while(!EEPROM.isReady()) { delay(1); }
};

void Sonar::printCalibratedValues() {
	Serial.println("Sonar:");
	Serial.print("SV:");Serial.println(soundVelocity,4);
	Serial.print("Servo: min=");Serial.print(servoMin);Serial.print(", max=");Serial.println(servoMax);
};

float Sonar::getDistance() {
	long duration = getDistDuration();
	return (float)duration * soundVelocity;
};

long Sonar::getDistDuration() {
	digitalWrite(SonarTrigerPin, LOW);
	delayMicroseconds(2);
	digitalWrite(SonarTrigerPin, HIGH);
	delayMicroseconds(10);
	digitalWrite(SonarTrigerPin, LOW);
	return pulseIn(SonarEchoPin,HIGH,SONAR_TIMEOUT);
};

void Sonar::setServoMin(int min) {
	if ((min < 0) ||
		(min > 1000)) {
		servoMin = SERVO_MIN;
	} else {
		servoMin = min;
	}
};

void Sonar::setServoMax(int max) {
	if ((max < 0) ||
			(max > 3000)) {
		servoMax = SERVO_MAX;
	} else {
		servoMax = max;
	}
};

void Sonar::setSoundVelocity(float vel) {
	if (isnan(vel)) {
		soundVelocity = SONAR_SOUND_VELOCITY;
		return;
	}
	float absVelRange = abs(vel - SONAR_SOUND_VELOCITY);
	if ((absVelRange > 0.02) || (absVelRange < 0.01) ) {
		soundVelocity = SONAR_SOUND_VELOCITY;
	} else {
		soundVelocity = vel;
	}
};
