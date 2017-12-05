/** @file opcontrol.c
 * @brief File for operator control code
 *
 * This file should contain the user operatorControl() function and any functions related to it.
 *
 * Any copyright is dedicated to the Public Domain.
 * http://creativecommons.org/publicdomain/zero/1.0/
 *
 * PROS contains FreeRTOS (http://www.freertos.org) whose source code may be
 * obtained from http://sourceforge.net/projects/freertos/files/ or on request.
 */

#include "main.h"

/*
 * Runs the user operator control code. This function will be started in its own task with the
 * default priority and stack size whenever the robot is enabled via the Field Management System
 * or the VEX Competition Switch in the operator control mode. If the robot is disabled or
 * communications is lost, the operator control task will be stopped by the kernel. Re-enabling
 * the robot will restart the task, not resume it from where it left off.
 *
 * If no VEX Competition Switch or Field Management system is plugged in, the VEX Cortex will
 * run the operator control task. Be warned that this will also occur if the VEX Cortex is
 * tethered directly to a computer via the USB A to A cable without any VEX Joystick attached.
 *
 * Code running in this task can take almost any action, as the VEX Joystick is available and
 * the scheduler is operational. However, proper use of delay() or taskDelayUntil() is highly
 * recommended to give other tasks (including system tasks such as updating LCDs) time to run.
 *
 * This task should never exit; it should end with some kind of infinite loop, even if empty.
 */

void operatorControl() {
	bool autoStacking = false;
	bool stopDrive = true;

	while (1) {
		if(autoStacking) {

			if(buttonIsNewPress(JOY1_6U)) {
					autoStack();
			}

			else if(buttonIsNewPress(JOY1_6D)) {
				if(autoStackTaskHandle != NULL &&
					 taskGetState(autoStackTaskHandle) == TASK_RUNNING) {
					cancelStack();
				}

				else {
					returnArm();
				}
			}

			if(buttonIsNewPress(JOY1_7L) && autoStackCone < 12) {
				autoStackCone++;
			}

			else if(buttonIsNewPress(JOY1_7D) && autoStackCone > 1) {
				autoStackCone--;
			}

			if(buttonIsNewPress(JOY1_7U)) {
				autoStacking = false;
			}
		}

		else {
			if(buttonGetState(JOY1_5U)) {
				fbcSetGoal(&arm1FBC, (int)getSensor(arm1Pot) + 750);
			}

			else if(buttonGetState(JOY1_5D)) {
				fbcSetGoal(&arm1FBC, (int)getSensor(arm1Pot) - 750);
			}
			if(arm1FBC.goal > ARM_1_TOP) {
				fbcSetGoal(&arm1FBC, ARM_1_TOP);
			}

			else if(arm1FBC.goal < ARM_1_BOTTOM) {
				fbcSetGoal(&arm1FBC, ARM_1_BOTTOM);
			}

			if(buttonGetState(JOY1_6U)) {
				fbcSetGoal(&arm2FBC, (int)getSensor(arm2Enc) + 200);
			}

			else if(buttonGetState(JOY1_6D)) {
				fbcSetGoal(&arm2FBC, (int)getSensor(arm2Enc) - 200);
			}

			if(arm2FBC.goal < ARM_2_BOTTOM) {
				fbcSetGoal(&arm2FBC, ARM_2_BOTTOM);
			}

			if(buttonIsNewPress(JOY1_7U)) {
				autoStacking = true;
			}
		}

		if(buttonIsNewPress(JOY1_8U)) {
			clawMove();
		}

		mogoSet(joystickGetAnalog(1, 2));
		if(abs(joystickGetAnalog(1, 3)) > 15 || abs(joystickGetAnalog(1, 4)) > 15) {
			fbcSetGoal(&driveLFBC, (int)getSensor(driveEncL) +
								 joystickGetAnalog(1, 3) + joystickGetAnalog(1,4));

			fbcSetGoal(&driveRFBC, (int)getSensor(driveEncR) +
								 joystickGetAnalog(1, 3) - joystickGetAnalog(1,4));
			stopDrive = true;
		}

		else if(stopDrive) {
			stopDrive = false;
			fbcSetGoal(&driveLFBC, (int)getSensor(driveEncL));
			fbcSetGoal(&driveRFBC, (int)getSensor(driveEncR));
		}

		delay(20);
	}
}
