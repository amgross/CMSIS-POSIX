#include "cmsis_os2.h"
#include <stdio.h>
#include <stdbool.h>
#include "cmsisPosix_TestInfra.h"

static osMutexId_t mutex;
void thread_recursive(void *argument);

void test_start(void)
{
    osThreadAttr_t tattr = {0};
    osMutexAttr_t mattr = {0};

    osKernelInitialize();

    mattr.attr_bits = osMutexRecursive;
    mattr.name = "recursive_mutex";

    mutex = osMutexNew(&mattr);
    CP_ASSERT_NE(mutex, NULL);

    osThreadId_t th = osThreadNew(thread_recursive, NULL, &tattr);
    CP_ASSERT_NE(th, NULL);

    osKernelStart();

    CP_ASSERT_UNREACHABLE();
}

void thread_recursive(void *argument)
{
    (void)argument;
    osStatus_t status;

    status = osMutexAcquire(mutex, osWaitForever);
    CP_ASSERT_EQ(status, osOK);

    /* recursive lock again should succeed */
    status = osMutexAcquire(mutex, osWaitForever);
    CP_ASSERT_EQ(status, osOK);

    /* release twice */
    status = osMutexRelease(mutex);
    CP_ASSERT_EQ(status, osOK);

    status = osMutexRelease(mutex);
    CP_ASSERT_EQ(status, osOK);

    return;
}
