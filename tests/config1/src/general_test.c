#include "cmsis_os2.h"
#include <stdio.h>
#include <stdbool.h>
#include "cmsisPosix_TestInfra.h"

// Mutex ID
static osMutexId_t myMutex;

// Thread function prototypes
void Thread1(void *argument);
void Thread2(void *argument);

void test_start(void)
{
  osKernelInitialize();                 // Initialize CMSIS-RTOS
  myMutex = osMutexNew(NULL);          // Create mutex with default attributes

  osThreadNew(Thread1, NULL, NULL);    // Create Thread1
  osThreadNew(Thread2, NULL, NULL);    // Create Thread2

  osKernelStart();                     // Start scheduler
}

void Thread1(void *argument)
{
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
}
