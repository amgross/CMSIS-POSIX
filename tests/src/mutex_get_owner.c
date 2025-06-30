#include "cmsis_os2.h"
#include <stdio.h>
#include <stdbool.h>
#include "cmsisPosix_TestInfra.h"

void Thread_owner(void *argument);
void second_thread(void *argument);

osThreadId_t g_thread_id;
static osMutexId_t mutex;

void test_start(void)
{
    osThreadId_t mutex_owner;

    osKernelInitialize();

    mutex = osMutexNew(NULL);          // Create mutex with default attributes
    CP_ASSERT_NE(mutex, NULL);

    // Ensure no owner before aquire
    mutex_owner = osMutexGetOwner(mutex);
    CP_ASSERT_EQ(mutex_owner, NULL);

    g_thread_id = osThreadNew(Thread_owner, NULL, NULL);
    CP_ASSERT_NE(g_thread_id, NULL);

    osKernelStart();

    CP_UASSERT_NREACHABLE();
}

void Thread_owner(void *argument)
{
    (void)argument;

    osStatus_t status;
    osThreadId_t curr_thread_id, mutex_owner, second_thread_id;

    status = osMutexAcquire(mutex, osWaitForever);
    CP_ASSERT_EQ(status, osOK);

    curr_thread_id = osThreadGetId();
    CP_ASSERT_NE(curr_thread_id, NULL);
    CP_ASSERT_EQ(curr_thread_id, g_thread_id);
    mutex_owner = osMutexGetOwner(mutex);
    CP_ASSERT_NE(mutex_owner, NULL);

    CP_ASSERT_EQ(g_thread_id, mutex_owner);

    second_thread_id = osThreadNew(second_thread, NULL, NULL);
    CP_ASSERT_NE(second_thread_id, NULL);

    osDelay(20);

    status = osMutexRelease(mutex);
    CP_ASSERT_EQ(status, osOK);

    // Ensure no owner after release
    mutex_owner = osMutexGetOwner(mutex);
    CP_ASSERT_EQ(mutex_owner, NULL);
    
}

void second_thread(void *argument)
{
    (void)argument;

    osThreadId_t mutex_owner;

    mutex_owner = osMutexGetOwner(mutex);
    CP_ASSERT_NE(mutex_owner, NULL);

    CP_ASSERT_EQ(g_thread_id, mutex_owner);
}