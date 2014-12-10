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
 * Accelgyro.hpp
 *
 *  Created on: Nov 11, 2014
 *      Author: Andriy Panasenko <apanasenko@cybervisiontech.com>
 */

#ifndef ACCELGYRO_HPP_
#define ACCELGYRO_HPP_

#include <Arduino.h>
#include "EEPROMex.h"

#define X 0
#define Y 1
#define Z 2

#define ACCEL_DEADZONE 8
#define GIRO_DEADZONE 4

#define EEPROM_INIT_PATTERN 0x23

//Default values for MPU6050
//#define DEF_ACC_X 1580
//#define DEF_ACC_Y 1290
//#define DEF_ACC_Z 1195
//#define DEF_GIR_X 29
//#define DEF_GIR_Y -8
//#define DEF_GIR_Z 8
#define DEF_ACC_X 0
#define DEF_ACC_Y 0
#define DEF_ACC_Z 0
#define DEF_GIR_X 0
#define DEF_GIR_Y 0
#define DEF_GIR_Z 0

typedef void Calibrate_command_complete_t();

#define FIFO_SIZE 64

class MPU6050;
class Quaternion;
class VectorFloat;
class VectorInt16;

class Accelgyro {
	private:
		MPU6050 * mpu;

		int eepromMemBase;
		int initAddress;
		int initAccAddress;
		int initGirAddress;
		int defAcc[3];
		int defGir[3];


		Quaternion * q;           // [w, x, y, z]         quaternion container
		VectorFloat * gravity;    // [x, y, z]            gravity vector
		VectorInt16 * aa;			// [x, y, z]            accel sensor measurements
		float ypr[3];           // [yaw, pitch, roll]   yaw/pitch/roll container and gravity vector
		int16_t ac[3];			// [x, y, z] acceleration
		int16_t gy[3];			// [x, y, z] gyro
//
//		int32_t giro[3];		// [x, y, z] gyro

		float yawVelocity;
		float prevYaw;
		long prevYawMicros;
		uint16_t packetSize;    // expected DMP packet size (default is 42 bytes)
		uint16_t fifoCount;     // count of all bytes currently in FIFO
		uint8_t fifoBuffer[FIFO_SIZE]; // FIFO storage buffer
		uint8_t devStatus;      // return status after each device operation (0 = success, !0 = error)
	public:
		Accelgyro();

		void init();

		void calibrate(Calibrate_command_complete_t * callback);

		void MPUReset();

		bool Compute();


		float getYaw();

		float getYawVelocity();

//		int getAccelMpuX();
//
//		int getAccelMpuY();

		void printMPUInit();
};



#endif /* ACCELGYRO_HPP_ */
