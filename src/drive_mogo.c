#include "main.h"

const unsigned char diffLF = 2;
const unsigned char diffLB = 3;
const unsigned char driveL = 4;
const unsigned char driveR = 7;
const unsigned char diffRB = 8;
const unsigned char diffRF = 9;

fbc_t driveLFBC;
fbc_t driveRFBC;
fbc_pid_t driveLPID;
fbc_pid_t driveRPID;

int _LDrivePower = 0;
int _RDrivePower = 0;
int _mogoPower = 0;

Encoder encL;
Encoder encR;

void _encodersInit() {
  encL = encoderInit(1, 2, false);
  encR = encoderInit(3, 4, true);
}

void _motorsInit() {
	blrsMotorInit(diffLF, false, DEFAULT_SLEW_RATE, NULL);
	blrsMotorInit(diffLB, false, DEFAULT_SLEW_RATE, NULL);
	blrsMotorInit(driveL, true, DEFAULT_SLEW_RATE, NULL);
	blrsMotorInit(driveR, false, DEFAULT_SLEW_RATE, NULL);
	blrsMotorInit(diffRB, true, DEFAULT_SLEW_RATE, NULL);
	blrsMotorInit(diffRF, true, DEFAULT_SLEW_RATE, NULL);
}


void driveLSet(int power) {
	_LDrivePower = power;
}

void driveRSet(int power) {
	_RDrivePower = power;
}

void mogoSet(int power) {
	_mogoPower = power;
}

int driveLSense() {
	return encoderGet(encL);
}

int driveRSense() {
	return encoderGet(encR);
}

void driveLReset() {
  encoderReset(encL);
}

void driveRReset() {
  encoderReset(encR);
}

void driveSet(int left, int right, bool blocking) {
  driveLReset();
  driveRReset();
  fbcSetGoal(&driveLFBC, left);
  fbcSetGoal(&driveLFBC, right);

  if(blocking) {
    //stub
  }
}

void _driveAndMogoTask(void *ignore) {
	while(true) {
    //the drive and mogo lift share motors, which makes controlling those mechanisms
    //more complex, this handles the math
		blrsMotorSet(diffLF, -_LDrivePower + _mogoPower, false);
		blrsMotorSet(diffLB,  _LDrivePower + _mogoPower, false);
		blrsMotorSet(driveL, _LDrivePower, false);
		blrsMotorSet(driveR, _RDrivePower, false);
		blrsMotorSet(diffRB, _RDrivePower + _mogoPower, false);
		blrsMotorSet(diffRF, -_RDrivePower + _mogoPower, false);

    fbcRunContinuous(&driveLFBC);
    fbcRunContinuous(&driveRFBC);
		delay(20);
	}
}

void driveAndMogoInit() {
	fbcInit(&driveLFBC, &driveLSet, &driveLSense, NULL, fbcStallDetect, -15, 15, 50, 15);
	fbcInit(&driveRFBC, &driveRSet, &driveRSense, NULL, fbcStallDetect, -15, 15, 50, 15);
	fbcPIDInitializeData(&driveLPID, 0.7, 0, 0, 0, 0);
	fbcPIDInitializeData(&driveRPID, 0.7, 0, 0, 0, 0);
	fbcPIDInit(&driveLFBC, &driveLPID);
	fbcPIDInit(&driveRFBC, &driveRPID);
	taskCreate(&_driveAndMogoTask, TASK_DEFAULT_STACK_SIZE, NULL,
						 TASK_PRIORITY_DEFAULT);
  _motorsInit();
  _encodersInit();
}


