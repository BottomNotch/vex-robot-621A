#include "main.h"
#define ARM_2_START 230

TaskHandle autoStackTaskHandle = NULL;
TaskHandle armReturnTaskHandle = NULL;
char autoStackCone = 1;
int armPositions[13][2] = {
//arm1 | arm2
	{275,  -44},  //cone 1
	{275,  -84},  //cone 2
	{950,   30},  //cone 3
	{1160,  30},  //cone 4
	{975,   30},  //cone 5
	{1150,  30},  //cone 6
	{1150,  30},  //cone 7
	{1460, -52},  //cone 8
	{1475, -85},  //come 9
	{1600, -86},  //cone 10
	{1815, -70},  //cone 11
	{2363, -430}, //cone 12 (step one)
	{2055, -430}, //cone 12 (step two)
};

void _autoStackTask(void *ignore) {
	fbcSetGoal(&arm1FBC, armPositions[autoStackCone-1][0]);
	print("[autoStack] waiting\n\r");
	while((int)getSensor(arm1Pot) < arm1FBC.goal - ARM_2_START) {
		printf("%d\n\r", (int)getSensor(arm1Pot));
		delay(20);
	}
	print("[autoStack] setting arm2\n\r");
	fbcSetGoal(&arm2FBC, armPositions[autoStackCone-1][1]);

	if(autoStackCone == 12) {
		fbcSetGoal(&arm1FBC, armPositions[12][0]);
		while((int)getSensor(arm1Pot) < arm1FBC.goal - ARM_2_START) {
			delay(20);
		}
		fbcSetGoal(&arm2FBC, armPositions[12][1]);
	}
	autoStackCone++;
}

void _autoStackReturnTask(void *ignore) {
	if(autoStackTaskHandle != NULL && taskGetState(autoStackTaskHandle) !=
		 TASK_RUNNING) {
		fbcSetGoal(&arm2FBC, ARM_2_BOTTOM);
		fbcSetGoal(&arm1FBC, ARM_1_BOTTOM);
	}
}

void autoStack() {
	if(autoStackTaskHandle == NULL ||
		 taskGetState(autoStackTaskHandle) != TASK_RUNNING) {
		autoStackTaskHandle = taskCreate(&_autoStackTask, TASK_DEFAULT_STACK_SIZE,
																		 NULL,
																		 TASK_PRIORITY_DEFAULT);
		printf("[autoStack] starting task for cone %d\n\r", autoStackCone);
	}

	else {
		print("[autoStack] couldn't start task\n\r");
	}
}

void returnArm() {
	if(armReturnTaskHandle == NULL ||
		 taskGetState(armReturnTaskHandle) != TASK_RUNNING) {
		armReturnTaskHandle = taskCreate(&_autoStackReturnTask,
																		 TASK_DEFAULT_STACK_SIZE, NULL,
																		 TASK_PRIORITY_DEFAULT);
	}
}

void cancelStack() {
	print("[autoStack] stopping task\n\r");
	if(autoStackTaskHandle != NULL &&
		 taskGetState(autoStackTaskHandle) == TASK_RUNNING) {
		taskDelete(autoStackTaskHandle);
	}

	if(armReturnTaskHandle != NULL &&
		 taskGetState(armReturnTaskHandle) == TASK_RUNNING) {
		taskDelete(armReturnTaskHandle);
	}
	fbcSetGoal(&arm1FBC, (int)getSensor(arm1Pot));
	fbcSetGoal(&arm2FBC, (int)getSensor(arm2Enc));
}
