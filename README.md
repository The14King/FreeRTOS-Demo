## Multitask App

This module demonstrates LED sequencing and button-controlled timing on the RP2040.  
It can operate in two different modes depending on the `MULTITASK_APP` compile-time flag.
Check the wokwi (alternative to TinkerCAD, that has Raspberry Pi Pico W) image in /images and put together the hardware yourself.
Then change the flag for yourself, build and see the difference. 

---

### Modes of Operation

#### **1. Multitask Mode (`MULTITASK_APP = true`)**

This mode uses **FreeRTOS** and runs two independent tasks:

- **LED Task**  
  Handles the LED cycle and reads `delay_value_ms` for timing.

- **Button Task**  
  Polls the button every 100 ms and updates `delay_value_ms`.

**Behavior characteristics:**

- LED updates and button polling run concurrently.  
- Button presses are detected quickly.  
- LED behavior remains smooth and uninterrupted.

---

#### **2. Single-Task Mode (`MULTITASK_APP = false`)**

This mode removes FreeRTOS and executes all logic in a single loop.

**Behavior characteristics:**

- Button state is checked once per LED cycle.
- Timing changes only take effect after a full cycle completes.
- Slower responsiveness compared to multitask mode.

## Queue-Based Multitask App

This module improves on the earlier multitask design by removing shared-variable concurrency issues.  
Instead of allowing both tasks to directly access a global `delay_value_ms`, this version uses a **FreeRTOS queue** for safe, event-driven communication between the button task and the LED task.

---

### Purpose of This Version

In the previous assignment, both tasks modified or read a shared `delay_value_ms` value.  
That approach works but can lead to:

- Race conditions  
- Unpredictable timing updates  
- Visibility issues between tasks  

This version solves these problems by sending timing updates **only through a queue**, ensuring that the LED task always receives clean, synchronized timing commands.

---

### How It Works

#### **LED Task**
- Initializes all LED pins.
- Maintains a private `delay_value_ms` variable.
- Periodically checks the queue for new timing values:
  - If a new value is available, it updates its internal delay.
  - If not, it continues with the previously stored delay.
- Performs the LED cycle (Green → Yellow → Red) using the current delay value.

#### **Button Task**
- Monitors the button state every 100 ms.
- Determines the correct timing value:
  - **HIGH** → `100 ms`
  - **LOW** → `1000 ms`
- Sends the updated timing value to the queue **only when it changes**, reducing unnecessary traffic.

---

### Why This Approach Is Better

- **No shared variables** → No risk of concurrent reads/writes.  
- **Event-driven updates** → Only sends data when needed.  
- **Predictable LED behavior** → LED task always uses its own local delay value.  
- **FreeRTOS-safe communication** → Queue ensures atomic transfer between tasks.


## Mutex-Based Multitask App

This module demonstrates another approach to coordinating two FreeRTOS tasks that both need access to a shared delay value.  
Unlike the queue-based version—which sends timing updates explicitly—this version keeps the shared delay in a **global variable** and protects access to it using a **FreeRTOS mutex**.

This provides controlled, synchronized read/write access without changing the program’s structure or data flow.

---

### Purpose of This Version

The earlier version improved concurrency by avoiding shared-state access entirely.  
  
This version intentionally brings back a shared global `globalDelay`, but ensures safety by guarding all access with a mutex:

- Writing to the variable happens only inside a mutex-protected critical section  
- Reading the variable also requires acquiring the same mutex  
- No race conditions  
- No inconsistent reads  

This makes the program closer to “classic” RTOS task-sharing patterns.

---

### How It Works

#### **LED Task**
- Initializes all LED pins.
- Attempts to take the mutex before reading `globalDelay`.
- Copies the shared value into its own local variable (`localValueDelay`).
- Releases the mutex immediately after the read.
- Uses the captured delay to perform the standard LED cycle.

**Result:**  
The LED task never uses a half-updated or inconsistent delay value.

---

#### **Button Task**
- Reads the physical button every 100 ms.
- Computes the new delay (`100 ms` fast or `1000 ms` slow).
- If the value has changed:
  - Takes the mutex  
  - Updates `globalDelay`  
  - Releases the mutex  
- Remembers the previous value to avoid unnecessary lock attempts.

**Result:**  
Button timing updates are synchronized and atomic.

---

### Why Use a Mutex?

A mutex is appropriate when:

- You want tasks to share the same variable directly  
- You want to maintain a single source of truth  
- You need to prevent simultaneous access  
- You want updates to be visible immediately to other tasks  

This method is lightweight and easy to integrate into existing designs.

---

## Timers-based Traffic Light Sequence

The final part of this demo showcases the use of **FreeRTOS software timers** to manage a complete traffic-light sequence in a clean, non-blocking way.

Instead of relying on delays or state machines inside tasks, the timing logic is delegated entirely to FreeRTOS timers.

---

## Watcher Task and Timer Creation

The `watcher_task` is responsible for monitoring the pedestrian button and triggering the traffic sequence when the button is pressed.

Inside this task, **four one-shot software timers** are created:

- **Timer 1 (3 seconds)**  
  Prepares cars to stop by switching the car light from green to yellow.

- **Timer 2 (5 seconds)**  
  Stops cars and allows pedestrians to cross (red for cars, green for pedestrians).

- **Timer 3 (12 seconds)**  
  Signals pedestrians that the crossing phase is ending by switching to yellow.

- **Timer 4 (14 seconds)**  
  Restores the original traffic state (green for cars, red for pedestrians).

All timers are created with `pdFALSE`, meaning they execute **only once per activation**.  
Each timer is assigned a **unique ID (1–4)**, which is later used inside the callback function to determine the correct action.

---

## Button Trigger and Timer Start

The watcher task continuously checks the button state:

- When the button is pressed (pedestrians request to cross),
- All four timers are started simultaneously using `xTimerStart()`.

From this point onward, the task does nothing else—the entire sequence is handled asynchronously by the timers.

This allows the system to remain responsive while the timing logic runs independently.

---

## Timer Callback Function

All timers share a single callback function: `callback_function`.

When a timer expires:
1. FreeRTOS calls the callback automatically.
2. The timer ID is retrieved using `pvTimerGetTimerID()`.
3. A `switch` statement executes the corresponding traffic-light action.

### Callback Steps

- **Step 1 (Timer ID 1)**  
  Cars switch from green to yellow.

- **Step 2 (Timer ID 2)**  
  Cars turn red and pedestrians turn green.

- **Step 3 (Timer ID 3)**  
  Pedestrians switch from green to yellow.

- **Step 4 (Timer ID 4)**  
  Pedestrians return to red and cars go back to green.

Because each timer expires at a different time, the traffic-light sequence unfolds automatically without blocking any task.

---

## Why Software Timers Are Used

This design highlights the advantages of FreeRTOS software timers:

- No blocking delays (`vTaskDelay`)  
- No busy-wait loops  
- Precise and deterministic timing  
- Clear separation between event handling (button press) and timed behavior  
- Easy to extend with additional steps or logic  

Once the scheduler is started in `main()`, the system runs autonomously: a button press triggers the timers, and the timers drive the entire traffic-light sequence step by step.

---

This completes the demo by demonstrating how **FreeRTOS software timers can be used to manage complex, time-dependent behavior in a clean, efficient, and scalable way**.


TODO: Do not forget to give credit to Tony Smitty for template