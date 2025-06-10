#include "cmsis_os2.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include "cmsisPosix_TestInfra.h"

// Memory Pool ID
static osMessageQueueId_t myMessageQueue;

// Message type
typedef struct {
  uint8_t value;
} message_t;

// Thread function prototypes
void Thread_manager(void *argument);
void Thread1(void *argument);
void Thread2(void *argument);
void Thread3(void *argument);
void Thread4(void *argument);
bool thread1 = true;
bool thread2 = true;
bool thread3 = true;
bool thread4 = true;
void test_start(void)
{
  osKernelInitialize();                 // Initialize CMSIS-RTOS

  osThreadNew(Thread_manager, NULL, NULL);

  osKernelStart();                     // Start scheduler

}

void Thread_manager(void *argument)
{
  (void)argument;
  message_t msg;
  myMessageQueue = osMessageQueueNew(4, sizeof(message_t), NULL);   // Create message queue with default attributes
  CP_ASSERT_NE(myMessageQueue, NULL);
  CP_ASSERT_EQ(osMessageQueueGetCapacity(myMessageQueue), 4);
  CP_ASSERT_EQ(osMessageQueueGetMsgSize(myMessageQueue), sizeof(message_t));
  CP_ASSERT_EQ(osMessageQueueGetCount(myMessageQueue), 0);
  CP_ASSERT_EQ(osMessageQueueGetSpace(myMessageQueue), 4);

  // Check data validation
  CP_ASSERT_EQ(osMessageQueueNew(0, sizeof(message_t), NULL), NULL);
  CP_ASSERT_EQ(osMessageQueueNew(4, 0, NULL), NULL);
  CP_ASSERT_EQ(osMessageQueuePut(NULL, &msg, 0, osWaitForever), osErrorParameter);
  CP_ASSERT_EQ(osMessageQueuePut(myMessageQueue, NULL, 0, osWaitForever), osErrorParameter);
  CP_ASSERT_EQ(osMessageQueueGet(NULL, &msg, NULL, osWaitForever), osErrorParameter);
  CP_ASSERT_EQ(osMessageQueueGet(myMessageQueue, NULL, NULL, osWaitForever), osErrorParameter);
  CP_ASSERT_EQ(osMessageQueueGetCapacity(NULL), 0);
  CP_ASSERT_EQ(osMessageQueueGetMsgSize(NULL), 0);
  CP_ASSERT_EQ(osMessageQueueGetCount(NULL), 0);
  CP_ASSERT_EQ(osMessageQueueGetSpace(NULL), 0);
  CP_ASSERT_EQ(osMessageQueueDelete(NULL), osErrorParameter);

  // Queue up messages with the same priority
  uint8_t prio = 0;
  msg.value = 1;
  CP_ASSERT_EQ(osMessageQueuePut(myMessageQueue, &msg, 0, 0), osOK);
  CP_ASSERT_EQ(osMessageQueueGetCount(myMessageQueue), 1);
  CP_ASSERT_EQ(osMessageQueueGetSpace(myMessageQueue), 3);
  msg.value = 2;
  CP_ASSERT_EQ(osMessageQueuePut(myMessageQueue, &msg, 0, 0), osOK);
  CP_ASSERT_EQ(osMessageQueueGetCount(myMessageQueue), 2);
  CP_ASSERT_EQ(osMessageQueueGetSpace(myMessageQueue), 2);
  msg.value = 3;
  CP_ASSERT_EQ(osMessageQueuePut(myMessageQueue, &msg, 0, 0), osOK);
  CP_ASSERT_EQ(osMessageQueueGetCount(myMessageQueue), 3);
  CP_ASSERT_EQ(osMessageQueueGetSpace(myMessageQueue), 1);
  msg.value = 4;
  CP_ASSERT_EQ(osMessageQueuePut(myMessageQueue, &msg, 0, 0), osOK);
  CP_ASSERT_EQ(osMessageQueueGetCount(myMessageQueue), 4);
  CP_ASSERT_EQ(osMessageQueueGetSpace(myMessageQueue), 0);
  CP_ASSERT_EQ(osMessageQueuePut(myMessageQueue, &msg, 0, 0), osErrorResource);

  // Verify the messages are retrieved in the right order
  CP_ASSERT_EQ(osMessageQueueGet(myMessageQueue, &msg, &prio, 0), osOK);
  CP_ASSERT_EQ(msg.value, 1);
  CP_ASSERT_EQ(prio, 0);
  CP_ASSERT_EQ(osMessageQueueGetCount(myMessageQueue), 3);
  CP_ASSERT_EQ(osMessageQueueGetSpace(myMessageQueue), 1);
  CP_ASSERT_EQ(osMessageQueueGet(myMessageQueue, &msg, &prio, 0), osOK);
  CP_ASSERT_EQ(msg.value, 2);
  CP_ASSERT_EQ(prio, 0);
  CP_ASSERT_EQ(osMessageQueueGetCount(myMessageQueue), 2);
  CP_ASSERT_EQ(osMessageQueueGetSpace(myMessageQueue), 2);
  CP_ASSERT_EQ(osMessageQueueGet(myMessageQueue, &msg, &prio, 0), osOK);
  CP_ASSERT_EQ(msg.value, 3);
  CP_ASSERT_EQ(prio, 0);
  CP_ASSERT_EQ(osMessageQueueGetCount(myMessageQueue), 1);
  CP_ASSERT_EQ(osMessageQueueGetSpace(myMessageQueue), 3);
  CP_ASSERT_EQ(osMessageQueueGet(myMessageQueue, &msg, &prio, 0), osOK);
  CP_ASSERT_EQ(msg.value, 4);
  CP_ASSERT_EQ(prio, 0);
  CP_ASSERT_EQ(osMessageQueueGetCount(myMessageQueue), 0);
  CP_ASSERT_EQ(osMessageQueueGetSpace(myMessageQueue), 4);
  CP_ASSERT_EQ(osMessageQueueGet(myMessageQueue, &msg, NULL, 0), osErrorResource);

  // Queue up messages with different priorities
  msg.value = 2;
  CP_ASSERT_EQ(osMessageQueuePut(myMessageQueue, &msg, msg.value, 0), osOK);
  msg.value = 4;
  CP_ASSERT_EQ(osMessageQueuePut(myMessageQueue, &msg, msg.value, 0), osOK);
  msg.value = 1;
  CP_ASSERT_EQ(osMessageQueuePut(myMessageQueue, &msg, msg.value, 0), osOK);
  msg.value = 3;
  CP_ASSERT_EQ(osMessageQueuePut(myMessageQueue, &msg, msg.value, 0), osOK);

  // Verify the messages are retrieved in the right order
  CP_ASSERT_EQ(osMessageQueueGet(myMessageQueue, &msg, &prio, 0), osOK);
  CP_ASSERT_EQ(msg.value, 4);
  CP_ASSERT_EQ(prio, 4);
  CP_ASSERT_EQ(osMessageQueueGet(myMessageQueue, &msg, &prio, 0), osOK);
  CP_ASSERT_EQ(msg.value, 3);
  CP_ASSERT_EQ(prio, 3);
  CP_ASSERT_EQ(osMessageQueueGet(myMessageQueue, &msg, &prio, 0), osOK);
  CP_ASSERT_EQ(msg.value, 2);
  CP_ASSERT_EQ(prio, 2);
  CP_ASSERT_EQ(osMessageQueueGet(myMessageQueue, &msg, &prio, 0), osOK);
  CP_ASSERT_EQ(msg.value, 1);
  CP_ASSERT_EQ(prio, 1);
  CP_ASSERT_EQ(osMessageQueueGetCount(myMessageQueue), 0);
  CP_ASSERT_EQ(osMessageQueueGet(myMessageQueue, &msg, NULL, 0), osErrorResource);

  // Test message passing between threads using Thread1 and Thread2
  osThreadNew(Thread1, NULL, NULL);
  osThreadNew(Thread2, NULL, NULL);
  while (thread1 || thread2) {}
  CP_ASSERT_EQ(osMessageQueueDelete(myMessageQueue), osOK);

  // Test sorting of message priority across threads using Thread3 and Thread 4
  myMessageQueue = osMessageQueueNew(100, sizeof(message_t), NULL);
  CP_ASSERT_NE(myMessageQueue, NULL);
  osThreadNew(Thread3, NULL, NULL);     // Create Thread3
  osThreadNew(Thread4, NULL, NULL);     // Create Thread4
  while (thread3 || thread4) {}
  CP_ASSERT_EQ(osMessageQueueGetCount(myMessageQueue), 100);
  uint8_t last_priority = 255;
  for (int n = 0;  n < 100;  n++) {
    CP_ASSERT_EQ(osMessageQueueGet(myMessageQueue, &msg, &prio, osWaitForever), osOK);
    CP_ASSERT_TRUE(prio <= last_priority);
    last_priority = prio;
  }
  CP_ASSERT_EQ(osMessageQueueDelete(myMessageQueue), osOK);
}

