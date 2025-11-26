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



TODO: Do not forget to give credit to Tony Smitty for template