#include "main.h"

const char claw = 2;
const char mogo = 3;
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
fbc_pid_t arm1PID;
fbc_pid_t arm2PID;

void encodersInit() {
	initEncoder(&arm2Enc, 12, SPEED, TWO_WIRE, TICKS, 5);
	initEncoder(&driveEncL, 9, SPEED, TWO_WIRE, TICKS, 1);
	initEncoder(&driveEncR, 42, SPEED, TWO_WIRE, TICKS, 1);
}

void motorsInit() {
	blrsMotorInit(claw, true, DEFAULT_SLEW_RATE, NULL);
	blrsMotorInit(mogo, true, DEFAULT_SLEW_RATE, NULL);
	blrsMotorInit(RDrive2, true, DEFAULT_SLEW_RATE, NULL);
	blrsMotorInit(RDrive1, true, DEFAULT_SLEW_RATE, NULL);
	blrsMotorInit(LDrive1, false, DEFAULT_SLEW_RATE, NULL);
	blrsMotorInit(LDrive2, false, DEFAULT_SLEW_RATE, NULL);
	blrsMotorInit(arm1, true, 0.55, NULL);
	blrsMotorInit(arm2, true, 2, NULL);
}

void driveSet(int left, int right) {
	blrsMotorSet(LDrive1, left, false);
	blrsMotorSet(LDrive2, left, false);
	blrsMotorSet(RDrive1, right, false);
	blrsMotorSet(RDrive2, right, false);
}

void armSetStage1(int power) {
	if(power > 0) {
		blrsMotorSet(arm1, power, false);
	}

	else {
		blrsMotorSet(arm1, power, false);
	}
}

void armSetStage2(int power) {
	blrsMotorSet(arm2, power, false);
}

void armSetBothStages(int stage1, int stage2) {
	armSetStage1(stage1);
	armSetStage2(stage2);
}

void mogoSet(int power) {
	blrsMotorSet(mogo, power, false);
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
	return (int)getSensor(arm1Pot);
}

void arm1Reset(fbc_t *fbc) {
	if(fbc->goal > ARM_1_LOWER_P) {
		arm1PID.kP = 0.13;
	}

	else {
		arm1PID.kP = 0.3;
	}
}

int _arm2Sense() {
	return (int)getSensor(arm2Enc);
}
void initFBCControllers() {
	fbcInit(&arm1FBC, &armSetStage1, &_arm1Sense, &fbcStallDetect, &arm1Reset, -1, 1, 15, 50);
	fbcInit(&arm2FBC, &armSetStage2, &_arm2Sense, &fbcStallDetect, NULL, -1, 1, 5, 10);
	fbcPIDInitializeData(&arm1PID, 0.3, 0, 80, 0, 0);
	fbcPIDInitializeData(&arm2PID, 0.7, 0, 0, 0, 0);
	fbcPIDInit(&arm1FBC, &arm1PID);
	fbcPIDInit(&arm2FBC, &arm2PID);
}
