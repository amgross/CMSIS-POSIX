#include "cmsis_os2.h"
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include "cmsisPosix_Config.h"
#include "cmsisPosix_TestInfra.h"

// Thread function prototypes
void Thread_manager(void *argument);
void Thread1(void *argument);

void test_start(void)
{
  osKernelInitialize();                 // Initialize CMSIS-RTOS

  osThreadId_t thread_id = osThreadNew(Thread1, NULL, NULL);     // Create Thread1
  CP_ASSERT_NE(NULL, thread_id);

  osKernelStart();                     // Start scheduler

  CP_ASSERT_UNREACHABLE();
}

void Thread1(void *argument)
{
  (void)argument;

  CP_ASSERT_EQ(osKernelGetTickFreq(), (int64_t)1000000000 / CP_CONFIG_NANO_IN_TICK);

  // Delay for 100 ticks and ensure the tick count has increased accordingly
  uint32_t start = osKernelGetTickCount();
  osDelay(100);
  uint32_t delta = osKernelGetTickCount() - start;
  CP_ASSERT_TRUE(delta >= 100);
  CP_ASSERT_TRUE(delta < 200);
  TEST_LOG_INFO("[Thread1] Delayed for 100ms\n");

  // Specify 200ms in terms of ticks
  int64_t ticks = (200 * (int64_t)1000000) / CP_CONFIG_NANO_IN_TICK;
  CP_ASSERT_EQ(osDelay(ticks), osOK);

  TEST_LOG_INFO("[Thread1] Done\n");
}