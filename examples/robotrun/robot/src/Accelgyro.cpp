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
 * Accelgyro.cpp
 *
 *  Created on: Dec 1, 2014
 *      Author: Andriy Panasenko <apanasenko@cybervisiontech.com>
 */

#include "Accelgyro.hpp"
#include <MPU6050_6Axis_MotionApps20.h>


Accelgyro::Accelgyro() :
	eepromMemBase(0),
	initAddress(0),
	initAccAddress(0),
	initGirAddress(0),
	yawVelocity(0),
	prevYaw(0),
	prevYawMicros(0),
	packetSize(0),
	fifoCount(0),
	devStatus(0)
{
	mpu = new MPU6050();

	for(int i=0;i<FIFO_SIZE;i++) {
		fifoBuffer[i] = 0;
	}
	defAcc[X] = DEF_ACC_X;
	defAcc[Y] = DEF_ACC_Y;
	defAcc[Z] = DEF_ACC_Z;
	defGir[X] = DEF_GIR_X;
	defGir[Y] = DEF_GIR_Y;
	defGir[Z] = DEF_GIR_Z;

	q = new Quaternion();
	gravity = new VectorFloat();
	aa = new VectorInt16();
};

void Accelgyro::init() {
	Wire.begin();
	//Serial.println(F("Initializing I2C devices..."));
	mpu->initialize();

	// verify connection
	//Serial.println(F("Testing device connections..."));
	//Serial.println(mpu.testConnection() ? F("MPU6050 connection successful") : F("MPU6050 connection failed"));



	//Serial.println(F("Initializing EEPROM..."));

	initAddress = EEPROM.getAddress(sizeof(byte));
	initAccAddress = EEPROM.getAddress(sizeof(defAcc));
	initGirAddress = EEPROM.getAddress(sizeof(defGir));
//			Serial.print("initAddres="); Serial.println(initAddress);
//			Serial.print("initAccAddress="); Serial.println(initAccAddress);
//			Serial.print("initGirAddress="); Serial.println(initGirAddress);
//
//
	byte inited = EEPROM.readByte(initAddress);
	while(!EEPROM.isReady()) { delay(1); }
	if (inited == EEPROM_INIT_PATTERN) {
		EEPROM.readBlock<int>(initAccAddress,defAcc,3);
		while(!EEPROM.isReady()) { delay(1); }
		EEPROM.readBlock<int>(initGirAddress,defGir,3);
		while(!EEPROM.isReady()) { delay(1); }
		Serial.println(F("MPU from EEPROM"));
	} else {
		Serial.println(F("MPU precompiled"));
	}




	//Serial.println(F("Initializing DMP..."));
	devStatus = mpu->dmpInitialize();
	if (devStatus == 0) {
		mpu->setXGyroOffset(defGir[X]);
		mpu->setYGyroOffset(defGir[Y]);
		mpu->setZGyroOffset(defGir[Z]);
		mpu->setXAccelOffset(defAcc[X]);
		mpu->setYAccelOffset(defAcc[Y]);
		mpu->setZAccelOffset(defAcc[Z]);

		// turn on the DMP, now that it's ready
		//Serial.println(F("Enabling DMP..."));
		mpu->setDMPEnabled(true);
		mpu->setDLPFMode(4);
		mpu->setDHPFMode(4);

//		uint8_t intVector = 0;
//		intVector |= 0x01<<MPU6050_INTERRUPT_DMP_INT_BIT;
//		intVector |= 0x01<<MPU6050_INTERRUPT_FIFO_OFLOW_BIT;
//		intVector |= 0x01<<MPU6050_INTERRUPT_ZMOT_BIT;
//		intVector |= 0x01<<MPU6050_INTERRUPT_MOT_BIT;
//		mpu->setIntEnabled(intVector);
//		//mpu.setIntZeroMotionEnabled(true);
//		mpu->setZeroMotionDetectionThreshold(1);
//		mpu->setZeroMotionDetectionDuration(100);
//		mpu->setMotionDetectionThreshold(1);
//		mpu->setMotionDetectionDuration(10);
		//mpu.setIntFIFOBufferOverflowEnabled(true);
		//Serial.println(F("Enabling ZMotion detector ..."));
		//mpu.setFullScaleAccelRange(0);
		//mpu.setFullScaleGyroRange(1);
		packetSize = mpu->dmpGetFIFOPacketSize();
	} //else initialization failed
	//Serial.println(F("MPU initialization complete"));
};