void Thread1(void *argument)
{
  (void)argument;
  message_t msg;

  for (int n = 0;  n < 100;  n++)
  {
    osDelay(5);
    msg.value = n;
    CP_ASSERT_EQ(osMessageQueuePut(myMessageQueue, &msg, 0, 0), osOK);
  }

  printf("[Thread1] Done\n");
  thread1 = false;
}

void Thread2(void *argument)
{
  (void)argument;
  message_t msg;

  for (int n = 0;  n < 100;  n++)
  {
    CP_ASSERT_EQ(osMessageQueueGet(myMessageQueue, &msg, NULL, 10), osOK);
    CP_ASSERT_EQ(msg.value, n);
  }
  CP_ASSERT_EQ(osMessageQueueGetCount(myMessageQueue), 0);

  printf("[Thread2] Done\n");
  thread2 = false;
}

void Thread3(void *argument)
{
  (void)argument;
  message_t msg;

  for (int n = 0;  n < 50;  n++)
  {
    msg.value = rand() % 256;
    CP_ASSERT_EQ(osMessageQueuePut(myMessageQueue, &msg, msg.value, 0), osOK);
  }

  printf("[Thread3] Done\n");
  thread3 = false;
}

void Thread4(void *argument)
{
  (void)argument;
  message_t msg;

  for (int n = 0;  n < 50;  n++)
  {
    msg.value = rand() % 256;
    CP_ASSERT_EQ(osMessageQueuePut(myMessageQueue, &msg, msg.value, 0), osOK);
  }

  printf("[Thread4] Done\n");
  thread4 = false;
}