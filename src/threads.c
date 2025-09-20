#include <stdio.h>
#include <FreeRTOS.h>
#include <semphr.h>
#include <task.h>
#include <pico/stdlib.h>
#include <pico/multicore.h>
#include <pico/cyw43_arch.h>
#include <my_functions.h>

#define MAIN_TASK_PRIORITY (tskIDLE_PRIORITY + 1UL)
#define MAIN_TASK_STACK_SIZE configMINIMAL_STACK_SIZE

#define SIDE_TASK_PRIORITY (tskIDLE_PRIORITY + 1UL)
#define SIDE_TASK_STACK_SIZE configMINIMAL_STACK_SIZE

SemaphoreHandle_t semaphore;

int counter;
int on;

void side_thread(void *params)
{
    while (1)
    {
        vTaskDelay(100);
        work(semaphore, "thread", &counter, portMAX_DELAY);
    }
}

void main_thread(void *params)
{
    while (1)
    {
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, on);
        on = !on;
        vTaskDelay(100);
        work(semaphore, "main", &counter, portMAX_DELAY);
    }
}

int main(void)
{
    stdio_init_all();
    hard_assert(cyw43_arch_init() == PICO_OK);
    on = false;
    counter = 0;
    TaskHandle_t main, side;
    semaphore = xSemaphoreCreateCounting(1, 1);
    if (semaphore != NULL)
    {
        xTaskCreate(main_thread, "MainThread",
                    MAIN_TASK_STACK_SIZE, NULL, MAIN_TASK_PRIORITY, &main);
        xTaskCreate(side_thread, "SideThread",
                    SIDE_TASK_STACK_SIZE, NULL, SIDE_TASK_PRIORITY, &side);
        vTaskStartScheduler();
    }
    return 0;
}