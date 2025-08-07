#include "cmsis_os2.h"
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include "cmsisPosix_Config.h"
#include "cmsisPosix_TestInfra.h"

// Thread function prototypes
void Thread_manager(void *argument);
void Thread1(void *argument);
void Thread2(void *argument);
void Thread3(void *argument);
bool thread1 = true;
bool thread2 = true;
bool thread3 = true;
void test_start(void)
{
  osThreadId_t thread_id;

  osKernelInitialize();                 // Initialize CMSIS-RTOS

  thread_id = osThreadNew(Thread_manager, NULL, NULL); // Create the manager thread
  CP_ASSERT_NE(NULL, thread_id);

  osKernelStart();

  CP_ASSERT_UNREACHABLE();
}

void Thread_manager(void *argument)
{
  (void)argument;

  osThreadId_t thread_id;

  thread_id = osThreadNew(Thread1, NULL, NULL);     // Create Thread1
  CP_ASSERT_NE(NULL, thread_id);

  thread_id = osThreadNew(Thread2, NULL, NULL);     // Create Thread2
  CP_ASSERT_NE(NULL, thread_id);

  thread_id = osThreadNew(Thread3, NULL, NULL);     // Create Thread3
  CP_ASSERT_NE(NULL, thread_id);

  usleep(100 * 1000);  			// Sleep for 100 ms
  CP_ASSERT_EQ(thread1, true);
  CP_ASSERT_EQ(thread2, true);
  CP_ASSERT_EQ(thread3, true);
  usleep(200 * 1000);  			// Sleep for 200 ms
  CP_ASSERT_EQ(thread1, false);
  usleep(200 * 1000);  			// Sleep for 200 ms
  CP_ASSERT_EQ(thread2, false);
  CP_ASSERT_EQ(thread3, false);
}

void Thread1(void *argument)
{
  (void)argument;

  // Specify 200ms in terms of ticks
  int64_t ticks = osKernelGetTickFreq() * 0.2;
  CP_ASSERT_EQ(osDelay(ticks), osOK);
  TEST_LOG_INFO("[Thread1] Delayed for 200ms\n");

  TEST_LOG_INFO("[Thread1] Done\n");
  thread1 = false;
}

void Thread2(void *argument)
{
  (void)argument;

  // Specify 400ms in terms of ticks
  int64_t ticks = osKernelGetTickFreq() * 0.4;
  CP_ASSERT_EQ(osDelay(ticks), osOK);
  TEST_LOG_INFO("[Thread2] Delayed for 400ms\n");

  TEST_LOG_INFO("[Thread2] Done\n");
  thread2 = false;
}

void Thread3(void *argument)
{
  (void)argument;

  // Specify 400ms in terms of ticks
  int64_t ticks = osKernelGetTickFreq() * 0.4;
  uint32_t start = osKernelGetTickCount();
  uint32_t end = start + ticks;
  CP_ASSERT_EQ(osDelayUntil(end), osOK);
  uint32_t delta = osKernelGetTickCount() - start;
  CP_ASSERT_TRUE(delta >= ticks);
  CP_ASSERT_TRUE(delta < ticks + (ticks / 4));
  TEST_LOG_INFO("[Thread3] Delayed for 400ms\n");

  TEST_LOG_INFO("[Thread3] Done\n");
  thread3 = false;
}