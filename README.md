# 🧪 Automated Test Tube Decapper

> A low-cost Arduino-based system to automatically open laboratory test tubes (snap-link and screw-cap types) using servo and stepper actuation.

---

## 📁 Repository Overview

This repository contains:

automated-decapper/
├── main/
│ └── decapper.ino # Main Arduino sketch
├── docs/
│ └── wiring-diagram.png # Hardware wiring overview
└── README.md


---

## 🧠 Project Description

This project automates the process of removing both **snap-link** and **screw-type** caps from laboratory sample tubes.  
It integrates:
- A **servo-driven claw** for gripping caps  
- A **rotational stepper** for twisting motion  
- A **linear stepper actuator** for vertical motion  
- A **hall-effect sensor** for homing calibration  
- A **toggle switch** to select between cap types  

---

## 🧰 Bill of Materials (BOM)

| Component                      | Qty | Description |
|--------------------------------|-----|-------------|
| Arduino Uno (ATmega328P)       | 1 | Main controller |
| Stepper Motor 28BYJ            | 2 | One for rotation, one for linear actuator |
| Stepper Driver A4988 / DRV8825 | 2 | Current-limited drivers |
| Servo Motor (e.g. MG996R)      | 1 | Controls claw open/close |
| Hall Effect Sensor             | 1 | Detects home position |
| Toggle Switch                  | 1 | Selects Snaplink / Screwcap mode |
| 12 V 4 A Power Supply          | 1 | Main power input |
| LM2596 Buck Converter          | 1 | Steps 12 V → 5 V for logic + servo |
| Wires, Breadboard / PCB        | — | As required |

---

## 🧱 Hardware Pin Mapping

| Function | Arduino Pin |
|-----------|--------------|
| Servo Signal | D9 |
| Rotation Stepper STEP | D2 |
| Rotation Stepper DIR | D3 |
| Linear Stepper STEP | D4 |
| Linear Stepper DIR | D5 |
| Hall Sensor Input | D6 |
| Mode Toggle Input | D7 |
| Stepper Enable (optional) | D8 |
| 5 V / GND | Shared with servo and sensors |

---

## ⚡ Power Wiring
```
[12 V 4 A Adapter]
│
├──> [Stepper Drivers] → Steppers (Rotation + Linear)
└──> [LM2596 Buck Converter → 5 V 3 A]
├──> Arduino Nano
├──> Servo Motor
└──> Hall Sensor + Toggle
```
```
⚠️ All GNDs must be connected together.
⚠️ Never power the servo directly from the Nano’s 5 V pin.
```

---

## 🧩 Software Setup

### 1️⃣ Install Arduino IDE

Download from [https://www.arduino.cc/en/software](https://www.arduino.cc/en/software)  
and install for your OS (Windows / macOS / Linux).

### 2️⃣ Install Libraries

Go to **Sketch → Include Library → Manage Libraries**  
Search and install:
- **AccelStepper** (by Mike McCauley)
- **Servo** (by Arduino)

### 3️⃣ Connect Board

- Plug in your **Arduino Nano** via USB.  
- If it’s a clone, install the **CH340 driver** (Google “CH340 Arduino driver” for your OS).

### 4️⃣ Select Board and Port

In the IDE:

Tools → Board → Arduino Nano
Tools → Processor → ATmega328P (Old Bootloader)
Tools → Port → [Your COM Port]


---

## 🧑‍💻 GitHub Setup (for your team)

### Step 1: Clone repository

Run this in a terminal:
```bash
git clone https://github.com/<your-username>/automated-decapper.git
cd automated-decapper
```
### Step 2: Add your Arduino sketch

Please ensure you are working on the dev branch, instead of the main.
Make changes to the files into a folder named main:
```
automated-decapper/
└── main/
    └── main.ino
```
### Step 3: Push changes to GitHub
```
git add .
git commit -m "feat: initial decapper firmware and documentation"
git push origin main
```
### 🧠 Running the Firmware
```
=== Decapper System Initializing ===
Homing linear actuator...
Homing rotation...
System Ready.
Mode: Screwcap
Starting Screwcap Routine...
Screwcap unscrewing complete.
```

### 🔧 Calibration Parameters
| Variable                  | Purpose                               |
| ------------------------- | ------------------------------------- |
| `screwcap_height`         | Z-height for screw cap engagement     |
| `snaplink_height`         | Z-height for snap cap engagement      |
| `unscrew_lift_distance`   | Distance claw lifts while unscrewing  |
| `final_lift_distance`     | Retraction distance after cap removal |
| `CLAW_OPEN`, `CLAW_CLOSE` | Servo angles for open/close           |

Tune these experimentally using Serial.print() readings.

### 💡 Operating Sequence
🧷 Screw-Cap Mode

    Claw opens

    Moves down to screwcap height

    Claw closes

    Rotates + lifts simultaneously to unscrew

    Lifts clear, releases cap, returns home

⚙️ Snap-Link Mode

    Claw opens

    Moves down to snaplink height

    Claw closes

    Performs one full rotation (to snap)

    Lifts clear, releases cap, returns home

### 🔋 Power Guidelines
| Rail             | Voltage | Continuous Current | Notes                    |
| ---------------- | ------- | ------------------ | ------------------------ |
| **Steppers**     | 12 V    | 3 A total          | Shared 12 V 4 A adapter  |
| **Servo + Nano** | 5 V     | 2–3 A              | Buck-converted from 12 V |
| **Total**        | —       | ~4 A               | ≈ 50 W total system draw |

### 🧩 Repository Usage Summary
| Command                     | Description                     |
| --------------------------- | ------------------------------- |
| `git clone <repo>`          | Clone the project               |
| `git add .`                 | Stage all new or modified files |
| `git commit -m "<message>"` | Save changes locally            |
| `git push`                  | Upload to GitHub                |
| `git pull`                  | Get the latest updates          |

🧱 Suggested Directory Structure
```
automated-decapper/
├── src/
│   └── decapper.ino
├── docs/
│   ├── wiring-diagram.png
│   └── calibration-table.md
├── LICENSE
└── README.md
```
### 📜 License

Released under the MIT License — see LICENSE.
👤 Author
```
Allen Prasad
Mechatronics B.Eng., THWS Würzburg-Schweinfurt
Supervisor: Prof. Dr. Lisa Kiesewetter
Industrial Project Topic 4 — Automated Decapping of Sample Tubes
```
    🧠 Tip: For reproducible setups, tag your releases using
    git tag -a v1.0.0 -m "first working prototype"
    git push origin v1.0.0