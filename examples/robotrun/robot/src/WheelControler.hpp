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

/**
 * WheelControler Class.
 * Used to control wheel.
 *   Hold in EEPROM two power factors for forward and backward rotating.
 *       To use EEPROM correctly, EEPROM.setMemPool(eepromBaseAddress, EEPROMSizeUno);
 *       should be called before init() of this class.
 *
 *   Power factors get from calibration process, and used to smooth out
 *   wheel rotation for both left and right wheels of robot.
 */
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
		/**
		 * Default constructor.
		 */
		WheelControler();

		/**
		 * Read power factors from EEPROM
		 */
		void init();

		/**
		 * Forward power factor setter.
		 * @param float power factor
		 */
		void setForwardPowerFactor(float factor);

		/**
		 * Forward poer factor getter.
		 * @return float power factor
		 */
		float getForwardPowerFactor() const;

		/**
		 * Backward power factor setter.
		 * @param float power factor
		 */
		void setBackwardPowerFactor(float factor);

		/**
		 * Backward poer factor getter.
		 * @return float power factor
		 */
		float getBackwardPowerFactor() const;

		/**
		 * Initialize pins of wheel control and rotation velocity detector.
		 * @param int speedPin - wheel PWM regulator pin
		 * @param int directionPin1 - L298N direction pin1
		 * @param int directionPin2 - L298N direction pin2
		 * @param int detectorPin - IR comparator digital pin
		 */
		void setup(int speedPin, int directionPin1, int directionPin2, int detectorPin);

		/**
		 * Method used to call rotation detector method detect(), should be called as often as possible.
		 */
		void Compute();

		/**
		 * Set L298N direction to forward
		 */
		void setForwardDirection();

		/**
		 * Set L298N direction to backward
		 */
		void setBackwardDirection();

		/**
		 * Drive wheel, setting PWM to value multiplied to appropriate power factor.
		 * @param int value PWM drive value
		 */
		void drive(int value);

		/**
		 * Stop driving.
		 */
		void stop();

		/**
		 * Write power factors to EEPROM
		 */
		void writePowerFactors();

		/**
		 * Rotate wheel detector getter.
		 * @return pointer to RotateWheelDetector
		 */
		RotateWheelDetector * getWheelDetector() const;

		/**
		 * Print into Serial power factor calibrated values.
		 */
		void printCalibratedValues();

	private:
		/**
		 * Check power factor range, if power factor out of range return false, otherwise true.
		 * @param float power factor to chaeck.
		 * @return bool if power factor inside defined range.
		 */
		bool checkPowerFactorRange(float powerFactor);
};



#endif /* WHEELCONTROLER_HPP_ */
