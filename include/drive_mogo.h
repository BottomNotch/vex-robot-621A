#ifndef DRIVE_MOGO_H_
#define DRIVE_MOGO_H_
#include "main.h"

fbc_t driveLFBC;
fbc_t driveRFBC;

fbc_pid_t driveLPID;
fbc_pid_t driveRPID;

void driveSet(int left, int right, bool blocking);
void mogoSet(int power);
int driveLSense();
int driveRSense();
void driveLReset();
void driveRReset();
void driveAndMogoInit();
#endif