void Accelgyro::calibrate(Calibrate_command_complete_t * callback) {
	//Serial.println("Start MPU6050 calibration....");
	bool complete = false;
	long t = 0;
	int mgy[3], iac[3], igy[3];
	long mac[3];
	bool ac_ready[3], gi_ready[3];
	mac[X] = 0; mac[Y] = 0; mac[Z] = 0;
	mgy[X] = 0; mgy[Y] = 0; mgy[Z] = 0;
	iac[X] = DEF_ACC_X; iac[Y] = DEF_ACC_Y; iac[Z] = DEF_ACC_Z;
	igy[X] = DEF_GIR_X; igy[Y] = DEF_GIR_Y; igy[Z] = DEF_GIR_Z;
	ac_ready[X] = false; ac_ready[Y] = false; ac_ready[Z] = false;
	gi_ready[X] = false; gi_ready[Y] = false; gi_ready[Z] = false;
	while(!complete) {
		int i = 0;
		t = millis();
		//Serial.print(">"); Serial.print(t); Serial.println(": MPU ....");
		while(i<100) {
			if (Compute()) {
				i++;
			}
		}
		t = millis();
		//Serial.print(t); Serial.println(": MPU .... 100 reading complete");

		i = 0;
		mac[X] = 0; mac[Y] = 0; mac[Z] = 0;
		mgy[X] = 0; mgy[Y] = 0; mgy[Z] = 0;
		while(i<1000) {
			if (Compute()) {
				i++;
				mac[X] += ac[X];
				mac[Y] += ac[Y];
				mac[Z] += ac[Z];
				mgy[X] += gy[X];
				mgy[Y] += gy[Y];
				mgy[Z] += gy[Z];
			}
		}
		mac[X] = mac[X]/(i+1);
		mac[Y] = mac[Y]/(i+1);
		mac[Z] = mac[Z]/(i+1);
		mgy[X] = mgy[X]/(i+1);
		mgy[Y] = mgy[Y]/(i+1);
		mgy[Z] = mgy[Z]/(i+1);
		//Serial.print("iac[X]="); Serial.print(iac[X]); Serial.print(" mac[X]="); Serial.println(mac[X]);
		//Serial.print("iac[Y]="); Serial.print(iac[Y]); Serial.print(" mac[Y]="); Serial.println(mac[Y]);
		//Serial.print("iac[Z]="); Serial.print(iac[Z]); Serial.print(" mac[Z]="); Serial.println(mac[Z]);
		//Serial.print("igy[X]="); Serial.print(igy[X]); Serial.print(" mgy[X]="); Serial.println(mgy[X]);
		//Serial.print("igy[Y]="); Serial.print(igy[Y]); Serial.print(" mgy[Y]="); Serial.println(mgy[Y]);
		//Serial.print("igy[Z]="); Serial.print(igy[Z]); Serial.print(" mgy[Z]="); Serial.println(mgy[Z]);

		if (!ac_ready[X]) {
			if(abs(mac[X])<=ACCEL_DEADZONE) {
				ac_ready[X] = true;
			} else {
				iac[X]=iac[X]-mac[X]/ACCEL_DEADZONE;
				mpu->setXAccelOffset(iac[X]);
				//Serial.print(".iac[X]="); Serial.println(iac[X]);
			}
		}

		if (!ac_ready[Y]) {
			if(abs(mac[Y])<=ACCEL_DEADZONE) {
				ac_ready[Y] = true;
			} else {
				iac[Y]=iac[Y]-mac[Y]/ACCEL_DEADZONE;
				mpu->setYAccelOffset(iac[Y]);
				//Serial.print(".iac[Y]="); Serial.println(iac[Y]);
			}
		}

		if (!ac_ready[Z]) {
			if(abs(16384-mac[Z])<=ACCEL_DEADZONE) {
				ac_ready[Z] = true;
			} else {
				iac[Z]=iac[Z]+(16384-mac[Z])/ACCEL_DEADZONE;
				mpu->setZAccelOffset(iac[Z]);
				//Serial.print(".iac[Z]="); Serial.println(iac[Z]);
			}
		}

		if (!gi_ready[X]) {
			if(abs(mgy[X])<=GIRO_DEADZONE) {
				gi_ready[X] = true;
			} else {
				igy[X]=igy[X]-mgy[X]/(GIRO_DEADZONE + 1);
				mpu->setXGyroOffset(igy[X]);
				//Serial.print(".igy[X]="); Serial.println(igy[X]);
				//Serial.print(".mgy[X]="); Serial.println(mgy[X]);
			}
		}

		if (!gi_ready[Y]) {
			if(abs(mgy[Y])<=GIRO_DEADZONE) {
				gi_ready[Y] = true;
			} else {
				igy[Y]=igy[Y]-mgy[Y]/(GIRO_DEADZONE + 1);
				mpu->setXGyroOffset(igy[Y]);
				//Serial.print(".igy[Y]="); Serial.println(igy[Y]);
				//Serial.print(".mgy[Y]="); Serial.println(mgy[Y]);
			}
		}

		if (!gi_ready[Z]) {
			if(abs(mgy[Z])<=GIRO_DEADZONE) {
				gi_ready[Z] = true;
			} else {
				igy[Z]=igy[Z]-mgy[Z]/(GIRO_DEADZONE + 1);
				mpu->setXGyroOffset(igy[Z]);
				//Serial.print(".igy[Z]="); Serial.println(igy[Z]);
				//Serial.print(".mgy[Z]="); Serial.println(mgy[Z]);
			}
		}
		//Serial.println(" ");
		if (   ac_ready[X]
			&& ac_ready[Y]
			&& ac_ready[Z]
			&& gi_ready[X]
			&& gi_ready[Y]
			&& gi_ready[Z]) {

			complete = true;
			defAcc[X] = iac[X];
			defAcc[Y] = iac[Y];
			defAcc[Z] = iac[Z];
			defGir[X] = igy[X];
			defGir[Y] = igy[Y];
			defGir[Z] = igy[Z];
//					Serial.println(": MPU .... calibration complete");
//					Serial.print("!iac[X]="); Serial.println(iac[X]);
//					Serial.print("!iac[Y]="); Serial.println(iac[Y]);
//					Serial.print("!iac[Z]="); Serial.println(iac[Z]);
//					Serial.print("!igy[X]="); Serial.println(igy[X]);
//					Serial.print("!igy[Y]="); Serial.println(igy[Y]);
//					Serial.print("!igy[Z]="); Serial.println(igy[Z]);
		}

	} //end of while
	//Store to EEPROM
	EEPROM.updateByte(initAddress,EEPROM_INIT_PATTERN);
	while(!EEPROM.isReady()) { delay(1); }

	EEPROM.updateBlock<int>(initAccAddress, defAcc, 3);
	while(!EEPROM.isReady()) { delay(1); }

	EEPROM.updateBlock<int>(initGirAddress, defGir, 3);
	while(!EEPROM.isReady()) { delay(1); }

	if (callback) {
		callback();
	}
};

