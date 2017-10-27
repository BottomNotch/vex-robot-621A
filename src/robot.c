#include "main.h"

const char claw = 2;
const char mogoL = 3;
const char mogoR = 1;
const char RDrive2 = 4;
const char RDrive1 = 5;
const char LDrive1 = 6;
const char LDrive2 = 7;
const char arm1 = 8;
const char arm2 = 9;

simpleSensor powerExpander = {1, ANALOG, false};
simpleSensor autoSelect = {2, ANALOG, false};
simpleSensor arm1Pot = {3, ANALOG, false};
simpleSensor mogoPotL = {5, ANALOG, false};
simpleSensor mogoPotR = {4, ANALOG, false};
simpleSensor arm2Enc = {11, OTHER, false};
simpleSensor driveEncL = {8, OTHER, false};
simpleSensor driveEncR = {6, OTHER, false};

fbc_t arm1FBC;
fbc_t arm2FBC;
fbc_t mogoLFBC;
fbc_t mogoRFBC;
fbc_pid_t arm1PID;
fbc_pid_t arm2PID;
fbc_pid_t mogoLPID;
fbc_pid_t mogoRPID;

void encodersInit() {
	initEncoder(&arm2Enc, 12, SPEED, TWO_WIRE, TICKS, 5);
	initEncoder(&driveEncL, 9, SPEED, TWO_WIRE, TICKS, 1);
	initEncoder(&driveEncR, 42, SPEED, TWO_WIRE, TICKS, 1);
}

void motorsInit() {
	blrsMotorInit(claw, true, DEFAULT_SLEW_RATE, NULL);
	blrsMotorInit(mogoL, true, DEFAULT_SLEW_RATE, NULL);
	blrsMotorInit(mogoR, true, DEFAULT_SLEW_RATE, NULL);
	blrsMotorInit(RDrive2, true, DEFAULT_SLEW_RATE, NULL);
	blrsMotorInit(RDrive1, true, DEFAULT_SLEW_RATE, NULL);
	blrsMotorInit(LDrive1, false, DEFAULT_SLEW_RATE, NULL);
	blrsMotorInit(LDrive2, false, DEFAULT_SLEW_RATE, NULL);
	blrsMotorInit(arm1, true, 0.35, NULL);
	blrsMotorInit(arm2, true, 2, NULL);
}

void driveSet(int left, int right) {
	blrsMotorSet(LDrive1, left, false);
	blrsMotorSet(LDrive2, left, false);
	blrsMotorSet(RDrive1, right, false);
	blrsMotorSet(RDrive2, right, false);
}

void armSetStage1(int power) {
		blrsMotorSet(arm1, power, true);
}

void armSetStage2(int power) {
	blrsMotorSet(arm2, power, false);
}

void armSetBothStages(int stage1, int stage2) {
	armSetStage1(stage1);
	armSetStage2(stage2);
}

void mogoSet(int power) {
	blrsMotorSet(mogoL, power, false);
	blrsMotorSet(mogoR, power, false);
}

void mogoLSet(int power) {
	blrsMotorSet(mogoL, power, true);
}

void mogoRSet(int power) {
	blrsMotorSet(mogoR, power, true);
}

void clawMove() {
	static bool open = true;
	if(!open) {
		blrsMotorSet(claw, -80, true);
		open = true;
	}

	else {
		blrsMotorSet(claw, 50, true);
		open = false;
	}
}

int _arm1Sense() {
	static bool armMoveDown = false;
	static lastGoal = 0;
	static long lastMillis;
	int timePassed;
	timePassed = millis() - lastMillis;

	if(arm1FBC.goal > ARM_1_LOWER_P) {
		arm1PID.kP = 0.13;
	}

	else {
		arm1PID.kP = 0.2;
	}
	return (int)getSensor(arm1Pot);
}

int _arm2Sense() {
	return (int)getSensor(arm2Enc);
}

int _mogoLSense() {
	fbcSetGoal(&mogoRFBC, mogoLFBC.goal + MOGO_OFFSET);
	return (int)getSensor(mogoPotL);
}

int _mogoRSense() {
	return (int)getSensor(mogoPotR);
}

void initFBCControllers() {
	fbcInit(&arm1FBC, &armSetStage1, &_arm1Sense, NULL, NULL, -1, 1, 250, 15);
	fbcInit(&arm2FBC, &armSetStage2, &_arm2Sense, NULL, NULL, -1, 1, 5, 10);
	fbcInit(&mogoLFBC, &mogoLSet, &_mogoLSense, NULL, NULL, -1, 1, 50, 15);
	fbcInit(&mogoRFBC, &mogoRSet, &_mogoRSense, NULL, NULL, -1, 1, 50, 15);
	fbcPIDInitializeData(&arm1PID, 0.2, 0, 10, 0, 0);
	fbcPIDInitializeData(&arm2PID, 0.7, 0, 0, 0, 0);
	fbcPIDInitializeData(&mogoLPID, 0.7, 0, 170, 0, 0);
	fbcPIDInitializeData(&mogoRPID, 0.7, 0, 170, 0, 0);
	fbcPIDInit(&arm1FBC, &arm1PID);
	fbcPIDInit(&arm2FBC, &arm2PID);
	fbcPIDInit(&mogoLFBC, &mogoLPID);
	fbcPIDInit(&mogoRFBC, &mogoRPID);
}
