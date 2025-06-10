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
bool thread1 = true;
bool thread2 = true;
void test_start(void)
{
  osKernelInitialize();                 // Initialize CMSIS-RTOS

  osThreadNew(Thread_manager, NULL, NULL); // Create the manager thread

  osKernelStart();

}

void Thread_manager(void *argument)
{
  (void)argument;

  osThreadNew(Thread1, NULL, NULL);     // Create Thread1
  osThreadNew(Thread2, NULL, NULL);     // Create Thread2
  usleep(100 * 1000);  			// Sleep for 100 ms
  CP_ASSERT_EQ(thread1, true);
  CP_ASSERT_EQ(thread2, true);
  usleep(200 * 1000);  			// Sleep for 200 ms
  CP_ASSERT_EQ(thread1, false);
  usleep(200 * 1000);  			// Sleep for 200 ms
  CP_ASSERT_EQ(thread2, false);
}

void Thread1(void *argument)
{
  (void)argument;

  // Specify 200ms in terms of ticks
  int64_t ticks = (200 * (int64_t)1000000) / CP_CONFIG_NANO_IN_TICK;
  CP_ASSERT_EQ(osDelay(ticks), osOK);
  printf("[Thread1] Delayed for 200ms\n");

  printf("[Thread1] Done\n");
  thread1 = false;
}

void Thread2(void *argument)
{
  (void)argument;

  // Specify 400ms in terms of ticks
  int64_t ticks = (400 * (int64_t)1000000) / CP_CONFIG_NANO_IN_TICK;
  CP_ASSERT_EQ(osDelay(ticks), osOK);
  printf("[Thread2] Delayed for 400ms\n");

  printf("[Thread2] Done\n");
  thread2 = false;
}