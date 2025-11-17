Here is your **new, updated, complete README**, rewritten to match:

✔ your **new joystick-based UI**
✔ your **OLED menu system**
✔ the **RoutineManager**
✔ the **modular actuator classes** (Linear, Rail, Claw, Servo)
✔ the **clean structured architecture**
✔ your new **Config.h**, **Safety**, **Indicators**, etc.
✔ the new **hybrid scheduler**
✔ the new **folders & file naming**

This README is clean, professional, accurate to your current system, and future-proof.

---

# 📦 Decapper Control System (Arduino MEGA)

*A modular multi-axis robotic system for automatic screw-cap and snaplink decapping.*

---

# 🧩 Overview

This project implements a **full robotic decapper** built on:

* ✔ Arduino Mega
* ✔ TMC2209 linear actuator
* ✔ 2× 28BYJ-48 steppers (rail + claw rotation)
* ✔ MG996R servo claw
* ✔ Hall sensor + limit switch homing
* ✔ OLED graphical UI with joystick navigation
* ✔ Modular C++ architecture
* ✔ Safe hybrid motion scheduling
* ✔ Snaplink & Screw-cap routines
* ✔ Fully non-blocking, real-time loop (200 Hz)

---

# 🎯 High-Level Features

### **🖥 Graphical OLED UI**

* 1.3" SH1106 display
* Mode selection (SNAP, SCREW, HOME, JOG, INFO)
* Real-time progress bar
* Jog movement feedback
* Homing, error, and status display

### **🕹 Joystick Input**

* Left/right → switch modes
* Up/down → jog axes
* Click → servo open/close
* Start/Stop buttons for actions

### **🦾 Multi-Axis Actuator Control**

* **Linear Actuator / TMC2209 / AccelStepper**
* **Rail Stepper (ULN2003)**
* **Claw Rotation Stepper (ULN2003 + Hall sensor)**
* **Servo Claw (MG996R)**

### **🧠 Modular Routines**

* SnapLinkRoutine
* ScrewRoutine
* HomeRoutine
* JogRoutine
* CalibrationRoutine
* RoutineManager responsible for orchestrating them

### **⚡ Safety System**

* Hard STOP interrupt
* Motion lockouts
* Sensor polarity configuration
* Load-based actuator scheduling

---

# 📁 Project Structure

```
/src
│
├── main/
│     ├── main.ino
│     ├── RoutineManager.h
│     ├── RoutineManager.cpp
│
│     ├── LinearActuator.h
│     ├── LinearActuator.cpp
│     ├── RailStepper.h
│     ├── RailStepper.cpp
│     ├── ClawStepper.h
│     ├── ClawStepper.cpp
│     ├── ServoClaw.h
│     └── ServoClaw.cpp
│
│     ├── HomeRoutine.h
│     ├── HomeRoutine.cpp
│     ├── JogRoutine.h
│     ├── JogRoutine.cpp
│     ├── SnapLinkRoutine.h
│     ├── SnapLinkRoutine.cpp
│     ├── ScrewRoutine.h
│     ├── ScrewRoutine.cpp
│     ├── CalibrationRoutine.h
│     └── CalibrationRoutine.cpp
│
│     ├── UI_OLED.h
│     └── UI_OLED.cpp
│
│     ├── Safety.h
│     ├── Safety.cpp
│     ├── Indicators.h
│     ├── Indicators.cpp
│     ├── Pins.h
│     └── Config.h
│
└── README.md
```

> Arduino IDE requires all files to reside in the same folder,
> but this virtual structure shows the intended modular layout.

---

# 🔌 Hardware Components

### **Actuators**

| Component             | Driver                    | Notes               |
| --------------------- | ------------------------- | ------------------- |
| Linear Actuator       | TMC2209 (UART + STEP/DIR) | Limit switch homing |
| Rail Stepper          | ULN2003                   | Full-step sequence  |
| Claw Rotation Stepper | ULN2003                   | Hall sensor homing  |
| Servo Claw            | MG996R                    | Smooth easing       |

