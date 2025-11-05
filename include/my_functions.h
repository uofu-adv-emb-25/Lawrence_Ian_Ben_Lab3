#include <stdio.h>
#include <FreeRTOS.h>
#include <semphr.h>
#include <pico/stdlib.h>
void work(SemaphoreHandle_t semaphore, char* caller, int* counter, uint32_t delay);
void long_work(SemaphoreHandle_t semaphore, uint32_t delay);
void taskA(void * params);
void taskB(void * params);
void task_orphaned(void * params);
void task_unorphaned(void * params);

struct Args {
    SemaphoreHandle_t a_lock;
    SemaphoreHandle_t b_lock;
    int * counter;
};