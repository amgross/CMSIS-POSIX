#include "cmsis_os2.h"
#include <stdio.h>
#include <stdbool.h>
#include "cmsisPosix_TestInfra.h"

// Mutex ID
static osMutexId_t myMutex;

// Thread function prototypes
void Thread1(void *argument);
void Thread2(void *argument);
bool thread1 = true;
bool thread2 = true;
void test_start(void) {
  printf("CMSIS-RTOS2 mutex test start\n");

  osKernelInitialize();                 // Initialize CMSIS-RTOS
  myMutex = osMutexNew(NULL);          // Create mutex with default attributes

  osThreadNew(Thread1, NULL, NULL);    // Create Thread1
  osThreadNew(Thread2, NULL, NULL);    // Create Thread2

//   osKernelStart();                     // Start scheduler

  // Should never reach here
  while (thread1 || thread2) {}
}

void Thread1(void *argument) {
  (void)argument;
  for (int i = 0; i < 5; ++i) {
    if (osMutexAcquire(myMutex, osWaitForever) == osOK) {
      printf("[Thread1] Acquired mutex\n");
      printf("[Thread1] Doing work\n");
      printf("[Thread1] Releasing mutex\n");
      osMutexRelease(myMutex);
    }
  }
  printf("[Thread1] Done\n");
  thread1 = false;
}

void Thread2(void *argument) {
  (void)argument;
  for (int i = 0; i < 5; ++i) {
    if (osMutexAcquire(myMutex, osWaitForever) == osOK) {
      printf("[Thread2] Acquired mutex\n");
      printf("[Thread2] Doing work\n");
      printf("[Thread2] Releasing mutex\n");
      osMutexRelease(myMutex);
    }
  }
  printf("[Thread2] Done\n");
  thread2 = false;
}
