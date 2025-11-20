
---

# 📦 Decapper Control System (Arduino MEGA)

*A modular multi-axis robotic decapper with screw-cap and snaplink capabilities.*

---

## 🔧 Features

* Linear actuator (TMC2209)
* Rail stepper (28BYJ-48 + ULN2003)
* Claw rotation stepper (28BYJ-48 + ULN2003)
* Servo claw (MG996R)
* Limit switches + Hall sensor for homing
* Hardware control panel
* Mode-based execution system
* Fully modular class-based C++ design
* Extendable routines

---

## 📁 Project Structure

> **Important:** Arduino IDE requires *all files in the same folder*.

```
src/
├─ main/
    │  main.ino
    │
    ├─ PanelIO.h
    ├─ PanelIO.cpp
    │
    ├─ LinearActuator.h
    ├─ LinearActuator.cpp
    │
    ├─ RailStepper.h
    ├─ RailStepper.cpp
    │
    ├─ ClawStepper.h
    ├─ ClawStepper.cpp
    │
    ├─ ServoClaw.h
    ├─ ServoClaw.cpp
    │
    ├─ Calibration.h
    ├─ Calibration.cpp
    │
    ├─ ScrewRoutine.h
    ├─ ScrewRoutine.cpp
    │
    ├─ SnaplinkRoutine.h
    └─ SnaplinkRoutine.cpp
```

---

## 🎮 Control Panel Overview

### **3-Way Mode Switch**

| Position | Description           |
| -------- | --------------------- |
| OFF      | System disabled       |
| SNAPLINK | Run snaplink routine  |
| SCREW    | Run screw-cap routine |

### **2-Way Function Switch**

| Position  | Description        |
| --------- | ------------------ |
| CALIBRATE | Homes all motors   |
| EXECUTE   | Runs selected mode |

### **Buttons**

| Button | Action                      |
| ------ | --------------------------- |
| START  | Begin action / calibration  |
| STOP   | Immediately halt all motors |

---

## 🧠 System Workflow

```
[Select Mode]
      ↓
[Select Function]
      ↓
   Press START
      ↓
-------------------------
| CALIBRATE → Home all |
| EXECUTE   → Run mode |
-------------------------
      ↓
 Press STOP anytime
```

---

# 🔌 Hardware Components

### Motors

| Component       | Driver  | Notes               |
| --------------- | ------- | ------------------- |
| Linear Actuator | TMC2209 | Limit switch homing |
| Rail Stepper    | ULN2003 | Full-step sequence  |
| Claw Stepper    | ULN2003 | Hall effect homing  |
| Servo Claw      | MG996R  | Smooth open/close   |

### Inputs

* Mode switch (3-way)
* Function switch (2-way)
* START button
* STOP button
* Linear actuator limit switch
* Rail limit switch
* Claw hall sensor

---

# 🛠 Code Modules

Each component has its own `.h/.cpp` pair for clean modularity.

---

## **1. PanelIO**

Handles:

* Reading hardware switches
* Debouncing
* Mode/function determination
* START/STOP logic

File:

```
PanelIO.h / PanelIO.cpp
```

---

## **2. LinearActuator**

Features:

* TMC2209 step/direction control
* Homing with limit switch
* Soft limits
* Absolute & relative movement
* Configurable speed

File:

```
LinearActuator.h / LinearActuator.cpp
```

---

## **3. RailStepper**

Features:

* Homing using limit switch
* Full-step sequence
* Adjustable movement speed

File:

```
RailStepper.h / RailStepper.cpp
```

---

## **4. ClawStepper**

Features:

* Hall effect homing
* Degree-based rotation
* Smooth step sequencing

File:

```
ClawStepper.h / ClawStepper.cpp
```

---

## **5. ServoClaw**

Features:

* Smooth open/close
* Tunable speed
* Adjustable grip angles

File:

```
ServoClaw.h / ServoClaw.cpp
```

---

## **6. Calibration Routine**

Homes all axes:

* Linear actuator
* Rail stepper
* Claw stepper

File:

```
Calibration.h / Calibration.cpp
```

---

## **7. ScrewRoutine**

Sequence:

1. Open claw
2. Lower to cap
3. Grip
4. Break torque
5. Rotate while lifting
6. Final lift
7. Hold cap securely

File:

```
ScrewRoutine.h / ScrewRoutine.cpp
```

---

## **8. SnaplinkRoutine**

Sequence:

1. Open claw
2. Lower
3. Partial grip
4. Align with small rotation
5. Quick upward “snap” motion
6. Reset

File:

```
SnaplinkRoutine.h / SnaplinkRoutine.cpp
```

---

# 📌 Tuning Parameters

### Linear Actuator

* steps/mm
* max/min travel
* speed (microsecond delay)

### Rail Stepper

* stepDelay
* direction
* homing logic

### Claw Stepper

* degreesPerStep
* rotation speed

### Servo

* open angle
* close angle

### Routines

* movement distances
* timing
* grip strengths
* rotational amounts

---

# 🧪 Testing Instructions

### Test Motors Individually

```cpp
lin.moveRelative(5);
rail.moveSteps(200);
claw.rotateDegrees(120);
servo.open(150);
```

### Test Calibration

1. Set function → **CALIBRATE**
2. Press **START**

### Test Action Modes

1. Set mode → SNAPLINK or SCREW
2. Set function → EXECUTE
3. Press START

### Emergency Stop

STOP button must interrupt motion at all times.

---

# ⚠️ Safety Notes

* Always perform homing before running routines.
* STOP immediately disables all motors.
* Use proper current limit on TMC2209.
* Never operate without end-stops connected.
* Keep hands away from rotating axes.

---

# 🧱 How to Add a New Routine

1. Create two new files:

```
MyRoutine.h
MyRoutine.cpp
```

2. Forward declare dependencies in `.h`:

```cpp
class LinearActuator;
class RailStepper;
class ClawStepper;
class ServoClaw;
```

3. Implement logic in `.cpp`.

4. Register in `Decapper.ino`:

```cpp
if (panel.mode() == PanelIO::MODE_MYMODE) {
    MyRoutine::run(lin, rail, claw, servo);
}
```

---

# 👥 Team Roles (Recommended)

| Person     | Role                            |
| ---------- | ------------------------------- |
|  A | Motor tuning & homing           |
|  B | Screw & Snaplink logic          |
|  C | PanelIO & main control          |
|  D | Wiring, connectors, control box |

---

# 📜 License

MIT License — free to modify, use, and extend.

---
