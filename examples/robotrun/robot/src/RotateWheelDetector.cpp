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
 * RotateWheelDetector.cpp
 *
 *  Created on: Nov 27, 2014
 *      Author: Andriy Panasenko <apanasenko@cybervisiontech.com>
 */

#include "RotateWheelDetector.hpp"

#define DEC_INDEX(i,p,LIMIT) \
	i = p -1; \
	if (i < 0){\
		i = LIMIT - 1; \
	}

#define INC_INDEX(p,LIMIT) \
	p++;\
	if (p >= LIMIT) {\
		p = 0; \
	}

void RotateWheelDetector::init() {
	ticks = 0;
	vel_p = 0;
	velocityFilter.init();
	cli();
	hist_p = 0;
	for(int i=0;i<TICKS_HISTORY_LENGTH;i++) {
		history[i] = 0;
	}
	sei();
	for(int i=0;i<VELOCITY_HISTORY_LENGTH;i++) {
		velosity[i] = 0;
	}
};

RotateWheelDetector::RotateWheelDetector() :
	currentState(HIGH),
	detectorPin(0),
	ticks(0),
	hist_p(0),
	vel_p(0),
	velocityFilter(VELOCITY_GAIN, VELOCITY_COFF_0, VELOCITY_COFF_1)
{
	init();
};

void RotateWheelDetector::setPin(int pin) {
	detectorPin = pin;
	pinMode(detectorPin, INPUT);
	//Serial.print("RotateWheelDetector set pin "); Serial.println(pin);
};



void RotateWheelDetector::detect() {
	long ctimeStamp = micros();

	int cState = digitalRead(detectorPin);
	if (cState != currentState) {
		currentState = cState;
		ticks++;
		history[hist_p] = ctimeStamp;

		int p1;
		DEC_INDEX(p1,hist_p,TICKS_HISTORY_LENGTH);
		lastUpdateCounter = ctimeStamp;
		long dti = history[hist_p] - history[p1];
		float dt = (float)dti * (float)TIMER_CLOCK;
		float v = DIST_PER_TICK/dt;


		velosity[vel_p] = velocityFilter.getFillteredValue(v);

		INC_INDEX(hist_p,TICKS_HISTORY_LENGTH);

		INC_INDEX(vel_p,VELOCITY_HISTORY_LENGTH);

	} else if ((ctimeStamp - lastUpdateCounter) >= VELOCITY_ZERO_TIMEOUT) {
		lastUpdateCounter = ctimeStamp;
		velosity[vel_p] = velocityFilter.getFillteredValue(0.0);
		INC_INDEX(vel_p,VELOCITY_HISTORY_LENGTH);
	}

};

int RotateWheelDetector::getState() { return currentState; };

void RotateWheelDetector::reset() {
	init();
};

int RotateWheelDetector::getTicks() {
	return ticks;
};

float RotateWheelDetector::getDistance() {
	return (float)getTicks() * (float)DIST_PER_TICK;
};

float RotateWheelDetector::getVelocity() {
	int vel;
	DEC_INDEX(vel,vel_p,VELOCITY_HISTORY_LENGTH);
	return velosity[vel];
};

void RotateWheelDetector::printHistory() {
	Serial.print("hist pointer=") ;Serial.println(hist_p);

	for(int i=0;i<TICKS_HISTORY_LENGTH;i++) {
		Serial.println(history[i]);
	}
	Serial.print("vel pointer=") ;Serial.println(vel_p);
	for(int i=0;i<VELOCITY_HISTORY_LENGTH;i++) {
		Serial.println(velosity[i]);
	}
};
