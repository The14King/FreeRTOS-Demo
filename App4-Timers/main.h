/**
 * RP2040 FreeRTOS LED/Button App
 *
 * @copyright 2025
 * @version   1.0.0
 * @license   MIT
 *
 */
#ifndef MAIN_H
#define MAIN_H

// FreeRTOS
#include <FreeRTOS.h>
#include <task.h>
#include <timers.h>

// C
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

// Pico SDK
#include "pico/stdlib.h"

/* CONSTANTS */
#define     GREEN_CAR       4
#define     YELLOW_CAR      5
#define     RED_CAR         6
#define     GREEN_PEDESTRIAN       8
#define     YELLOW_PEDESTRIAN      9
#define     RED_PEDESTRIAN         10
#define     BUTTON_PIN          7

/* PROTOTYPES */
void callback_function( TimerHandle_t Timer);
void watcher_task(void* unused_arguments);

#endif  // MAIN_H
