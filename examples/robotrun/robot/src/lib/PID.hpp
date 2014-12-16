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
 * PID.hpp
 *
 *  Created on: Dec 9, 2014
 *      Author: Andriy Panasenko <apanasenko@cybervisiontech.com>
 */

#ifndef LIB_PID_HPP_
#define LIB_PID_HPP_

/**
 * Interface which should be implemented by drivable object.
 */
class PIDDrivable {
	public:
		virtual ~PIDDrivable() {};
		/**
		 * Return input float value for PID.
		 * @return float Input value
		 */
		virtual float getInput() = 0;

		/**
		 * Return current output value for PID
		 * @return float output value
		 */
		virtual float getOutput() = 0;

		/**
		 * Return current setpoint for PID
		 * @return float setpoint value
		 */
		virtual float getSetpoint() = 0;

		/**
		 * Set new calculated by PID output value
		 * @param float new output value
		 */
		virtual void setOutput(float output) = 0;

		/**
		 * Return Output minimum value
		 * @return float minimum output
		 */
		virtual float getOutputMin() = 0;

		/**
		 * Return Output maximum value
		 * @return float maximum output
		 */
		virtual float getOutputMax() = 0;

		/**
		 * Return is PID regulation is necessary, if true, new output will be calculated
		 * @return bool is PID should operate
		 */
		virtual bool isOperating() = 0;
};

/**
 * PID regulator class.
 *  calculates new output using coefficients and setpoint
 */
class PID {
	private:
		PIDDrivable * drivable;
		float kp,ki,kd;
		long pidSampleTime;
		long lastCalculatedTime;
		double ITerm;
		double lastInput;
	public:
		/**
		 * Default constructor.
		 * @param - pointer to PIDDrivable interface
		 * @param - long sample time, in milliseconds, how often new output should be calculated.
		 */
		PID(PIDDrivable * d, long sampleTime);

		/**
		 * All calculation making here.
		 * Method should be invoked frequent than sample time is set.
		 */
		bool Compute();

		/**
		 * Apply new regulation coefficients
		 * @param - float Kp
		 * @param - float Ki
		 * @param - float Kd
		 */
		void setCoefficients(float Kp, float Ki, float Kd);

		/**
		 * Reset PID internal values, to start working from scratch.
		 */
		void reset();
};


#endif /* LIB_PID_HPP_ */
