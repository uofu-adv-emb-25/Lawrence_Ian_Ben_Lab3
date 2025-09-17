#include <stdio.h>
#include <pico/stdlib.h>
#include <stdint.h>
#include <unity.h>
#include "unity_config.h"
#include <pico/multicore.h>
#include <pico/cyw43_arch.h>
#include <semphr.h>
#include <FreeRTOS.h>


#include <unity.h>
#include "unity_config.h"
#include <stdint.h>
#include <stdio.h>
#include <FreeRTOS.h>
#include <semphr.h>
#include <task.h>
#include <pico/stdlib.h>
#include <pico/multicore.h>
#include <pico/cyw43_arch.h>
SemaphoreHandle_t semaphore;

int counter;
int on;


void setUp(void) {}

void tearDown(void) {}

void side_thread(void * params){
    if (xSemaphoreTake(semaphore, portMAX_DELAY) == pdTRUE) {
        sleep_ms(100); 
        printf("hello world from %s! Count %d\n", "thread", counter++);
        xSemaphoreGive(semaphore);
    }
}

void main_thread(void * params){
    if (xSemaphoreTake(semaphore, portMAX_DELAY) == pdTRUE) {
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, on);
        sleep_ms(100); 
        printf("hello world from %s! Count %d\n", "main", counter++);
        on = !on;
        xSemaphoreGive(semaphore);
    }
}
int main (void)
{
    stdio_init_all();
    hard_assert(cyw43_arch_init() == PICO_OK);
    on = false;
    counter = 0;
    TaskHandle_t main, side;
    semaphore = xSemaphoreCreateCounting(1, 1);
    sleep_ms(5000); // Give time for TTY to attach.
    TEST_ASSERT_TRUE_MESSAGE(sempahore != NULL, "Semaphore is returned as NULL");
    if (semaphore != NULL){
        printf("Start tests\n");
        UNITY_BEGIN();
        side_thread("TestSideThread");
        sleep_ms(5000);
        main_thread("TestMainThread");
        while(1) {sleep_ms(5000);}
        return UNITY_END();
    }
}