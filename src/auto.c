/** @file auto.c
 * @brief File for autonomous code
 *
 * This file should contain the user autonomous() function and any functions related to it.
 *
 * Any copyright is dedicated to the Public Domain.
 * http://creativecommons.org/publicdomain/zero/1.0/
 *
 * PROS contains FreeRTOS (http://www.freertos.org) whose source code may be
 * obtained from http://sourceforge.net/projects/freertos/files/ or on request.
 */

#include "main.h"

/*
 * Runs the user autonomous code. This function will be started in its own task with the default
 * priority and stack size whenever the robot is enabled via the Field Management System or the
 * VEX Competition Switch in the autonomous mode. If the robot is disabled or communications is
 * lost, the autonomous task will be stopped by the kernel. Re-enabling the robot will restart
 * the task, not re-start it from where it left off.
 *
 * Code running in the autonomous task cannot access information from the VEX Joystick. However,
 * the autonomous function can be invoked from another task if a VEX Competition Switch is not
 * available, and it can access joystick information if called in this way.
 *
 * The autonomous task may exit, unlike operatorControl() which should never exit. If it does
 * so, the robot will await a switch to another mode or disable/enable cycle.
 */
void drive(int left, int right) {
	fbcSetGoal(&driveLFBC, (int)getSensor(driveEncL) + left);
	fbcSetGoal(&driveRFBC, (int)getSensor(driveEncR) + right);
}

void mogo(bool intake) {
	if(intake) {
		mogoSet(-84);
		for(int i = 0; i < 1000; i+=20) {
			if(getSensor(mogoSwitch)) {
				break;
			}
			delay(20);
		}
		mogoSet(0);
	}

	else {
		mogoSet(84);
		delay(700);
		mogoSet(20);
	}

}

void mogo10ptZone() {
	intakeMove(INTAKE);
	delay(250);
	drive(1200, 1200);
	mogo(false);
	delay(2000);
	fbcSetGoal(&arm1FBC, 650);
	mogo(true);
	drive(800, -800);
	delay(1500);
	drive(1700, 1700);
	delay(2000);
	fbcSetGoal(&arm1FBC, 1600);
	delay(500);
	fbcSetGoal(&arm2FBC, 209);
	delay(1000);
	fbcSetGoal(&arm1FBC, 650);
	intakeMove(OUTTAKE);
	delay(500);
	mogo(false);
	drive(-400, -400);
}

void autonomous() {
	mogo10ptZone();
}


