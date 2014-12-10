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
 * PID.cpp
 *
 *  Created on: Dec 9, 2014
 *      Author: Andriy Panasenko <apanasenko@cybervisiontech.com>
 */

#include <Arduino.h>
#include "PID.hpp"

PID::PID(PIDDrivable * d, long sampleTime) {
	drivable = d;
	kp = 0;
	ki = 0;
	kd = 0;
	pidSampleTime = sampleTime;
	long now = millis();
	lastCalculatedTime = now - sampleTime;
	ITerm = drivable->getOutput();
	lastInput = drivable->getInput();
};

bool PID::Compute() {
	if(!drivable->isOperating()) return false;
	long now = millis();
	long t = (now - lastCalculatedTime);
	if(t>=pidSampleTime) {
		double input = (double)drivable->getInput();
		double error = (double)drivable->getSetpoint() - input;
		ITerm += ((double)ki * error);
		if(ITerm > drivable->getOutputMax()) {
			ITerm= drivable->getOutputMax();
		} else if(ITerm < drivable->getOutputMin()) {
			ITerm= drivable->getOutputMin();
		}
		double dInput = input - lastInput;

		double output = (double)kp * error + ITerm - (double)kd * dInput;

		if(output > drivable->getOutputMax())  {
			output = drivable->getOutputMax();
		} else if(output < drivable->getOutputMin()) {
			output = drivable->getOutputMin();
		}
		drivable->setOutput(output);

		lastInput = input;
		lastCalculatedTime = now;
		return true;
	} else {
		return false;

	}
};

void PID::setCoefficients(float Kp, float Ki, float Kd) {
	if (Kp<0 || Ki<0 || Kd<0) {
		return;
	}

	double pidSampleTimeInSec = ((double)pidSampleTime)/1000.0;

	kp = Kp;
	ki = Ki * pidSampleTimeInSec;
	kd = Kd / pidSampleTimeInSec;
};

void PID::reset() {
	ITerm = drivable->getOutput();
	lastInput = drivable->getInput();
	if(ITerm > drivable->getOutputMax()) {
		ITerm= drivable->getOutputMax();
	} else if(ITerm < drivable->getOutputMin()) {
		ITerm= drivable->getOutputMin();
	}
};