---

# 🧱 Code Architecture

### **Documentation for each module**

---

## 🧩 **1. Config.h**

Central configuration file controlling:

* Sensor polarity
* Axis speeds
* Servo limits
* Motion scheduling rules
* Routine timing
* Jog speeds
* System tick rate

Change everything here — **no rewrites needed.**

---

## 🧩 **2. Pins.h**

Pin definitions for:

* Linear actuator STEP/DIR/EN
* Rail & Claw stepper coils
* Limit & Hall sensors
* Servo
* Joystick
* Buttons
* Indicators

Clean and consistent.

---

## 🧩 **3. Safety System**

Files:

```
Safety.h
Safety.cpp
```

Provides:

* STOP button override
* Emergency shutdown
* Motor disable
* Safety flags
* Door switch compatibility

Integrated into main loop at high priority.

---

## 🧩 **4. Indicators**

Files:

```
Indicators.h
Indicators.cpp
```

LED animations for:

* IDLE → breathing
* RUN → blinking
* ERROR → solid red

Driven at 200 Hz via main loop.

---

## 🧩 **5. UI_OLED**

Files:

```
UI_OLED.h
UI_OLED.cpp
```

Features:

* Mode strip with animated highlight
* Header bar with status indicator (IDLE / RUN / HOME)
* Progress bar system
* Jog feedback
* Info screens
* Full non-blocking draw cycle

---

# 🚦 Routines Overview

### All routines follow:

* State machine design
* Non-blocking updates
* Hybrid power scheduling
* Safety check every cycle
* UI updates every frame

---

## 🔧 **HomeRoutine**

Sequence:

1. Rail → home
2. Claw → hall sensor
3. Linear → upper limit
4. Servo → open
5. Reset axes & flags

---

## 🔧 **JogRoutine**

User-controlled:

* X = claw rotation
* Y = linear actuator
* Press = servo open/close
* Compliant movement based on Config.h jog speeds

---

## 🔧 **SnapLinkRoutine**

1. Rail moves into position
2. Linear actuator lowers
3. Servo closes claw
4. Linear actuator lifts sharply
5. Rail returns home

---

## 🔧 **ScrewRoutine**

1. Rail moves into position
2. Linear actuator lowers
3. Servo closes strongly
4. Torque break rotation
5. Continuous rotation + slow upward lift
6. Linear full lift
7. Rail retreats

---

## 🔧 **CalibrationRoutine**

Joystick-assisted positioning for:

* Linear
* Rail
* Claw
* Servo

Stores calibration offsets.

---

# 🧠 RoutineManager

Files:

```
RoutineManager.h
RoutineManager.cpp
```

Coordinates everything:

* Determines which routine to run
* Owns routine state
* Triggers run() calls
* Passes progress to UI
* Ensures no routine overlaps

Main loop becomes extremely clean:

```cpp
void loop() {
    ui.update();
    safety.update();
    indicators.update();

    linear.update();
    rail.update();
    claw.update();
    servo.update();

    routineManager.update();
}
```

---

# 🧪 Testing Instructions

### **1. UI & Input Test**

* Move joystick L/R → mode cycles
* Press START → begins routine
* Press STOP → forces halt

### **2. Motor Tests (individual)**

```cpp
linear.moveToMM(40);
rail.moveSteps(500);
claw.rotateDegrees(90);
servo.open();
```

### **3. Jog Test**

Select JOG → move joystick in 4 directions.

### **4. Homing Test**

Select HOME → START.

### **5. Routine Test**

SNAP or SCREW → EXECUTE → START.

---

# ⚠️ Safety Guidelines

* Never run without endstops connected.
* Perform HOMING before any routine.
* STOP button must always be reachable.
* Keep fingers away from claw and rotation axis.
* Adjust TMC2209 Vref properly.

---

# 📜 License

MIT — free for personal or commercial use.

---
