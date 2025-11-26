/**
 * RP2040 FreeRTOS LED/Button App
 *
 * @copyright 2025
 * @version   1.0.0
 * @license   MIT
 *
 */
#include "main.h"

/* FUNCTIONS */

/**
 * @brief LED cycling task:
 *        Green → Yellow → Red, with a variable delay value.
 */
void led_task(QueueHandle_t queue_handle) {

    gpio_init(GREEN_LED_PIN);
    gpio_set_dir(GREEN_LED_PIN, GPIO_OUT);

    gpio_init(YELLOW_LED_PIN);
    gpio_set_dir(YELLOW_LED_PIN, GPIO_OUT);

    gpio_init(RED_LED_PIN);
    gpio_set_dir(RED_LED_PIN, GPIO_OUT);

    volatile int delay_value_ms = 1000;

    while (true) {

        // Receive a message from queue if any
        xQueueReceive(queue_handle, &delay_value_ms, 1/portTICK_PERIOD_MS);
        // RED off, GREEN on
        gpio_put(RED_LED_PIN, 0);
        gpio_put(GREEN_LED_PIN, 1);
        vTaskDelay(delay_value_ms / portTICK_PERIOD_MS);

        // GREEN off, YELLOW on
        gpio_put(GREEN_LED_PIN, 0);
        gpio_put(YELLOW_LED_PIN, 1);
        vTaskDelay(delay_value_ms / portTICK_PERIOD_MS);

        // YELLOW off, RED on
        gpio_put(YELLOW_LED_PIN, 0);
        gpio_put(RED_LED_PIN, 1);
        vTaskDelay(delay_value_ms / portTICK_PERIOD_MS);
    }
}

/**
 * @brief Button monitoring task:
 *        HIGH = fast blink (100 ms)
 *        LOW  = slow blink (1000 ms)
 */
void button_task(QueueHandle_t queue_handle) {

    gpio_init(BUTTON_PIN);
    gpio_set_dir(BUTTON_PIN, GPIO_IN);

    volatile int delay_value_ms = 1000;
    volatile int prev_delay_value_ms = 1000;

    while (true) {

        if (gpio_get(BUTTON_PIN)) {
            delay_value_ms = 100;
        } 
        else {
            delay_value_ms = 1000;
        }

        if (delay_value_ms != prev_delay_value_ms)
        {
            xQueueSend(queue_handle, &delay_value_ms, 1/portTICK_PERIOD_MS);
            prev_delay_value_ms = delay_value_ms;
        }
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}

/* RUNTIME START */
int main() {

    stdio_init_all();

    // Initialize the queue
    QueueHandle_t delayTimeQueue = xQueueCreate(1, sizeof(int));
    // Create tasks and give the buffer as a parameter
    xTaskCreate(button_task, "BUTTON_TASK", 128, delayTimeQueue, 1, NULL);
    xTaskCreate(led_task, "LED_TASK", 128, delayTimeQueue, 1, NULL);
    // Start FreeRTOS
    vTaskStartScheduler();
    // Should never reach here
    while (true) {
    }

}
