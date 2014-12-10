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
 * MoveDriver.cpp
 *
 *  Created on: Nov 27, 2014
 *      Author: Andriy Panasenko <apanasenko@cybervisiontech.com>
 */

#include "MoveDriver.hpp"
#include "Accelgyro.hpp"

MoveDriver::MoveDriver(int baseAddress, int servoPin, int sonarEchoPin, int sonarTriggerPin) :
			eepromBaseAddress(baseAddress),
			callback(NULL),
			state(UNDEFINED),
			wheelBasePower(0),
			rightWheelPower(0),
			leftWheelPower(0),
			fDistance(0),
			cDist(0),
			slowMotionFirstDetected(0),
			calibrateStarted(0),
			leftTurnCalibrate(false),
			storeCalibrateRatio(false),
			sonar(servoPin, sonarEchoPin, sonarTriggerPin)
		{
			orientator = new OrientationDriver(this);
			mpu = new Accelgyro();
		};


void MoveDriver::init() {
	EEPROM.setMemPool(eepromBaseAddress, EEPROMSizeUno);
	EEPROM.setMaxAllowedWrites(100);
	delay(100);
	mpu->init();
	sonar.init();
	leftWheel.init();
	rightWheel.init();
};

void MoveDriver::Compute() {
	mpu->Compute();
	leftWheel.Compute();
	rightWheel.Compute();
	compute();
	sonar.Compute();
};

void MoveDriver::Forward(Drive_command_complete_t * commadCompleteCallback, float distance) {
	mpu->MPUReset();
	state  = FORWARD;
	callback = commadCompleteCallback;
	fDistance = distance;
	leftWheel.getWheelDetector()->reset();
	rightWheel.getWheelDetector()->reset();
	pidConstants.applyTunings(PIDConstants::PID_FORWARD,(PIDControlable *)orientator);
	slowMotionFirstDetected = 0;
	wheelBasePower = FORWARD_BASE_POWER;
	orientator->keepOrientation(mpu->getYaw());
};

void MoveDriver::Stop(bool keep) {
	if (keep) {
		state = STOPPING;
		wheelBasePower = 0;
		slowMotionFirstDetected = 0;
		pidConstants.applyTunings(PIDConstants::PID_FAST,(PIDControlable *)orientator);
		orientator->keepOrientation();
	} else {
		state = UNDEFINED;
		orientator->stopOrientation();
		leftWheel.stop();
		rightWheel.stop();
		wheelBasePower = 0;
	}
};

void MoveDriver::TurnLeft(Drive_command_complete_t * commadCompleteCallback) {
	mpu->MPUReset();
	state = TURN_LEFT;
	callback = commadCompleteCallback;
	wheelBasePower = 0;
	pidConstants.applyTunings(PIDConstants::PID_FAST,(PIDControlable *)orientator);
	slowMotionFirstDetected = 0;
	orientator->keepOrientation(mpu->getYaw() + TURN_LEFT_ANGEL);
};

void MoveDriver::TurnRight(Drive_command_complete_t * commadCompleteCallback) {
	mpu->MPUReset();
	state = TURN_RIGHT;
	callback = commadCompleteCallback;
	wheelBasePower = 0;
	pidConstants.applyTunings(PIDConstants::PID_FAST,(PIDControlable *)orientator);
	slowMotionFirstDetected = 0;
	orientator->keepOrientation(mpu->getYaw() + TURN_RIGHT_ANGEL);
};

void MoveDriver::WheelCalibrate(bool leftTurn, bool store) {
	state = CALIBRATE;
	calibrateStarted = millis();
	leftWheel.getWheelDetector()->reset();
	rightWheel.getWheelDetector()->reset();
	leftTurnCalibrate = leftTurn;
	storeCalibrateRatio = store;
	if (leftTurnCalibrate) {
		leftWheel.drive(-FORWARD_BASE_POWER);
		rightWheel.drive(FORWARD_BASE_POWER);
	} else {
		leftWheel.drive(FORWARD_BASE_POWER);
		rightWheel.drive(-FORWARD_BASE_POWER);
	}
};


void MoveDriver::compute() {
	if (state == UNDEFINED) {
		return;
	}
	if (state == CALIBRATE) {
		CalibrateCompute();
	} else if (orientator->isOperating()) {
		if (state == TURN_LEFT || state == TURN_RIGHT || state == STOPPING) {
			TurnCompute();
		} else if (state == FORWARD) {
			ForwardCompute();
		}
		orientator->Compute();

	}
};

void MoveDriver::CalibrateCompute() {
	if ((millis() - calibrateStarted) >= WHEEL_CALIBRATE_TIME) {
		Stop(false);
		state = UNDEFINED;
		int leftTicks = leftWheel.getWheelDetector()->getTicks();
		int rightTicks = rightWheel.getWheelDetector()->getTicks();
		Serial.print("left ticks="); Serial.println(leftTicks);
		Serial.print("right ticks="); Serial.println(rightTicks);
		if (rightTicks > 0) {
			float wheelRatio = ((float)leftTicks/(float) rightTicks);
			if (wheelRatio > WHEEL_MAX_RATIO) {
				wheelRatio = WHEEL_MAX_RATIO;
			} else if (wheelRatio < WHEEL_MIN_RATIO) {
				wheelRatio = WHEEL_MIN_RATIO;
			}
			Serial.print("left-right ratio="); Serial.println(wheelRatio,4);
			float dF = WHEEL_CALIBRATE_COEFF*(1.0 - wheelRatio);
			float leftFactor, rightFactor;
			if (leftTurnCalibrate) {
				leftFactor = leftWheel.getBackwardPowerFactor() + (dF/2.0);
				rightFactor = rightWheel.getForwardPowerFactor() - (dF/2.0);
				if (storeCalibrateRatio) {
					leftWheel.setBackwardPowerFactor(leftFactor);
					rightWheel.setForwardPowerFactor(rightFactor);
				}
			} else {
				leftFactor = leftWheel.getForwardPowerFactor() + (dF/2.0);
				rightFactor = rightWheel.getBackwardPowerFactor() - (dF/2.0);
				if (storeCalibrateRatio) {
					leftWheel.setForwardPowerFactor(leftFactor);
					rightWheel.setBackwardPowerFactor(rightFactor);
				}
			}
			Serial.print("Left "); Serial.println((double)leftFactor,4);
			Serial.print("Right "); Serial.println((double)rightFactor,4);

		} else {
			Serial.println("right ticks zero");
		}
	}
};

