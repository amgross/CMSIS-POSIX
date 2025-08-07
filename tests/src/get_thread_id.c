#include "cmsis_os2.h"
#include <stdio.h>
#include <stdbool.h>
#include "cmsisPosix_TestInfra.h"

void Thread1(void *argument);

osThreadId_t orig_thread_id;

void test_start(void)
{
  osKernelInitialize();

  orig_thread_id = osThreadNew(Thread1, NULL, NULL);
  CP_ASSERT_NE(orig_thread_id, NULL);

  osKernelStart();

  CP_ASSERT_UNREACHABLE();
}

void Thread1(void *argument)
{
  (void)argument;

  osThreadId_t curr_thread_id;

  curr_thread_id = osThreadGetId();
  CP_ASSERT_EQ(curr_thread_id, orig_thread_id);
}