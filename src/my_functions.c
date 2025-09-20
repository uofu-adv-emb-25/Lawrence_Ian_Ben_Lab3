#include "my_functions.h"


void work(SemaphoreHandle_t semaphore, char *caller, int *counter, uint32_t delay)
{
    if (xSemaphoreTake(semaphore, delay) == 1)
    {
        printf("hello world from %s! Count %d\n", caller, (*counter)++);
        xSemaphoreGive(semaphore);
    }
}

void long_work(SemaphoreHandle_t semaphore, uint32_t delay)
{
    if (xSemaphoreTake(semaphore, delay) == 1)
    {
        sleep_ms(3000);
        xSemaphoreGive(semaphore);
    }
}

void taskA(void * params ){
    struct Args * task_A_args = (struct Args *) params;
    if (xSemaphoreTake(task_A_args->a_lock, portMAX_DELAY) == 1){
        printf("HELLO from %s! Count %d\n", "a_thread", ++(*(task_A_args->counter)));
        vTaskDelay(100);
        if (xSemaphoreTake(task_A_args->b_lock, portMAX_DELAY) == 1){
            printf("HELLO from %s! Count %d\n", "b_thread", ++(*(task_A_args->counter)));
        }
        xSemaphoreGive(task_A_args->b_lock);
    }

    xSemaphoreGive(task_A_args->a_lock);
    vTaskSuspend(NULL);
}

void taskB(void * params){
    struct Args * task_B_args = (struct Args *) params;
    if (xSemaphoreTake(task_B_args->b_lock, portMAX_DELAY) == 1){
        printf("HELLO from %s! Count %d\n", "b_thread", ++(*(task_B_args->counter)));
        vTaskDelay(100);
        if (xSemaphoreTake(task_B_args->a_lock, portMAX_DELAY) == 1){
            printf("HELLO from %s! Count %d\n", "a_thread", ++(*(task_B_args->counter)));
        }
        xSemaphoreGive(task_B_args->a_lock);
    }
   
    xSemaphoreGive(task_B_args->b_lock);
    vTaskSuspend(NULL);
}

void task_orphaned(void * params){
    struct Args * task_orphaned_args = (struct Args *) params;
    if (xSemaphoreTake(task_orphaned_args->a_lock, portMAX_DELAY) == 1){
        printf("HELLO from %s! Count %d\n", "orphaned_thread", ++(*(task_orphaned_args->counter)));
        vTaskDelete(NULL);
        xSemaphoreGive(task_orphaned_args->a_lock);
    }
    vTaskSuspend(NULL);
}

void task_unorphaned(void * params){
    struct Args * task_orphaned_args = (struct Args *) params;
    if (xSemaphoreTake(task_orphaned_args->a_lock, portMAX_DELAY) == 1){
        printf("HELLO from %s! Count %d\n", "un-orphaned_thread", ++(*(task_orphaned_args->counter)));
        xSemaphoreGive(task_orphaned_args->a_lock);
       
    }
    vTaskSuspend(NULL);
}