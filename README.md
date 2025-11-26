## Multitask App

This module demonstrates LED sequencing and button-controlled timing on the RP2040.  
It can operate in two different modes depending on the `MULTITASK_APP` compile-time flag.
Check the TinkerCAD image in /images and put together the hardware yourself.
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


TODO: Do not forget to give credit to Tony Smitty for template