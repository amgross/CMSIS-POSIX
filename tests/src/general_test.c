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
  osThreadId_t thread_id;

  osKernelInitialize();                 // Initialize CMSIS-RTOS
  myMutex = osMutexNew(NULL);          // Create mutex with default attributes

  thread_id = osThreadNew(Thread1, NULL, NULL);    // Create Thread1
  CP_ASSERT_NE(NULL, thread_id);

  thread_id = osThreadNew(Thread2, NULL, NULL);    // Create Thread2
  CP_ASSERT_NE(NULL, thread_id);

  osKernelStart();                     // Start scheduler

  CP_ASSERT_UNREACHABLE();
}

void Thread1(void *argument)
{
  (void)argument;
  for (int i = 0; i < 5; ++i)
  {
    if (osMutexAcquire(myMutex, osWaitForever) == osOK)
    {
      TEST_LOG_INFO("[Thread1] Acquired mutex\n");
      TEST_LOG_INFO("[Thread1] Doing work\n");
      TEST_LOG_INFO("[Thread1] Releasing mutex\n");
      osMutexRelease(myMutex);
    }
  }
  TEST_LOG_INFO("[Thread1] Done\n");
}

void Thread2(void *argument)
{
  (void)argument;
  for (int i = 0; i < 5; ++i)
  {
    if (osMutexAcquire(myMutex, osWaitForever) == osOK)
    {
      TEST_LOG_INFO("[Thread2] Acquired mutex\n");
      TEST_LOG_INFO("[Thread2] Doing work\n");
      TEST_LOG_INFO("[Thread2] Releasing mutex\n");
      osMutexRelease(myMutex);
    }
  }
  TEST_LOG_INFO("[Thread2] Done\n");
}
