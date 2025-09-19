#include <semphr.h>
#include <FreeRTOS.h>

void work(SemaphoreHandle_t semaphore, char *caller, int *counter, uint32_t delay)
{
    if (xSemaphoreTake(semaphore, delay) == pdTRUE)
    {
        printf("hello world from %s! Count %d\n", caller, (*counter)++);
        xSemaphoreGive(semaphore);
    }
}