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
 * MoveDriver.hpp
 *
 *  Created on: Nov 6, 2014
 *      Author: Andriy Panasenko <apanasenko@cybervisiontech.com>
 */

#ifndef MOVEDRIVER_HPP_
#define MOVEDRIVER_HPP_

#include "WheelControler.hpp"
#include "lib/PID.hpp"
#include "EEPROMex.h"
#include "Sonar.hpp"
#include "Accelgyro.hpp"


#define ORIENTATION_PID_MIN_LIMIT -250
#define ORIENTATION_PID_MAX_LIMIT 250
#define ORIENTATION_PID_SAMPLE_TIME 25

#define SLOW_MOTION_THRESHOLD 1.0 //Velocity degree per second
#define SLOW_MOTION_PID_THRESHOLD 10.0 //degree
#define SLOW_MOTION_DETECT_DURATION 2000 //msec
#define ACCEPT_TURN_THRESHOLD 0.5 //degree

#define TURN_LEFT_ANGEL -90
#define TURN_RIGHT_ANGEL 90

//PID Constants
#define TURN_FAST_KP 23
#define TURN_FAST_KI 7
#define TURN_FAST_KD 2

#define TURN_SLOW_KP 27
#define TURN_SLOW_KI 120
#define TURN_SLOW_KD 2

#define FORWARD_FAST_KP 27
#define FORWARD_FAST_KI 7.0
#define FORWARD_FAST_KD 2

#define FORWARD_BASE_POWER 100
#define FORWARD_DISTANCE_TO_SWITCH_LOW_POWER 50
#define FORWARD_LOW_POWER 80

#define WHEEL_CALIBRATE_TIME 3000
#define WHEEL_CALIBRATE_COEFF 0.7
#define WHEEL_MAX_RATIO  1.2
#define WHEEL_MIN_RATIO 0.8

#define NUMBER_PIDS_CONSTANTS 3

class PIDControlable {
	public:
		virtual ~PIDControlable() {};
		virtual void updatePidCoefficients(float Kp, float Ki, float Kd) = 0;
};

class PIDConst {

	private:
		float constKp;
		float constKi;
		float constKd;
	public:
		PIDConst(float Kp, float Ki, float Kd) {
			constKp = Kp;
			constKi = Ki;
			constKd = Kd;
		};

		void setCoefficients(PIDControlable * pid) const {
			if (pid == NULL)
				return;
			pid->updatePidCoefficients(constKp,constKi,constKd);
		};
};

class PIDConstants {
	public:
		enum CONST_TYPE {
			PID_SLOW = 0,
			PID_FAST = 1,
			PID_FORWARD = 2
		};

	private:
		CONST_TYPE current;
		PIDConst * constants[NUMBER_PIDS_CONSTANTS];
		void addConst(CONST_TYPE type, PIDConst * pidConst) {
			if (pidConst == NULL)
				return;
			if (type >= NUMBER_PIDS_CONSTANTS)
				return;
			constants[type] = pidConst;
		};
	public:

		PIDConstants() : current(PID_SLOW)
		{
			addConst(PID_SLOW, new PIDConst(TURN_SLOW_KP, TURN_SLOW_KI, TURN_FAST_KD));
			addConst(PID_FAST, new PIDConst(TURN_FAST_KP,TURN_FAST_KI, TURN_FAST_KD));
			addConst(PID_FORWARD, new PIDConst(FORWARD_FAST_KP, FORWARD_FAST_KI, FORWARD_FAST_KD));
		};

		void applyTunings(CONST_TYPE type, PIDControlable * pid) {
			if (type == current)
				return;
			switch (type) {
				case PID_SLOW:
					constants[PID_SLOW]->setCoefficients(pid);
					current = PID_SLOW;
					break;
				case PID_FAST:
					constants[PID_FAST]->setCoefficients(pid);
					current = PID_FAST;
					break;
				case PID_FORWARD:
					constants[PID_FORWARD]->setCoefficients(pid);
					current = PID_FORWARD;
					break;
			}
		};
};

class MoveDriver;

class OrientationDriver : PIDDrivable, PIDControlable {
	private:
		MoveDriver * driver;
		PID * pid;
		bool operating;
		float input;
		float output;
		float setpoint;
		float yawInitial;

	public:
		OrientationDriver(MoveDriver * d);
		void keepOrientation(float angel);
		void keepOrientation();
		void stopOrientation();
		bool Compute();
		float getYawOffset();
		float angelTranslateToPositive(float a);

		void updatePidCoefficients(float Kp, float Ki, float Kd);

		virtual float getInput();
		virtual float getOutput();
		virtual float getSetpoint();
		virtual void setOutput(float output);
		virtual float getOutputMin();
		virtual float getOutputMax();
		virtual bool isOperating();

};

typedef void Drive_command_complete_t();

class MoveDriver {
	public:
		enum STATE { UNDEFINED, FORWARD, STOPPING, TURN_LEFT, TURN_RIGHT, CALIBRATE };

	private:
		int eepromBaseAddress;
		PIDConstants pidConstants;

		Drive_command_complete_t * callback;

		STATE state;

		int wheelBasePower;
		int rightWheelPower;
		int leftWheelPower;

		float fDistance;
		float cDist;

		long slowMotionFirstDetected;

		long calibrateStarted;
		bool leftTurnCalibrate;
		bool storeCalibrateRatio;

		OrientationDriver * orientator;
		Accelgyro * mpu;
		WheelControler leftWheel;
		WheelControler rightWheel;
		Sonar sonar;

	public:
		MoveDriver(int baseAddress, int servoPin, int sonarEchoPin, int sonarTriggerPin);

		WheelControler * getLeft() {
			return &leftWheel;
		};

		WheelControler * getRight() {
			return &rightWheel;
		};

		int getWheelPower() {
			return wheelBasePower;
		};

		void init();

		void setupLeft(int speedPin, int directionPin1, int directionPin2, int detectorPin) {
			leftWheel.setup(speedPin, directionPin1, directionPin2, detectorPin);
		};

		void setupRight(int speedPin, int directionPin1, int directionPin2, int detectorPin) {
			rightWheel.setup(speedPin, directionPin1, directionPin2, detectorPin);
		};

		Accelgyro * getMPU() {
			return mpu;
		};

		Sonar * getSonar() {
			return &sonar;
		};

		void Compute();

		void Forward(Drive_command_complete_t * commadCompleteCallback, float distance);

		void Stop(bool keep);

		void TurnLeft(Drive_command_complete_t * commadCompleteCallback);

		void TurnRight(Drive_command_complete_t * commadCompleteCallback);

		void print() {

		};

		void printCalibratedValues();

		void WheelCalibrate(bool leftTurn, bool store);

		void writeWheelPowerFactor();

	private:
		void compute();

		void CalibrateCompute();

		void TurnCompute();

		void ForwardCompute();

};


#endif /* MOVEDRIVER_HPP_ */
