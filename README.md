# Multitask App – RP2040 Traffic Light Demo

## Prerequisites

Before using this project, your system must be set up for **RP2040 C/C++ development**.

**Students:** go to the *Week 1 setup* from the **Microcontrollers module** in the curriculum before you continue. The module's start includes installing the Pico SDK, toolchain, and required build tools.

---

## Usage

Follow these steps to build and run the project:

1. **Clone the repository**

   ```bash
   git clone https://github.com/The14King/FreeRTOS-Demo
   ```

2. **Enter the repository**

   ```bash
   cd RP2040-FreeRTOS
   ```

3. **Install required submodules**
   ```bash
   git submodule update --init
   ```

4. **Install the Pico SDK submodules**

   ```bash
   cd pico-sdk
   git submodule update --init
   cd ..
   ```

5. **Configure the build**

   ```bash
   cmake -S . -B build
   ```

6. **Build the application**

   ```bash
   cmake --build build
   ```

7. **Connect your RP2040 device**
   Attach your Raspberry Pi Pico / Pico W and transfer the UF2 file to it to see it in action.

---

This project demonstrates multitasking, task synchronization, and timer-based event handling on the RP2040 (Raspberry Pi Pico / Pico W) using FreeRTOS. It showcases multiple design approaches for coordinating LED sequencing and button-controlled timing.

> Hardware setup can be recreated using the Wokwi simulator (see `/images`) or built physically with a Raspberry Pi Pico W.

---

## Overview

The application supports several progressively more robust designs:

* **Single-task loop** (no RTOS)
* **Multitask FreeRTOS app** with shared variables
* **Queue-based multitask design** (event-driven, no shared state)
* **Mutex-protected multitask design** (classic shared-state RTOS pattern)
* **Software-timer–driven traffic light sequence**

Each version highlights different concurrency and timing tradeoffs.

---

## Compile-Time Modes

### 1. Multitask Mode (`MULTITASK_APP = true`)

Uses FreeRTOS with two concurrent tasks:

* **LED Task** – Cycles LEDs (Green → Yellow → Red) using a configurable delay
* **Button Task** – Polls the button every 100 ms and updates the timing

**Characteristics**:

* Fast button responsiveness
* Smooth LED behavior
* Concurrent execution

### 2. Single-Task Mode (`MULTITASK_APP = false`)

Runs all logic inside a single loop without FreeRTOS.

**Characteristics**:

* Button checked once per LED cycle
* Timing changes apply only after a full cycle
* Reduced responsiveness

---

## Queue-Based Multitask Design

This version removes shared-variable access by using a FreeRTOS queue to communicate timing updates.

### How It Works

**LED Task**

* Maintains a private delay value
* Reads new timing values from the queue (if available)
* Runs the LED cycle using the most recent value

**Button Task**

* Polls the button every 100 ms
* Sends a new delay value (100 ms or 1000 ms) *only when it changes*

### Benefits

* No race conditions
* Event-driven updates
* Predictable and isolated LED timing

---

## Mutex-Based Multitask Design

This version reintroduces a shared global delay value, protected by a FreeRTOS mutex.

### How It Works

**LED Task**

* Acquires the mutex
* Copies the shared delay into a local variable
* Releases the mutex immediately
* Uses the local copy for the LED cycle

**Button Task**

* Polls the button every 100 ms
* Updates the shared delay only when it changes
* Performs all writes inside a mutex-protected section

### Why a Mutex?

* Maintains a single source of truth
* Prevents concurrent access
* Ensures consistent reads and writes
* Mirrors common RTOS design patterns

---

## Software Timer–Based Traffic Light Sequence

The final demo uses **FreeRTOS software timers** to manage a complete traffic-light sequence without blocking tasks.

### Design

A `watcher_task` monitors the pedestrian button and starts four one-shot timers when pressed:

| Timer | Delay | Action                        |
| ----- | ----- | ----------------------------- |
| 1     | 3 s   | Cars: Green → Yellow          |
| 2     | 5 s   | Cars: Red, Pedestrians: Green |
| 3     | 12 s  | Pedestrians: Green → Yellow   |
| 4     | 14 s  | Reset to default state        |

All timers share a single callback function. The timer ID determines which action is executed when the timer expires.

### Advantages

* No blocking delays (`vTaskDelay`)
* Fully asynchronous sequencing
* Clear separation between events and timing logic
* Easy to extend or modify

Once the scheduler starts, the system runs autonomously: a button press triggers the timers, and the timers control the entire sequence.

---


## Credits

The template for this project was provided by [Tony Smith](https://github.com/smittytone/RP2040-FreeRTOS).
