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
 * WheelControler.hpp
 *
 *  Created on: Nov 4, 2014
 *      Author: Andriy Panasenko <apanasenko@cybervisiontech.com>
 */

#ifndef WHEELCONTROLER_HPP_
#define WHEELCONTROLER_HPP_

#include <Arduino.h>
#include "RotateWheelDetector.hpp"

#define DEFAULT_FORWARD_POWER_FACTOR 1.0
#define DEFAULT_BACKWARD_POWER_FACTOR 1.0
#define POWER_FACTOR_RANGE 0.4

class WheelControler {
	private:

		int speed_pin;
		int direction_pin1;
		int direction_pin2;
		int wheelRotDetectorPin;


		bool run;

		int powerFactorForwardAddress;
		int powerFactorBackwardAddress;
		float powerFactorForward;
		float powerFactorBackward;

		RotateWheelDetector * detector;

	public:
		WheelControler();

		void init();

		void setForwardPowerFactor(float factor);

		float getForwardPowerFactor() const;

		void setBackwardPowerFactor(float factor);

		float getBackwardPowerFactor() const;

		void setup(int speedPin, int directionPin1, int directionPin2, int detectorPin);

		void Compute();

		void setForwardDirection();

		void setBackwardDirection();

		void drive(int value);

		void stop();

		void writePowerFactors();

		RotateWheelDetector * getWheelDetector() const;

		void printCalibratedValues();

	private:
		bool checkPowerFactorRange(float powerFactor);
};



#endif /* WHEELCONTROLER_HPP_ */
