#ifndef ROBOT_H_
#define ROBOT_H_
#include "main.h"

#define CLAW_OPEN true
#define CLAW_CLOSE false

#define ARM_1_BOTTOM 260
#define ARM_1_TOP 2300
#define ARM_2_BOTTOM -1160
#define ARM_1_LOWER_P 1600

#define MOGO_OFFSET 20
#define MOGO_INTAKE_POSITION 3520
#define MOGO_STACK_POSITION 300

const char claw;
const char mogoL;
const char mogoR;
const char Rdrive2;
const char RDrive1;
const char LDrive1;
const char LDrive2;
const char arm1;
const char arm2;


simpleSensor powerExpander;
simpleSensor autoSelect;
simpleSensor arm1Pot;
simpleSensor mogoPotL;
simpleSensor mogoPotR;
simpleSensor arm2Enc;
simpleSensor driveEncL;
simpleSensor driveEncR;

fbc_t arm1FBC;
fbc_t arm2FBC;
fbc_t mogoLFBC;
fbc_t mogoRFBC;
fbc_pid_t arm1PID;
fbc_pid_t arm2PID;
fbc_pid_t mogoLPID;
fbc_pid_t mogoRPID;

void encodersInit();
void motorsInit();
void driveSet(int left, int right);
void armSetStage1(int power);
void armSetStage2(int power);
void armSetBothStages(int stage1, int stage2);
void mogoSet(int power);
void clawMove();
void initFBCControllers();
#endif
