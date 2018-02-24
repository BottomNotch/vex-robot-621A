#include "main.h"

void _debugTask(void *ignore) {
  while(true) {
    printf("drive: %d, %d\n\r", driveLSense(), driveRSense());
    delay(500);
  }
}

void startDebug() {
  taskCreate(&_debugTask, TASK_DEFAULT_STACK_SIZE, NULL, TASK_PRIORITY_DEFAULT);
}