void MoveDriver::writeWheelPowerFactor() {
	leftWheel.writePowerFactors();
	rightWheel.writePowerFactors();
}

void MoveDriver::TurnCompute() {
	float offset = abs(orientator->getYawOffset());
	long ct = millis();
	if (mpu->getYawVelocity() >= SLOW_MOTION_THRESHOLD) {
		if (offset > SLOW_MOTION_PID_THRESHOLD) {
			pidConstants.applyTunings(PIDConstants::PID_FAST,(PIDControlable *)orientator);
			slowMotionFirstDetected = 0;
		}
	} else {
		if (offset < SLOW_MOTION_PID_THRESHOLD) {
			if (slowMotionFirstDetected == 0){
				slowMotionFirstDetected = ct;
			} else if ((ct - slowMotionFirstDetected) >= SLOW_MOTION_DETECT_DURATION) {
				pidConstants.applyTunings(PIDConstants::PID_SLOW,(PIDControlable *)orientator);
			}
		} else {
			pidConstants.applyTunings(PIDConstants::PID_FAST,(PIDControlable *)orientator);
			slowMotionFirstDetected = 0;
		}
	}
	if (offset < ACCEPT_TURN_THRESHOLD && mpu->getYawVelocity() == 0.0) {
		Stop(false);
		if (callback) {
			callback();
		}
	}

};

void MoveDriver::ForwardCompute() {
	float dist = 0.5 * (leftWheel.getWheelDetector()->getDistance() + rightWheel.getWheelDetector()->getDistance());
	if (dist != cDist) {
		cDist = dist;
	}
	if ((fDistance - dist) <= FORWARD_DISTANCE_TO_SWITCH_LOW_POWER) {
		wheelBasePower = FORWARD_LOW_POWER;
	}
	if ((fDistance - dist) <= RotateWheelDetector::getMinDetectDistance()) {
		Stop(true);
	}
};



void MoveDriver::printCalibratedValues() {
	mpu->printMPUInit();
	Serial.println("Left:");
	leftWheel.printCalibratedValues();
	Serial.println("Right:");
	rightWheel.printCalibratedValues();
	sonar.printCalibratedValues();
};



/*
 * OrientationDriver implementation
 *
 */

OrientationDriver::OrientationDriver(MoveDriver * d) {
	driver = d;
	pid = new PID((PIDDrivable *)this,ORIENTATION_PID_SAMPLE_TIME);
	input = driver->getMPU()->getYaw();
	yawInitial = input;
	output = 0.0;
	setpoint = 0.0;
	operating = false;
}

bool OrientationDriver::Compute() {
	input = getYawOffset();
	return pid->Compute();
};

void OrientationDriver::keepOrientation(float angel) {
	keepOrientation();
	yawInitial = angel;
};

void OrientationDriver::keepOrientation() {
	operating = true;
};

void OrientationDriver::stopOrientation() {
	operating = false;
};

void OrientationDriver::updatePidCoefficients(float Kp, float Ki, float Kd) {
	pid->setCoefficients(Kp,Ki,Kd);
};


float OrientationDriver::getInput() {
	return input;
};

float OrientationDriver::getOutput() {
	return output;
};

float OrientationDriver::getSetpoint() {
	return setpoint;
};

void OrientationDriver::setOutput(float cOutput) {
	output = cOutput;
	float correction = output/2.0;
	int rightWheelPower = driver->getWheelPower() - (int)round(correction);
	int leftWheelPower = driver->getWheelPower() + (int)round(correction);
	driver->getLeft()->drive(leftWheelPower);
	driver->getRight()->drive(rightWheelPower);
};

float OrientationDriver::getOutputMin() {
	return ORIENTATION_PID_MIN_LIMIT;
};

float OrientationDriver::getOutputMax() {
	return ORIENTATION_PID_MAX_LIMIT;
};

bool OrientationDriver::isOperating() {
	return operating;
};

float OrientationDriver::getYawOffset() {
	//MPU Yaw angel changed from 0 to 180 degree and then -180 to 0 degree,
	//-0 and +0 same angel and 180 and -180 same angel
	float result;
	result = angelTranslateToPositive(yawInitial) - angelTranslateToPositive(driver->getMPU()->getYaw());
	if (result < -180) {
		result = 360 + result;
	} else if (result > 180) {
		result = result - 360;
	}
	return result;
};

float OrientationDriver::angelTranslateToPositive(float a) {
	float result = 0;
	result = ( a >= 0) ? a : 360+a;
	return result;
};
