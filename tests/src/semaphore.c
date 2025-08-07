#include "cmsis_os2.h"
#include <stdio.h>
#include <stdbool.h>
#include "cmsisPosix_TestInfra.h"

// Memory Pool ID
static osSemaphoreId_t mySemaphore;

// Thread function prototypes
void Thread_manager(void *argument);
void Thread1(void *argument);
void Thread2(void *argument);
bool thread1 = true;
bool thread2 = true;
void test_start(void)
{
  osThreadId_t thread_id;
  osKernelInitialize();                 // Initialize CMSIS-RTOS

  thread_id = osThreadNew(Thread_manager, NULL, NULL);
  CP_ASSERT_NE(thread_id, NULL);

  osKernelStart();                     // Start scheduler

  CP_ASSERT_UNREACHABLE();
}

void Thread_manager(void *argument)
{
  (void)argument;
  osThreadId_t thread_id;
  osStatus_t status;

  mySemaphore = osSemaphoreNew(2, 0, NULL);   // Create semaphore with initially zero tokens
  CP_ASSERT_NE(mySemaphore, NULL);
  CP_ASSERT_EQ(osSemaphoreGetCount(mySemaphore), 0);
  CP_ASSERT_EQ(osSemaphoreRelease(mySemaphore), osOK);
  CP_ASSERT_EQ(osSemaphoreGetCount(mySemaphore), 1);
  CP_ASSERT_EQ(osSemaphoreRelease(mySemaphore), osOK);
  CP_ASSERT_EQ(osSemaphoreGetCount(mySemaphore), 2);
  CP_ASSERT_EQ(osSemaphoreRelease(mySemaphore), osErrorResource);
  CP_ASSERT_EQ(osSemaphoreGetCount(mySemaphore), 2);
  CP_ASSERT_EQ(osSemaphoreDelete(mySemaphore), osOK);

  mySemaphore = osSemaphoreNew(2, 2, NULL);   // Create semaphore with default attributes
  CP_ASSERT_NE(mySemaphore, NULL);
  CP_ASSERT_EQ(osSemaphoreGetCount(mySemaphore), 2);

  // Check data validation
  CP_ASSERT_EQ(osSemaphoreNew(0, 2, NULL), NULL);
  CP_ASSERT_EQ(osSemaphoreAcquire(NULL, 0), osErrorParameter);
  CP_ASSERT_EQ(osSemaphoreAcquire(NULL, 1), osErrorParameter);
  CP_ASSERT_EQ(osSemaphoreAcquire(NULL, osWaitForever), osErrorParameter);
  CP_ASSERT_EQ(osSemaphoreRelease(NULL), osErrorParameter);
  CP_ASSERT_EQ(osSemaphoreGetCount(NULL), 0);
  CP_ASSERT_EQ(osSemaphoreDelete(NULL), osErrorParameter);

  thread_id = osThreadNew(Thread1, NULL, NULL);     // Create Thread1
  CP_ASSERT_NE(thread_id, NULL);
  thread_id = osThreadNew(Thread2, NULL, NULL);     // Create Thread2
  CP_ASSERT_NE(thread_id, NULL);

  status = osThreadSetPriority(osThreadGetId(), osPriorityBelowNormal);
  CP_ASSERT_EQ(status, osOK);

  // Wait for the threads to terminate
  while (thread1 || thread2) {}

  CP_ASSERT_EQ(osSemaphoreDelete(mySemaphore), osOK);
}

void Thread1(void *argument)
{
  (void)argument;

  CP_ASSERT_EQ(osSemaphoreAcquire(mySemaphore, osWaitForever), osOK);
  TEST_LOG_INFO("[Thread1] Acquired 1st token\n");

  osDelay(50);  // Delay for 50 ticks
  CP_ASSERT_EQ(osSemaphoreGetCount(mySemaphore), 0);
  CP_ASSERT_EQ(osSemaphoreAcquire(mySemaphore, 0), osErrorResource);
  TEST_LOG_INFO("[Thread1] Timed-out acquiring 2nd token\n");

  CP_ASSERT_EQ(osSemaphoreAcquire(mySemaphore, 1000), osOK);
  TEST_LOG_INFO("[Thread1] Acquired 2nd token\n");

  osDelay(100);  // Delay for 100 ticks
  CP_ASSERT_EQ(osSemaphoreRelease(mySemaphore), osOK);
  CP_ASSERT_EQ(osSemaphoreRelease(mySemaphore), osOK);
  TEST_LOG_INFO("[Thread1] Released both token\n");

  TEST_LOG_INFO("[Thread1] Done\n");
  thread1 = false;
}

void Thread2(void *argument)
{
  (void)argument;

  CP_ASSERT_EQ(osSemaphoreAcquire(mySemaphore, osWaitForever), osOK);
  TEST_LOG_INFO("[Thread2] Acquired 1st token\n");

  osDelay(100);  // Delay for 100 ticks
  CP_ASSERT_EQ(osSemaphoreRelease(mySemaphore), osOK);
  TEST_LOG_INFO("[Thread2] Released 1st token\n");

  osDelay(50);  // Delay for 50 ticks
  CP_ASSERT_EQ(osSemaphoreGetCount(mySemaphore), 0);
  CP_ASSERT_EQ(osSemaphoreAcquire(mySemaphore, 0), osErrorResource);
  TEST_LOG_INFO("[Thread2] Timed-out acquiring 2nd token\n");

  CP_ASSERT_EQ(osSemaphoreAcquire(mySemaphore, osWaitForever), osOK);
  TEST_LOG_INFO("[Thread2] Acquired 2nd token\n");

  CP_ASSERT_EQ(osSemaphoreRelease(mySemaphore), osOK);
  TEST_LOG_INFO("[Thread2] Released 2nd token\n");

  TEST_LOG_INFO("[Thread2] Done\n");
  thread2 = false;
}