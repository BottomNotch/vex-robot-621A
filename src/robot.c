#include "main.h"

const char intake = 2;
const char wrist = 3;
const char RDriveB = 1;
const char RDriveF = 4;
const char RDriveM = 5;
const char LDriveM = 6;
const char LDriveF = 7;
const char LDriveB = 10;
const char arm1 = 8;
const char arm2 = 9;

simpleSensor powerExpander = {1, ANALOG, false};
simpleSensor autoSelect = {2, ANALOG, false};
simpleSensor arm1Pot = {3, ANALOG, false};
simpleSensor arm2Enc = {11, OTHER, false};
simpleSensor driveEncL = {8, OTHER, false};
simpleSensor driveEncR = {6, OTHER, false};
simpleSensor mogoSwitch = {5, DIGITAL, false};

fbc_t arm1FBC;
fbc_t arm2FBC;
fbc_t driveLFBC;
fbc_t driveRFBC;
fbc_pid_t arm1PID;
fbc_pid_t arm2PID;
fbc_pid_t driveLPID;
fbc_pid_t driveRPID;

char _LDrivePower = 0;
char _RDrivePower = 0;
char _mogoPower = 0;


void encodersInit() {
	initEncoder(&arm2Enc, 12, SPEED, TWO_WIRE, TICKS, 5);
	initEncoder(&driveEncL, 9, SPEED, TWO_WIRE, TICKS, 1);
	initEncoder(&driveEncR, 42, SPEED, TWO_WIRE, TICKS, 1);
}

void motorsInit() {
	blrsMotorInit(intake,  false, DEFAULT_SLEW_RATE, NULL);
	blrsMotorInit(wrist, true, DEFAULT_SLEW_RATE, NULL);
	blrsMotorInit(RDriveB, true, DEFAULT_SLEW_RATE, NULL);
	blrsMotorInit(RDriveM, true, DEFAULT_SLEW_RATE, NULL);
	blrsMotorInit(RDriveF, true, DEFAULT_SLEW_RATE, NULL);
	blrsMotorInit(LDriveF, false, DEFAULT_SLEW_RATE, NULL);
	blrsMotorInit(LDriveM, false, DEFAULT_SLEW_RATE, NULL);
	blrsMotorInit(LDriveB, false, DEFAULT_SLEW_RATE, NULL);
	blrsMotorInit(arm1, false, 0.35, NULL);
	blrsMotorInit(arm2, true, 2, NULL);
}

//the drive and mogo lift share motors, which makes controlling those mechanisms
//more complex, this task makes it easier.
void driveAndMogoTask(void *ignore) {
	while(true) {
		blrsMotorSet(RDriveF, _RDrivePower + _mogoPower, false);
		blrsMotorSet(RDriveM, -_RDrivePower + _mogoPower, false);
		blrsMotorSet(LDriveF, _LDrivePower + _mogoPower, false);
		blrsMotorSet(LDriveM, -_LDrivePower + _mogoPower, false);
		blrsMotorSet(RDriveB, _RDrivePower, false);
		blrsMotorSet(LDriveB, _LDrivePower, false);
		delay(20);
	}
}

void driveLSet(int power) {
	if(driveLFBC.isStalled) {
		fbcSetGoal(&driveLFBC, (int)getSensor(driveEncL));
	}
	_LDrivePower = power;
}

void driveRSet(int power) {
	if(driveRFBC.isStalled) {
		fbcSetGoal(&driveRFBC, (int)getSensor(driveEncR));
	}
	_RDrivePower = power;
}

void armSetStage1(int power) {
	if(arm1FBC.isStalled) {
		fbcSetGoal(&arm1FBC, (int)getSensor(arm1Pot));
	}

	blrsMotorSet(arm1, power, true);
}

void armSetStage2(int power) {
	if(arm2FBC.isStalled) {
		fbcSetGoal(&arm2FBC, (int)getSensor(arm2Enc));
	}

	blrsMotorSet(arm2, power, false);
}

void armSetBothStages(int stage1, int stage2) {
	armSetStage1(stage1);
	armSetStage2(stage2);
}

void mogoSet(int power) {
	_mogoPower = power;
}

void clawMove() {
	static bool open = false;
	if(!open) {
		blrsMotorSet(intake, -65, true);
		open = true;
	}

	else
		{
		blrsMotorSet(intake, 50, true);
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
	fbcInit(&arm1FBC, &armSetStage1, &_arm1Sense, NULL, fbcStallDetect, -20, 20, 250, 15);
	fbcInit(&arm2FBC, &armSetStage2, &_arm2Sense, NULL, fbcStallDetect, -15, 15, 50, 15);
	fbcInit(&driveLFBC, &driveLSet, &_driveLSense, NULL, fbcStallDetect, -15, 15, 50, 15);
	fbcInit(&driveRFBC, &driveRSet, &_driveRSense, NULL, fbcStallDetect, -15, 15, 50, 15);
	fbcPIDInitializeData(&arm1PID, 0.15, 0, 80, 0, 0);
	fbcPIDInitializeData(&arm2PID, 0.6, 0, 40, 0, 0);
	fbcPIDInitializeData(&driveLPID, 0.7, 0, 0, 0, 0);
	fbcPIDInitializeData(&driveRPID, 0.7, 0, 0, 0, 0);
	fbcPIDInit(&arm1FBC, &arm1PID);
	fbcPIDInit(&arm2FBC, &arm2PID);
	fbcPIDInit(&driveLFBC, &driveLPID);
	fbcPIDInit(&driveRFBC, &driveRPID);
}


