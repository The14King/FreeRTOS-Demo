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

// C
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

// Pico SDK
#include "pico/stdlib.h"

/* CONSTANTS */
#define     GREEN_LED_PIN       4
#define     YELLOW_LED_PIN      5
#define     RED_LED_PIN         6
#define     BUTTON_PIN          7

/* PROTOTYPES */
void led_task(void* unused_arg);
void button_task(void* unused_arg);

#endif  // MAIN_H
