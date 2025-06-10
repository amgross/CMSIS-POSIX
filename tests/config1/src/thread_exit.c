#include "cmsis_os2.h"
#include <stdio.h>
#include <stdbool.h>
#include "cmsisPosix_TestInfra.h"

void Thread1(void *argument);

void test_start(void)
{
  osKernelInitialize();

  osThreadId_t thread_id = osThreadNew(Thread1, NULL, NULL);
  CP_ASSERT_NE(thread_id, NULL);

  osKernelStart();

  CP_UASSERT_NREACHABLE();
}

void Thread1(void *argument)
{
  (void)argument;

  osThreadExit();

  CP_UASSERT_NREACHABLE();
}