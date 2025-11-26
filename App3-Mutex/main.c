/**
 * RP2040 FreeRTOS LED/Button App
 *
 * @copyright 2025
 * @version   1.0.0
 * @license   MIT
 *
 */
#include "main.h"

/* GLOBALS */
volatile uint32_t globalDelay = 1000;

/* FUNCTIONS */

/**
 * @brief LED cycling task:
 *        Green → Yellow → Red, with a variable delay value.
 */
void led_task(void* delayMutex) {

    gpio_init(GREEN_LED_PIN);
    gpio_set_dir(GREEN_LED_PIN, GPIO_OUT);

    gpio_init(YELLOW_LED_PIN);
    gpio_set_dir(YELLOW_LED_PIN, GPIO_OUT);

    gpio_init(RED_LED_PIN);
    gpio_set_dir(RED_LED_PIN, GPIO_OUT);

    volatile uint32_t localValueDelay = 1000;
    while (true) {

        // Wait for the Mutex to be available indefinetly. If it is not, it means that the value is getting changed right now 
        if (xSemaphoreTake(delayMutex, portMAX_DELAY) == pdPASS) {
            localValueDelay = globalDelay;
            xSemaphoreGive(delayMutex);
        }
        // RED off, GREEN on
        gpio_put(RED_LED_PIN, 0);
        gpio_put(GREEN_LED_PIN, 1);
        vTaskDelay(localValueDelay / portTICK_PERIOD_MS);

        // GREEN off, YELLOW on
        gpio_put(GREEN_LED_PIN, 0);
        gpio_put(YELLOW_LED_PIN, 1);
        vTaskDelay(localValueDelay / portTICK_PERIOD_MS);

        // YELLOW off, RED on
        gpio_put(YELLOW_LED_PIN, 0);
        gpio_put(RED_LED_PIN, 1);
        vTaskDelay(localValueDelay / portTICK_PERIOD_MS);
    }
}

/**
 * @brief Button monitoring task:
 *        HIGH = fast blink (100 ms)
 *        LOW  = slow blink (1000 ms)
 */
void button_task(void* delayMutex) {

    gpio_init(BUTTON_PIN);
    gpio_set_dir(BUTTON_PIN, GPIO_IN);

    volatile int delay = 1000;
    volatile int prevDelay = 1000;

    while (true) {

        if (gpio_get(BUTTON_PIN)) {
            delay = 100;
        } 
        else {
            delay = 1000;
        }

        if (delay != prevDelay)
        {
            if (xSemaphoreTake(delayMutex, portMAX_DELAY) == pdPASS) {
                globalDelay = delay;
                xSemaphoreGive(delayMutex);
            }
            prevDelay = delay;
        }
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}
/* RUNTIME START */
int main() {
    
    stdio_init_all();

    // Initialize the mutex for the shared delay time variable
    SemaphoreHandle_t delayMutex = xSemaphoreCreateMutex();
    // Create tasks
    xTaskCreate(&button_task, "BUTTON_TASK", 128, delayMutex, 1, NULL);
    xTaskCreate(&led_task, "LED_TASK", 128, delayMutex, 1, NULL);
    // Start FreeRTOS
    vTaskStartScheduler();
    // Should never reach here
    while (true) {
    }

}