void Accelgyro::MPUReset() {
	mpu->resetFIFO();
};

bool Accelgyro::Compute() {
	bool dataUpdated = false;
//			cli();
	uint8_t mpuIntStatus = mpu->getIntStatus();
	fifoCount = mpu->getFIFOCount();
//			sei();


	if (mpuIntStatus & (0x01<<MPU6050_INTERRUPT_DMP_INT_BIT)) {
		if (fifoCount >= packetSize) {
			// read a packet from FIFO
//					cli();
			mpu->getFIFOBytes(fifoBuffer, packetSize);
//					sei();
			dataUpdated = true;
			mpu->dmpGetQuaternion(q, fifoBuffer);
//			mpu->dmpGetAccel(aa, fifoBuffer);
//					mpu.dmpGetGyro(&giro[0], fifoBuffer);
			mpu->dmpGetGravity(gravity, q);
			mpu->dmpGetYawPitchRoll(ypr, q, gravity);
			mpu->getMotion6(&ac[X], &ac[Y], &ac[Z], &gy[X], &gy[Y], &gy[Z]);
			long ctime = micros();
			double dt = ((double)(ctime - prevYawMicros))/(double)1000000;
			yawVelocity = ((double)(prevYaw - getYaw()))/dt;
			prevYawMicros = ctime;
			prevYaw = getYaw();
		}
	}
	if ((mpuIntStatus & (0x01<<MPU6050_INTERRUPT_FIFO_OFLOW_BIT)) || fifoCount == 1024) {
		MPUReset();
		//Serial.println("ERROR: MPU FIFO overflow");
	}
	return dataUpdated;
};

float Accelgyro::getYaw() {
	return ypr[0] * 180/M_PI;
};

float Accelgyro::getYawVelocity() {
	return yawVelocity;
};

//int Accelgyro::getAccelMpuX() {
//	return aa->x;
//};
//
//int Accelgyro::getAccelMpuY() {
//	return aa->y;
//};

void Accelgyro::printMPUInit() {
	Serial.print("Acc: "); Serial.print(defAcc[X]); Serial.print(","); Serial.print(defAcc[Y]); Serial.print(","); Serial.println(defAcc[Z]);
	Serial.print("Gir: "); Serial.print(defGir[X]); Serial.print(","); Serial.print(defGir[Y]); Serial.print(","); Serial.println(defGir[Z]);
};
