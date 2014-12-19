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



#include <Arduino.h>
#include "MoveDriver.hpp"
#include "CLI.hpp"

#define BOT_NAME "BOTKAA3"

#define EEPROM_DRIVER_BASE 0x1

/*
 * Sonar
 */

#define SONAR_TRIGGER_PIN  12
#define SONAR_ECHO_PIN     10

/*
 * Turn detectors constants
 */
#define TURN_DETECTOR_RIGHT_PIN 5
#define TURN_DETECTOR_LEFT_PIN 6

/*
 * Wheel PINS
 */

#define MOTOR_R_DIRECTION_PIN1 2
#define MOTOR_R_DIRECTION_PIN2 4
#define MOTOR_R_SPEED_PIN 3

#define MOTOR_L_DIRECTION_PIN1 7
#define MOTOR_L_DIRECTION_PIN2 8
#define MOTOR_L_SPEED_PIN 11

#define SERVO_PIN 14
#define LED_PIN 13

MoveDriver driver(EEPROM_DRIVER_BASE,SERVO_PIN, SONAR_ECHO_PIN, SONAR_TRIGGER_PIN);

CLI * cliInterface;
CLI * initCliInterface();

volatile static bool setupOK = false;


void setup() {
//	Serial.begin(9600);
//	Serial.print("AT+NAME"BOT_NAME);
//	delay(1000);
//	Serial.print("AT+BAUD4");
//	delay(1000);

	Serial.begin(9600);
	pinMode(LED_PIN,OUTPUT);

	digitalWrite(LED_PIN,LOW);

#ifdef CALIBRATE_CODE
	Serial.println("Calibrate version");
#else
	Serial.println("Run version");
#endif

	Serial.println("Setup start");
	driver.init();
	//Serial.println("Motor driver init complete");
	driver.setupRight(MOTOR_R_SPEED_PIN, MOTOR_R_DIRECTION_PIN1, MOTOR_R_DIRECTION_PIN2, TURN_DETECTOR_RIGHT_PIN);
	driver.setupLeft(MOTOR_L_SPEED_PIN, MOTOR_L_DIRECTION_PIN1, MOTOR_L_DIRECTION_PIN2, TURN_DETECTOR_LEFT_PIN);
	//Serial.println("Motor driver setup complete");



	cliInterface = initCliInterface();

	Serial.println("Complete");
	driver.printCalibratedValues();
	digitalWrite(LED_PIN,HIGH);
	setupOK = true;
}



void loop() {
	driver.Compute();
	cliInterface->Compute();
}

/*
 * CLI functions
 */

void Sonar_Measure(float distance) {
	Serial.print("p"); Serial.println(distance);
	cliInterface->commandComplete();
}

void Forward_Complete() {
	Serial.println("f");
	//float LD = driver.getLeft()->getWheelDetector()->getDistance();
	//float RD = driver.getRight()->getWheelDetector()->getDistance();
	//Serial.print(LD); Serial.print(","); Serial.println(RD);
	cliInterface->commandComplete();
}

void TurnLeft_Complete() {
	Serial.println("l");
	cliInterface->commandComplete();
}
void TurnRight_Complete() {
	Serial.println("r");
	cliInterface->commandComplete();
}

void MPUCalibrate_Complete() {
	Serial.println("z");
	digitalWrite(LED_PIN,HIGH);
	cliInterface->commandComplete();
}

void keepAliveAction() {
	Serial.println("k");
	cliInterface->commandComplete();
}

void forwardAction() {
	driver.Forward(Forward_Complete,360);
}

void leftTurnAction() {
	driver.TurnLeft(TurnLeft_Complete);
}

void rightTurnAction() {
	driver.TurnRight(TurnRight_Complete);
}

void stopAction() {
	driver.Stop(false);
	cliInterface->commandComplete();
}

void pingLeftAction() {
	driver.getSonar()->measure(Sonar_Measure,Sonar::LEFT);
}
void pingFrontAction() {
	driver.getSonar()->measure(Sonar_Measure,Sonar::FRONT);
}

void pingRightAction() {
	driver.getSonar()->measure(Sonar_Measure,Sonar::RIGHT);
}

void calibrateServoAction() {
	driver.getSonar()->calibrateServo();
	cliInterface->commandComplete();
}

void writeServoAction() {
	driver.getSonar()->writeServoToEEPROM();
	cliInterface->commandComplete();
}

void calibrateSoundVelocityAction() {
	driver.getSonar()->calibrateSoundVelocity();
	cliInterface->commandComplete();
}

void writeSoundVelocityAction() {
	driver.getSonar()->writeSoundVelocityToEEPROM();
	cliInterface->commandComplete();
}

void calibrateMPUAction() {
	digitalWrite(LED_PIN,LOW);
	driver.getMPU()->calibrate(MPUCalibrate_Complete);
}

void verbousOutput() {

#ifdef CALIBRATE_CODE
	driver.printCalibratedValues();
#else
	Serial.print("MPU: yaw="); Serial.print(driver.getMPU()->getYaw());
	Serial.println(" ");
	driver.getLeft()->getWheelDetector()->printHistory();
	driver.getRight()->getWheelDetector()->printHistory();
	driver.getSonar()->print();
#endif
	cliInterface->commandComplete();
}

void testLeftWheelAction() {
	driver.WheelCalibrate(true,false);
	cliInterface->commandComplete();
}

void calibrateLeftWheelAction() {
	driver.WheelCalibrate(true,true);
	cliInterface->commandComplete();
}

void testRightWheelAction() {
	driver.WheelCalibrate(false,false);
	cliInterface->commandComplete();
}

void calibrateRightWheelAction() {
	driver.WheelCalibrate(false,true);
	cliInterface->commandComplete();
}

void writeWheelPowerFactorsAction() {
	driver.writeWheelPowerFactor();
	cliInterface->commandComplete();
}

CLI * initCliInterface() {
	CLI * c = new CLI();
	c->addAction('k', keepAliveAction);
	c->addAction('v', verbousOutput);
#ifndef CALIBRATE_CODE
	c->addAction('f',forwardAction);
	c->addAction('F',forwardAction);
	c->addAction('l', leftTurnAction);
	c->addAction('r', rightTurnAction);
	c->addAction('s', stopAction);
	c->addAction('b', pingLeftAction);
	c->addAction('n', pingFrontAction);
	c->addAction('m', pingRightAction);
	c->addAction('w', calibrateSoundVelocityAction);
	c->addAction('W', writeSoundVelocityAction);

#else
	c->addAction('x', testLeftWheelAction);
	c->addAction('X', calibrateLeftWheelAction);
	c->addAction('c', testRightWheelAction);
	c->addAction('C', calibrateRightWheelAction);
	c->addAction('D', writeWheelPowerFactorsAction);
	c->addAction('q', calibrateServoAction);
	c->addAction('Q', writeServoAction);
	c->addAction('z', calibrateMPUAction);
#endif
	Serial.println(" OK");
	return c;
}
