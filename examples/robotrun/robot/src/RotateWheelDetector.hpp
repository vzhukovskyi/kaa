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
 * TurnDetector.hpp
 *
 *  Created on: Nov 3, 2014
 *      Author: Andriy Panasenko <apanasenko@cybervisiontech.com>
 */

#ifndef ROTATEWHEELDETECTOR_HPP_
#define ROTATEWHEELDETECTOR_HPP_

#include <Arduino.h>
#include "lib/IIRFilter.hpp"

#define TICKS_HISTORY_LENGTH 3
#define VELOCITY_HISTORY_LENGTH 3
#define VELOCITY_ZERO_TIMEOUT 100000 //micro seconds


#define DIST_PER_TICK 9.333333
//#define TIMER_CLOCK 0.000256
#define TIMER_CLOCK 0.000001

//IIR Low pass filter coefficients for velocity, cutoff 0.2
#define VELOCITY_GAIN   4.088704738
#define VELOCITY_COFF_0 -0.3296559103
#define VELOCITY_COFF_1 0.3513509810

/**
 * RotateWheelDetector Class.
 *
 * 	Used to calculate wheel speed rotation and measure running distance.
 * 	For rotate intervals use IR comparator on detectorPin pin.
 * 	Require periodic call method detect() as much as possible.
 * 	Time in microseconds of signal change from IR comparator stored in circular buffer hist_p.
 * 	Using two hist_p calculates velocity, velocity values processed with IIRFilter
 * 	and stores into  circular buffer vel_p.
 */
class RotateWheelDetector {
	private:
		int currentState;
		int detectorPin;
		int ticks;
		unsigned long lastUpdateCounter;
		unsigned long history[TICKS_HISTORY_LENGTH];
		float velosity[VELOCITY_HISTORY_LENGTH];
		int hist_p;
		int vel_p;
		IIRFilter velocityFilter;

	/**
	 * init() method cleanup all variables and buffers.
	 */
	void init();

	public:
		/**
		 * Default constructor.
		 */
		RotateWheelDetector();

		/**
		 * Set pin assigned to IR comparator.
		 */
		void setPin(int pin);

		/**
		 * return distance which travel wheel on one tick of comparator, in mm.
		 * @return float distance in mm
		 */
		float const static getMinDetectDistance() {
			return DIST_PER_TICK;
		};

		/**
		 * Method which detect signal change and calculate velocity and traveled distance
		 * should be called as much as possible.
		 */
		void detect();

		/**
		 * cleanup all variables and buffers.
		 */
		void reset();

		/**
		 * Return number of times signal on comparator was changed.
		 * @return int
		 */
		int getTicks();

		/**
		 * Return traveled distance in mm
		 * @return float
		 */
		float getDistance();

		/**
		 * Return current velocity
		 * @return float
		 */
		float getVelocity();


		/**
		 * print into Serial current debug hist_p and vel_p arrays.
		 */
		void printHistory();
};



#endif /* ROTATEWHEELDETECTOR_HPP_ */
