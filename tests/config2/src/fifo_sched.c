#include "cmsis_os2.h"
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include "cmsisPosix_Config.h"
#include "cmsisPosix_TestInfra.h"

// Thread function prototypes
void ThreadHigh1(void *argument);
void ThreadHigh2(void *argument);
void ThreadLow(void *argument);

typedef enum
{
  PRE_TEST,
  TEST_STARTED,
  FIRST_THREAD_RUN1,
  FIRST_THREAD_JOIN,
  SECOND_THREAD_RUN,
  SECOND_THREAD_END,
  FIRST_THREAD_END2,
}test_state_e;

test_state_e test_state = PRE_TEST;

void test_start(void)
{
  osThreadId_t thread_id;
  osThreadAttr_t attr = {0};

  attr.priority = osPriorityBelowNormal1;

  CP_ASSERT_EQ(test_state, PRE_TEST);

  osKernelInitialize();                 // Initialize CMSIS-RTOS

  thread_id = osThreadNew(ThreadHigh1, NULL, &attr);
  CP_ASSERT_NE(NULL, thread_id);

  thread_id = osThreadNew(ThreadHigh2, NULL, &attr);
  CP_ASSERT_NE(NULL, thread_id);

  attr.priority = osPriorityLow2;
  thread_id = osThreadNew(ThreadLow, NULL, &attr);
  CP_ASSERT_NE(NULL, thread_id);

  CP_ASSERT_EQ(test_state, PRE_TEST);

  test_state = TEST_STARTED;

  osKernelStart();

  CP_ASSERT_UNREACHABLE();
}

void ThreadHigh2(void *argument)
{
  (void)argument;

  osStatus_t status;

  CP_ASSERT_EQ(test_state, TEST_STARTED);

  test_state = FIRST_THREAD_RUN1;

  // wait long

  CP_ASSERT_EQ(test_state, FIRST_THREAD_RUN1);

  test_state = FIRST_THREAD_JOIN;

  status = osThreadYield();
  CP_ASSERT_EQ(status, osOK);

  CP_ASSERT_EQ(test_state, SECOND_THREAD_END);

  test_state = FIRST_THREAD_END2;

  TEST_LOG_INFO("[Thread1] Done\n");
}

void ThreadHigh1(void *argument)
{
  (void)argument;

  CP_ASSERT_EQ(test_state, FIRST_THREAD_JOIN);
  
  test_state = SECOND_THREAD_END;

  TEST_LOG_INFO("[Thread2] Done\n");
}

void ThreadLow(void *argument)
{
  (void)argument;

  CP_ASSERT_EQ(test_state, FIRST_THREAD_END2);
}