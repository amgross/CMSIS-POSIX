#include "cmsis_os2.h"
#include <stdio.h>
#include <stdbool.h>
#include "cmsisPosix_TestInfra.h"

// Memory Pool ID
static osMemoryPoolId_t myMemoryPool;

// Thread function prototypes
void Thread1(void *argument);
void Thread2(void *argument);
bool thread1 = true;
bool thread2 = true;
void test_start(void) {
  printf("CMSIS-RTOS2 memory pool test start\n");

  osKernelInitialize();                 // Initialize CMSIS-RTOS

  myMemoryPool = osMemoryPoolNew(2, 128, NULL);   // Create memory pool with default attributes
  CP_ASSERT_TRUE(osMemoryPoolGetCapacity(myMemoryPool) == 2);
  CP_ASSERT_TRUE(osMemoryPoolGetBlockSize(myMemoryPool) == 128);
  CP_ASSERT_TRUE(osMemoryPoolGetCount(myMemoryPool) == 0);
  CP_ASSERT_TRUE(osMemoryPoolGetSpace(myMemoryPool) == 2);

  // Check data validation
  CP_ASSERT_TRUE(osMemoryPoolNew(0, 128, NULL) == NULL);
  CP_ASSERT_TRUE(osMemoryPoolNew(2, 0, NULL) == NULL);
  CP_ASSERT_TRUE(osMemoryPoolAlloc(NULL, 0) == NULL);
  CP_ASSERT_TRUE(osMemoryPoolAlloc(NULL, 1) == NULL);
  CP_ASSERT_TRUE(osMemoryPoolAlloc(NULL, osWaitForever) == NULL);
  CP_ASSERT_TRUE(osMemoryPoolFree(NULL, NULL) == osErrorParameter);
  CP_ASSERT_TRUE(osMemoryPoolFree(myMemoryPool, NULL) == osErrorParameter);
  CP_ASSERT_TRUE(osMemoryPoolGetCapacity(NULL) == 0);
  CP_ASSERT_TRUE(osMemoryPoolGetBlockSize(NULL) == 0);
  CP_ASSERT_TRUE(osMemoryPoolGetCount(NULL) == 0);
  CP_ASSERT_TRUE(osMemoryPoolGetSpace(NULL) == 0);
  CP_ASSERT_TRUE(osMemoryPoolDelete(NULL) == osErrorParameter);

  osThreadNew(Thread1, NULL, NULL);     // Create Thread1
  osThreadNew(Thread2, NULL, NULL);     // Create Thread2

  // Wait for the threads to terminate
  while (thread1 || thread2) {}

  osMemoryPoolDelete(myMemoryPool);
}

void Thread1(void *argument) {
  (void)argument;
  void *block1;
  void *block2;

  block1 = osMemoryPoolAlloc(myMemoryPool, osWaitForever);
  CP_ASSERT_TRUE(block1 != NULL);
  printf("[Thread1] Allocated 1st memory block\n");

  osDelay(50);  // Delay for 50 ticks
  CP_ASSERT_TRUE(osMemoryPoolGetCount(myMemoryPool) == 2);

  block2 = osMemoryPoolAlloc(myMemoryPool, 0);
  CP_ASSERT_TRUE(block2 == NULL);
  printf("[Thread1] Timed-out allocating 2nd memory block\n");

  block2 = osMemoryPoolAlloc(myMemoryPool, 1000);
  CP_ASSERT_TRUE(block2 != NULL);
  printf("[Thread1] Allocated 2nd memory block\n");

  osDelay(100);  // Delay for 100 ticks
  CP_ASSERT_TRUE(osMemoryPoolFree(myMemoryPool, block1) == osOK);
  CP_ASSERT_TRUE(osMemoryPoolFree(myMemoryPool, block2) == osOK);
  printf("[Thread1] Freed both memory block\n");

  printf("[Thread1] Done\n");
  thread1 = false;
}

void Thread2(void *argument) {
  (void)argument;
  void *block1;
  void *block2;

  block1 = osMemoryPoolAlloc(myMemoryPool, osWaitForever);
  CP_ASSERT_TRUE(block1 != NULL);
  printf("[Thread2] Allocated 1st memory block\n");

  osDelay(100);  // Delay for 100 ticks
  CP_ASSERT_TRUE(osMemoryPoolFree(myMemoryPool, block1) == osOK);
  printf("[Thread2] Freed 1st memory block\n");

  osDelay(50);  // Delay for 50 ticks
  CP_ASSERT_TRUE(osMemoryPoolGetCount(myMemoryPool) == 2);

  block2 = osMemoryPoolAlloc(myMemoryPool, 0);
  CP_ASSERT_TRUE(block2 == NULL);
  printf("[Thread2] Timed-out allocating 2nd memory block\n");

  block2 = osMemoryPoolAlloc(myMemoryPool, osWaitForever);
  CP_ASSERT_TRUE(block2 != NULL);
  printf("[Thread2] Allocated 2nd memory block\n");

  CP_ASSERT_TRUE(osMemoryPoolFree(myMemoryPool, block2) == osOK);
  printf("[Thread2] Freed 2nd memory block\n");

  printf("[Thread2] Done\n");
  thread2 = false;
}