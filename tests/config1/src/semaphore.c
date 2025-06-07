#include "cmsis_os2.h"
#include <stdio.h>
#include <stdbool.h>
#include "cmsisPosix_TestInfra.h"

// Memory Pool ID
static osSemaphoreId_t mySemaphore;

// Thread function prototypes
void Thread1(void *argument);
void Thread2(void *argument);
bool thread1 = true;
bool thread2 = true;
void test_start(void) {
  printf("CMSIS-RTOS2 semaphore test start\n");

  osKernelInitialize();                 // Initialize CMSIS-RTOS

  mySemaphore = osSemaphoreNew(2, 0, NULL);   // Create semaphore with initially zero tokens
  CP_ASSERT_TRUE(osSemaphoreGetCount(mySemaphore) == 0);
  osSemaphoreDelete(mySemaphore);
  mySemaphore = osSemaphoreNew(2, 2, NULL);   // Create semaphore with default attributes
  CP_ASSERT_TRUE(osSemaphoreGetCount(mySemaphore) == 2);

  // Check data validation
  CP_ASSERT_TRUE(osSemaphoreNew(0, 2, NULL) == NULL);
  CP_ASSERT_TRUE(osSemaphoreAcquire(NULL, 0) == osErrorParameter);
  CP_ASSERT_TRUE(osSemaphoreAcquire(NULL, 1) == osErrorParameter);
  CP_ASSERT_TRUE(osSemaphoreAcquire(NULL, osWaitForever) == osErrorParameter);
  CP_ASSERT_TRUE(osSemaphoreRelease(NULL) == osErrorParameter);
  CP_ASSERT_TRUE(osSemaphoreGetCount(NULL) == 0);
  CP_ASSERT_TRUE(osSemaphoreDelete(NULL) == osErrorParameter);

  osThreadNew(Thread1, NULL, NULL);     // Create Thread1
  osThreadNew(Thread2, NULL, NULL);     // Create Thread2

  // Wait for the threads to terminate
  while (thread1 || thread2) {}

  osSemaphoreDelete(mySemaphore);
}

void Thread1(void *argument) {
  (void)argument;

  CP_ASSERT_TRUE(osSemaphoreAcquire(mySemaphore, osWaitForever) == osOK);
  printf("[Thread1] Acquired 1st token\n");

  osDelay(50);  // Delay for 50 ticks
  CP_ASSERT_TRUE(osSemaphoreGetCount(mySemaphore) == 0);
  CP_ASSERT_TRUE(osSemaphoreAcquire(mySemaphore, 0) == osErrorResource);
  printf("[Thread1] Timed-out acquiring 2nd token\n");

  CP_ASSERT_TRUE(osSemaphoreAcquire(mySemaphore, 1000) == osOK);
  printf("[Thread1] Acquired 2nd token\n");

  osDelay(100);  // Delay for 100 ticks
  CP_ASSERT_TRUE(osSemaphoreRelease(mySemaphore) == osOK);
  CP_ASSERT_TRUE(osSemaphoreRelease(mySemaphore) == osOK);
  printf("[Thread1] Released both token\n");

  printf("[Thread1] Done\n");
  thread1 = false;
}

void Thread2(void *argument) {
  (void)argument;

  CP_ASSERT_TRUE(osSemaphoreAcquire(mySemaphore, osWaitForever) == osOK);
  printf("[Thread2] Acquired 1st token\n");

  osDelay(100);  // Delay for 100 ticks
  CP_ASSERT_TRUE(osSemaphoreRelease(mySemaphore) == osOK);
  printf("[Thread2] Released 1st token\n");

  osDelay(50);  // Delay for 50 ticks
  CP_ASSERT_TRUE(osSemaphoreGetCount(mySemaphore) == 0);
  CP_ASSERT_TRUE(osSemaphoreAcquire(mySemaphore, 0) == osErrorResource);
  printf("[Thread2] Timed-out acquiring 2nd token\n");

  CP_ASSERT_TRUE(osSemaphoreAcquire(mySemaphore, osWaitForever) == osOK);
  printf("[Thread2] Acquired 2nd token\n");

  CP_ASSERT_TRUE(osSemaphoreRelease(mySemaphore) == osOK);
  printf("[Thread2] Released 2nd token\n");

  printf("[Thread2] Done\n");
  thread2 = false;
}