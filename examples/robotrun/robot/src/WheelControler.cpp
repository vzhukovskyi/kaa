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
 * WheelControler.cpp
 *
 *  Created on: Nov 27, 2014
 *      Author: Andriy Panasenko <apanasenko@cybervisiontech.com>
 */

#include "WheelControler.hpp"
#include "EEPROMex.h"

WheelControler::WheelControler() :
	speed_pin(0),
	direction_pin1(0),
	direction_pin2(0),
	wheelRotDetectorPin(0),
	run(false),
	powerFactorForwardAddress(0),
	powerFactorBackwardAddress(0),
	powerFactorForward(DEFAULT_FORWARD_POWER_FACTOR),
	powerFactorBackward(DEFAULT_BACKWARD_POWER_FACTOR)
{
	detector = new RotateWheelDetector();
};

void WheelControler::init() {
	powerFactorForwardAddress = EEPROM.getAddress(sizeof(powerFactorForward));
	powerFactorBackwardAddress = EEPROM.getAddress(sizeof(powerFactorBackward));

	setForwardPowerFactor(EEPROM.readFloat(powerFactorForwardAddress));
	setBackwardPowerFactor(EEPROM.readFloat(powerFactorBackwardAddress));
};

void WheelControler::writePowerFactors() {
	EEPROM.writeFloat(powerFactorForwardAddress,getForwardPowerFactor());
	EEPROM.writeFloat(powerFactorBackwardAddress,getBackwardPowerFactor());
};

void WheelControler::setForwardPowerFactor(float factor) {
	if (checkPowerFactorRange(factor)) {
		powerFactorForward = factor;
	} else {
		powerFactorForward = DEFAULT_FORWARD_POWER_FACTOR;
	}
};

float WheelControler::getForwardPowerFactor() const {
	return powerFactorForward;
};

void WheelControler::setBackwardPowerFactor(float factor) {
	if (checkPowerFactorRange(factor)) {
		powerFactorBackward = factor;
	} else {
		powerFactorBackward = DEFAULT_BACKWARD_POWER_FACTOR;
	}
};

float WheelControler::getBackwardPowerFactor() const {
	return powerFactorBackward;
};

void WheelControler::setup(int speedPin, int directionPin1, int directionPin2, int detectorPin) {
	speed_pin = speedPin;
	direction_pin1 = directionPin1;
	direction_pin2 = directionPin2;
	wheelRotDetectorPin = detectorPin;
	pinMode(speed_pin, OUTPUT);
	pinMode(direction_pin1, OUTPUT);
	pinMode(direction_pin2, OUTPUT);
	detector->setPin(wheelRotDetectorPin);
};

void WheelControler::drive(int value) {
	int v = 0;
	float powerFactor = 0.0;
	if (value > 0) {
		setForwardDirection();
		v = value;
		powerFactor = powerFactorForward;
	} else if (value < 0){
		setBackwardDirection();
		v = -value;
		powerFactor = powerFactorBackward;
	}
	if (v > 255) {
		v = 255;
	}
	analogWrite(speed_pin,v*powerFactor);
};

void WheelControler::stop() {
	digitalWrite(direction_pin1, LOW);
	digitalWrite(direction_pin2, LOW);
	analogWrite(speed_pin,0);
	run = false;
};

void WheelControler::Compute() {
	detector->detect();
};


RotateWheelDetector * WheelControler::getWheelDetector() const {
	return detector;
}

void WheelControler::printCalibratedValues() {
	Serial.print("F:"); Serial.print(powerFactorForward,4);Serial.print(",B:");Serial.println(powerFactorBackward,4);
};

void WheelControler::setForwardDirection() {
	digitalWrite(direction_pin1, LOW);
	digitalWrite(direction_pin2, HIGH);
};

void WheelControler::setBackwardDirection() {
	digitalWrite(direction_pin1, HIGH);
	digitalWrite(direction_pin2, LOW);
};

bool WheelControler::checkPowerFactorRange(float powerFactor) {
	if (isnan(powerFactor)) {
		return false;
	}
	float delta = (DEFAULT_BACKWARD_POWER_FACTOR + DEFAULT_FORWARD_POWER_FACTOR)* 0.5;
	delta -= powerFactor;
	delta = abs(delta);
	if (delta > POWER_FACTOR_RANGE) {
		return false;
	} else {
		return true;
	}
}
