# Splinter — ClearCore Toolhead Control
 
C++ scripts for the **Teknic ClearCore PLC** controlling two interchangeable toolheads on **Splinter**, a 6-axis ABB IRB-2600 robotic arm used for additive manufacturing research at [New Bedford Research & Robotics (NBRR)](https://nbrr.org).
 
Splinter can print with two completely different materials — thermoplastic pellets and ceramic clay — each requiring its own motor type, wiring architecture, and control logic. These scripts are the brain behind both.
 
---
 
## Scripts
 
### `Thermoplastics_Master.cpp`
Controls the **MDPH2 pellet extruder** (Massive Dimension) mounted on Splinter.
 
The thermoplastic motor is a native Teknic device, so Teknic's built-in C++ API handles velocity control directly. The script reads ABB digital and analog I/Os through the ClearCore's input pins and translates them into real-time toolhead commands.
 
**Functions:**
- Start/stop extrusion via digital signal from ABB (`DO1 → A10`)
- Variable motor velocity via analog input from ABB (`AO1 → A12`)
- Staged activation of 4 blower fans in 2 groups via 24V relay (`DO4/DO5 → IO3/IO2`)
**Key parameters:**
| Parameter | Value |
|---|---|
| Max Velocity | 30,000 steps/s |
| Max Acceleration | 5,000 steps/s² |
| RPM Multiplier | 0.0187 |
 
---
 
### `Ceramics_Master.cpp`
Controls a **Nema 23 closed-loop stepper motor** (23HS45-420D-E1000) driving the ceramic clay extruder via a CL57T stepper driver.
 
The ceramic motor is **not** a Teknic device and is incompatible with Teknic's native motor API. Stepping is handled manually — a digital output is toggled HIGH/LOW with a calculated microsecond delay. Motor speed is controlled by adjusting this delay via analog input. A smoothing interpolation prevents abrupt acceleration/deceleration from causing missed steps or errors.
 
> ⚠️ The loop contains zero extra logic by design. Every additional calculation adds latency to the step timing. Do not add code to the loop without accounting for its effect on step frequency.
 
**Functions:**
- Motor enable/disable via 24V relay (`DO5 → A9 → IO2`)
- Variable velocity via analog delay mapping (`AO1 → A12`)
- Motor direction reversal via digital signal (`DO6 → A11 → IO1`)
- Speed smoothing via exponential interpolation
**Key parameters:**
| Parameter | Value |
|---|---|
| Step Resolution | microsecond delay mapping |
| Smoothing Factor | 5.0 (out of 100) |
| Max Delay (dt) | 1000 µs |
 
---
 
## Hardware Context
 
| Component | Details |
|---|---|
| Robot | ABB IRB-2600 (6-axis) |
| PLC | Teknic ClearCore |
| IDE | Arduino IDE (Teknic ClearCore wrapper) |
| Thermoplastic Motor | Teknic CPM (native, step & dir mode) |
| Ceramic Motor | Nema 23 — 23HS45-420D-E1000 |
| Ceramic Driver | Closed-loop stepper driver CL57T |
| Robot Controller | ABB IRC5 |
| Signal Cable | 12-pin gray connector (low voltage signals) |
| Power Cable | 5-pin 24V DC connector |
 
For full pinout and wiring details, refer to the Splinter Cables, Connectors, IOs and Splinter Toolhead Guide documents included in this repository.
 
---
 
## I/O Reference
 
### Thermoplastics
| ABB I/O | ClearCore Input | ClearCore Output | Function |
|---|---|---|---|
| DO1 | A10 | M-0 | Start/stop extrusion |
| AO1 | A12 | M-0 | Variable motor velocity |
| DO4 | DI8 | IO3 | Activate fans group 1 |
| DO5 | A9 | IO2 | Activate fans group 2 |
 
### Ceramics
| ABB I/O | ClearCore Input | ClearCore Output | Function |
|---|---|---|---|
| DO1 | A10 | IO0 | Start/stop extrusion |
| AO1 | A12 | IO0 | Variable motor velocity |
| DO5 | A9 | IO2 | Enable motor (24V relay) |
| DO6 | A11 | IO1 | Reverse motor direction |
 
---
 
## Usage
 
1. Open the appropriate script in **Arduino IDE** with the Teknic ClearCore board package installed.
2. Verify the correct toolhead is physically connected to Splinter.
3. Upload the script to the ClearCore.
4. Control via ABB FlexPendant I/Os or RAPID code.
> ⚠️ **Do not run `Thermoplastics_Master` with the ceramic toolhead connected, or vice versa.** The relay and motor configurations are toolhead-specific. Incorrect script/toolhead combinations can damage equipment.
 
---
 
## Notes
 
- `DO4`/`DO5` on the thermoplastics script can be wired directly to 24V relay ports 2+3 to free up ClearCore pins if needed. They are currently wired through the ClearCore for data analytics purposes.
- `DO5` on the ceramics script can similarly be wired directly to relay port 3. `DO6` (direction) **must** remain plugged into the ClearCore.
- If additional ClearCore pins are needed across either script, a **Teknic CCIO-8** expansion board is available.
---
 
*Built at New Bedford Research & Robotics — Keigan Cullen, 2025*
