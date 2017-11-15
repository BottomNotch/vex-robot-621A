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

bool arm1Stalled = false;

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
fbc_t driveLFBC;
fbc_t driveRFBC;
fbc_pid_t arm1PID;
fbc_pid_t arm2PID;
fbc_pid_t driveLPID;
fbc_pid_t driveRPID;


void encodersInit() {
	initEncoder(&arm2Enc, 12, SPEED, TWO_WIRE, TICKS, 5);
	initEncoder(&driveEncL, 9, SPEED, TWO_WIRE, TICKS, 1);
	initEncoder(&driveEncR, 42, SPEED, TWO_WIRE, TICKS, 1);
}

void motorsInit() {
	blrsMotorInit(claw,  false, DEFAULT_SLEW_RATE, NULL);
	blrsMotorInit(mogoL, true, DEFAULT_SLEW_RATE, NULL);
	blrsMotorInit(mogoR, true, DEFAULT_SLEW_RATE, NULL);
	blrsMotorInit(RDrive2, true, DEFAULT_SLEW_RATE, NULL);
	blrsMotorInit(RDrive1, true, DEFAULT_SLEW_RATE, NULL);
	blrsMotorInit(LDrive1, false, DEFAULT_SLEW_RATE, NULL);
	blrsMotorInit(LDrive2, false, DEFAULT_SLEW_RATE, NULL);
	blrsMotorInit(arm1, true, 0.35, NULL);
	blrsMotorInit(arm2, true, 2, NULL);
}

void driveLSet(int power) {
	blrsMotorSet(LDrive1, power, false);
	blrsMotorSet(LDrive2, power, false);
}

void driveRSet(int power) {
	blrsMotorSet(RDrive1, power, false);
	blrsMotorSet(RDrive2, power, false);
}

void armSetStage1(int power) {
	if(!arm1Stalled) {
		blrsMotorSet(arm1, power, true);
	}

	else {
		blrsMotorSet(arm1, 0, true);
	}
}

void armSetStage2(int power) {
	if(fbcStallDetect(&arm2FBC)) {
		fbcSetGoal(&arm2FBC, (int)getSensor(arm2Enc));
	}

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

void clawMove() {
	static bool open = false;
	if(!open) {
		blrsMotorSet(claw, -65, true);
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

int _arm2Sense() {
	return (int)getSensor(arm2Enc);
}

int _driveLSense() {
	return (int)getSensor(driveEncL);
}

int _driveRSense() {
	return (int)getSensor(driveEncR);
}

void initFBCControllers() {
	fbcInit(&arm1FBC, &armSetStage1, &_arm1Sense, NULL, NULL, -20, 20, 250, 15);
	fbcInit(&arm2FBC, &armSetStage2, &_arm2Sense, NULL, fbcStallDetect, -15, 15, 50, 15);
	fbcInit(&driveLFBC, &driveLSet, &_driveLSense, NULL, NULL, -15, 15, 50, 15);
	fbcInit(&driveRFBC, &driveRSet, &_driveRSense, NULL, NULL, -15, 15, 50, 15);
	fbcPIDInitializeData(&arm1PID, 0.15, 0, 80, 0, 0);
	fbcPIDInitializeData(&arm2PID, 0.6, 0, 40, 0, 0);
	fbcPIDInitializeData(&driveLPID, 0.7, 0, 0, 0, 0);
	fbcPIDInitializeData(&driveRPID, 0.7, 0, 0, 0, 0);
	fbcPIDInit(&arm1FBC, &arm1PID);
	fbcPIDInit(&arm2FBC, &arm2PID);
	fbcPIDInit(&driveLFBC, &driveLPID);
	fbcPIDInit(&driveRFBC, &driveRPID);
}


