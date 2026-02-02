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
TimerHandle_t t1, t2, t3, t4; // The timers' handles

void watcher_task(void* unused_arguments){  
 
    t1 = xTimerCreate("Step 1", pdMS_TO_TICKS(2000), pdFALSE, (void *)1, callback_function);   // Step 1: make the light yellow for cars
    t2 = xTimerCreate("Step 2", pdMS_TO_TICKS(2000), pdFALSE, (void *)2, callback_function);   // Step 2: make red for cars and green for pedestrians 
    t3 = xTimerCreate("Step 3", pdMS_TO_TICKS(8000), pdFALSE, (void *)3, callback_function);   // Step 3: make yellow for pedestrians
    t4 = xTimerCreate("Step 4", pdMS_TO_TICKS(2000), pdFALSE, (void *)4, callback_function);   // Step 4: restore the original state: green cars, red pedestrians

    while (true)
    {
        // Wait for the buttont to be pressed = pedestrians waiting to cross the road
        if (gpio_get(BUTTON_PIN))
        {
            // Start the cascade
            xTimerStart(t1, 0); 
        }
    }

}

void callback_function( TimerHandle_t xTimer){

    uint16_t id_timer = (uint16_t) pvTimerGetTimerID( xTimer);

    switch (id_timer){
        case 1:
        {
            gpio_put(YELLOW_CAR, 1);
            gpio_put(GREEN_CAR, 0);
            xTimerStart(t2, 0);
            break;
        }
        case 2:
        {
            gpio_put(RED_CAR, 1);
            gpio_put(YELLOW_CAR, 0);
            gpio_put(RED_PEDESTRIAN, 0);
            gpio_put(GREEN_PEDESTRIAN, 1); 
            xTimerStart(t3, 0); 
            break;
        }
        case 3:
        {
            gpio_put(YELLOW_PEDESTRIAN, 1);
            gpio_put(GREEN_PEDESTRIAN, 0);
            xTimerStart(t4, 0); 
            break;
        }
        case 4:
        {
            gpio_put(RED_PEDESTRIAN, 1);
            gpio_put(YELLOW_PEDESTRIAN, 0);
            gpio_put(RED_CAR, 0);
            gpio_put(GREEN_CAR, 1);
            break;
        }
    }
}
/* RUNTIME START */
int main() {

    // Start the microcontroller
    stdio_init_all();

    // Initialize the button
    gpio_init(BUTTON_PIN);
    gpio_set_dir(BUTTON_PIN, GPIO_IN);
    
    // Car lights initialization
    gpio_init(RED_CAR);
    gpio_init(YELLOW_CAR);
    gpio_init(GREEN_CAR);
    gpio_set_dir(RED_CAR, GPIO_OUT);
    gpio_set_dir(YELLOW_CAR, GPIO_OUT);
    gpio_set_dir(GREEN_CAR, GPIO_OUT);
    gpio_put(GREEN_CAR, 1);

    // Pedestrian lights initialization
    gpio_init(RED_PEDESTRIAN);
    gpio_init(YELLOW_PEDESTRIAN);
    gpio_init(GREEN_PEDESTRIAN);
    gpio_set_dir(RED_PEDESTRIAN, GPIO_OUT);
    gpio_set_dir(YELLOW_PEDESTRIAN, GPIO_OUT);
    gpio_set_dir(GREEN_PEDESTRIAN, GPIO_OUT);
    gpio_put(RED_PEDESTRIAN, 1);

    // Create the task that will take care of the traffic :)
    xTaskCreate(watcher_task, "WATCHER", 256, NULL, 1, NULL);

    // Start FreeRTOS
    vTaskStartScheduler();
        
    while (true) {
        // Should not get here
    }
}
