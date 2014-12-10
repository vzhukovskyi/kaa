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

class PIDDrivable {
	public:
		virtual ~PIDDrivable() {};
		virtual float getInput() = 0;
		virtual float getOutput() = 0;
		virtual float getSetpoint() = 0;
		virtual void setOutput(float output) = 0;
		virtual float getOutputMin() = 0;
		virtual float getOutputMax() = 0;
		virtual bool isOperating() = 0;
};

class PID {
	private:
		PIDDrivable * drivable;
		float kp,ki,kd;
		long pidSampleTime;
		long lastCalculatedTime;
		double ITerm;
		double lastInput;
	public:
		PID(PIDDrivable * d, long sampleTime);
		bool Compute();
		void setCoefficients(float Kp, float Ki, float Kd);
		void reset();
};


#endif /* LIB_PID_HPP_ */
