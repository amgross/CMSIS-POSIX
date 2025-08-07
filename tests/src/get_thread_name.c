#include "cmsis_os2.h"
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include "cmsisPosix_TestInfra.h"

// Thread function prototypes
void Thread1(void *argument);

#define THREAD_NAME "thread name"

void test_start(void)
{
  osThreadAttr_t attr = (osThreadAttr_t){0};

  osKernelInitialize();                 // Initialize CMSIS-RTOS

  attr.name = THREAD_NAME;

  osThreadId_t thread_id = osThreadNew(Thread1, NULL, &attr);    // Create Thread1
  CP_ASSERT_NE(NULL, thread_id);

  const char *name = osThreadGetName(thread_id);
  CP_ASSERT_STREQ(name, THREAD_NAME);

  osKernelStart();                     // Start scheduler

  CP_ASSERT_UNREACHABLE();
}

void Thread1(void *argument)
{
  (void) argument;
  return;
}
