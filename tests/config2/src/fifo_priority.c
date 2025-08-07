
#include "cmsis_os2.h"
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include "cmsisPosix_Config.h"
#include "cmsisPosix_TestInfra.h"

void Thread1(void *argument);
void Thread2(void *argument);

typedef enum
{
    PRE_TEST,
    TEST_STARTED,
    FIRST_THREAD_CHANGE_PRIORITY,
    SECOND_THREAD_CHANGE_PRIORITY,
    FIRST_THREAD_END,
}test_state_e;

test_state_e test_state = PRE_TEST;
osThreadId_t first_thread_id;

void test_start(void)
{
    osThreadId_t thread_id;
    osThreadAttr_t attr = {0};

    CP_ASSERT_EQ(test_state, PRE_TEST);

    osKernelInitialize();

    attr.priority = osPriorityAboveNormal1;
    first_thread_id = osThreadNew(Thread1, NULL, &attr);
    CP_ASSERT_NE(NULL, first_thread_id);

    attr.priority = osPriorityBelowNormal;
    thread_id = osThreadNew(Thread2, NULL, &attr);
    CP_ASSERT_NE(NULL, thread_id);

    CP_ASSERT_EQ(test_state, PRE_TEST);

    test_state = TEST_STARTED;

    osKernelStart();

    CP_ASSERT_UNREACHABLE();
}

void Thread1(void *argument)
{
     (void)argument;

     osStatus_t status;

    CP_ASSERT_EQ(test_state, TEST_STARTED);

    test_state = FIRST_THREAD_CHANGE_PRIORITY;
    status = osThreadSetPriority(osThreadGetId(), osPriorityLow2);
    CP_ASSERT_EQ(status, osOK);

    CP_ASSERT_EQ(test_state, SECOND_THREAD_CHANGE_PRIORITY);

    test_state = FIRST_THREAD_END;

}

void Thread2(void *argument)
{
    (void)argument;
    osStatus_t status;

    CP_ASSERT_EQ(test_state, FIRST_THREAD_CHANGE_PRIORITY);

    test_state = SECOND_THREAD_CHANGE_PRIORITY;
    status = osThreadSetPriority(first_thread_id, osPriorityAboveNormal1);
    CP_ASSERT_EQ(status, osOK);

    CP_ASSERT_EQ(test_state, FIRST_THREAD_END);

}