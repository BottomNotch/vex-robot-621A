#ifndef ROBOT_H_
#define ROBOT_H_
#include "main.h"

#define CLAW_OPEN true
#define CLAW_CLOSE false
#define ARM_1_BOTTOM 350
#define ARM_1_TOP 2500
#define ARM_2_BOTTOM -1160
#define ARM_1_HIGH 0 //value TBD
#define ARM_2_HIGH 0 //value TBD
#define ARM_1_LOWER_P 1600

const char intake;
const char wrist;
const char RDriveB;
const char RDriveM;
const char RDriveF;
const char LDriveF;
const char LDriveM;
const char LDriveB ;
const char arm1;
const char arm2;

bool arm1Stalled;

simpleSensor powerExpander;
simpleSensor autoSelect;
simpleSensor arm1Pot;
simpleSensor arm2Enc;
simpleSensor driveEncL;
simpleSensor driveEncR;
simpleSensor mogoSwitch;

fbc_t arm1FBC;
fbc_t arm2FBC;
fbc_t driveLFBC;
fbc_t driveRFBC;
fbc_pid_t arm1PID;
fbc_pid_t arm2PID;
fbc_pid_t driveLPID;
fbc_pid_t driveRPID;

void encodersInit();
void motorsInit();
void driveAndMogoTask();
void driveSet(int left, int right);
void armSetStage1(int power);
void armSetStage2(int power);
void armSetBothStages(int stage1, int stage2);
void mogoSet(int power);
void clawMove();
void initFBCControllers();
#endif
