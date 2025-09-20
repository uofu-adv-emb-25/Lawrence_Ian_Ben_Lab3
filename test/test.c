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
#include "my_functions.h"

#define A_TASK_PRIORITY (TEST_RUNNER_PRIORITY - 1UL)
#define A_TASK_STACK_SIZE configMINIMAL_STACK_SIZE

#define B_TASK_PRIORITY (TEST_RUNNER_PRIORITY - 1UL)
#define B_TASK_STACK_SIZE configMINIMAL_STACK_SIZE

#define TEST_RUNNER_PRIORITY ( tskIDLE_PRIORITY + 5UL )

SemaphoreHandle_t semaphore;

int counter;
int on;

// void work(SemaphoreHandle_t semaphore, char *caller, int *counter, uint32_t delay)

void setUp(void) {}

void tearDown(void) {}

void side_thread(void){
    if (xSemaphoreTake(semaphore, portMAX_DELAY) == 1) {
        sleep_ms(100); 
        printf("hello world from %s! Count %d\n", "thread", counter++);
        xSemaphoreGive(semaphore);
    }
}

void main_thread(void){
    if (xSemaphoreTake(semaphore, portMAX_DELAY) == 1) {
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, on);
        sleep_ms(100); 
        printf("hello world from %s! Count %d\n", "main", counter++);
        on = !on;
        xSemaphoreGive(semaphore);
    }
}

// void work(SemaphoreHandle_t semaphore, char *caller, int *counter, uint32_t delay)

void test_simple_lock(void) {
    xSemaphoreTake(semaphore, pdMS_TO_TICKS(10));
    // Check that the we are locked out
    TEST_ASSERT_EQUAL_MESSAGE(0, xSemaphoreTake(semaphore, pdMS_TO_TICKS(10)), "Lock was not properly set");
    xSemaphoreGive(semaphore);
}

void test_dead_lock(void){
    SemaphoreHandle_t a_lock = xSemaphoreCreateCounting(1, 1);
    SemaphoreHandle_t b_lock = xSemaphoreCreateCounting(1, 1);
    counter = 0;
    TaskHandle_t a, b;

    struct Args taskA_Args = {a_lock, b_lock, &counter};
    struct Args taskB_Args = {a_lock, b_lock, &counter};
    xTaskCreate(taskA, "firstThread",
                    A_TASK_STACK_SIZE, (void*) &taskA_Args, A_TASK_PRIORITY, &a);
    xTaskCreate(taskB, "secondThread",
                    B_TASK_STACK_SIZE, (void*) &taskB_Args, B_TASK_PRIORITY, &b);

    vTaskDelay(1000);
    TEST_ASSERT_EQUAL_MESSAGE(2, counter, "Counter should only increment to 2.");
    //TEST_ASSERT_EQUAL_MESSAGE(0, xSemaphoreTake(a_lock, pdMS_TO_TICKS(10)), "a lock was not is use.");
    //TEST_ASSERT_EQUAL_MESSAGE(0, xSemaphoreTake(b_lock, pdMS_TO_TICKS(10)), "b lock was not in use.");
    
    vTaskDelete(a);
    vTaskDelete(b);
    
}
void runner_thread (__unused void* args){
    TaskHandle_t main, side;
    sleep_ms(5000); // Give time for TTY to attach.
    TEST_ASSERT_TRUE_MESSAGE(semaphore != NULL, "Semaphore is returned as NULL");

    for (;;){
        printf("Start tests\n");
        UNITY_BEGIN();
        RUN_TEST(side_thread);
        sleep_ms(100);
        RUN_TEST(main_thread);
        sleep_ms(100);
        RUN_TEST(test_simple_lock);
        sleep_ms(100);
        
        RUN_TEST(test_dead_lock);
        UNITY_END();
        sleep_ms(10000);
    }


    // if (semaphore != NULL){
    //     printf("Start tests\n");
    //     UNITY_BEGIN();
    //     RUN_TEST(side_thread);
    //     sleep_ms(100);
    //     RUN_TEST(main_thread);
    //     sleep_ms(100);
    //     RUN_TEST(test_simple_lock);
    //     sleep_ms(100);
        
    //     RUN_TEST(test_dead_lock);
    //     UNITY_END();
    
    //     while(1) {sleep_ms(5000);}
    //     return UNITY_END();
    // }
}




int main (void)
{
    stdio_init_all();
    hard_assert(cyw43_arch_init() == PICO_OK);
    on = false;
    counter = 0;
    semaphore = xSemaphoreCreateCounting(1, 1);
    xTaskCreate(runner_thread, "TestRunner",
                configMINIMAL_STACK_SIZE, NULL, TEST_RUNNER_PRIORITY, NULL);
    vTaskStartScheduler();

}