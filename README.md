# VANGUARD – Neonatal Jaundice Screening Prototype

> **BME IGNITE HACKFEST 2026**

VANGUARD is an Arduino-based prototype designed to explore **non-invasive neonatal jaundice screening** using RGB color sensing.

The system uses a **TCS3200 RGB color sensor** to capture color information from the skin surface and processes the measured RGB values to estimate the degree of yellow coloration. The result is presented through an **OLED display** and a **three-level traffic-light indicator**, with an optional buzzer for alerts.

> **Important:** VANGUARD is a hackathon/research prototype and is **not a clinically validated medical diagnostic device**. The output should not be used to make medical decisions without appropriate clinical validation and professional medical assessment.

---

## Project Overview

Neonatal jaundice is a common condition in newborns caused by elevated bilirubin levels. Conventional bilirubin assessment may require clinical equipment or blood sampling.

VANGUARD explores whether an inexpensive RGB sensing system can provide a simple, non-invasive indication of skin yellowness that could potentially support early screening.

### Core idea

```text
Newborn skin
     │
     ▼
TCS3200 RGB Sensor
     │
     ▼
RGB Measurement
     │
     ▼
Yellowness / Threshold Processing
     │
     ├───────────────┐
     ▼               ▼
 OLED Display   Traffic-Light
                  Indicator
     │
     ▼
 Optional Buzzer Alert
```

---

# Features

- Non-invasive optical sensing approach
- TCS3200 RGB color sensor
- Arduino UNO based controller
- OLED visual output
- Three-level traffic-light indication
- Optional buzzer alert
- Calibration workflow using Python
- Baseline/reference-based measurement approach
- Portable prototype concept
- Designed for hackathon demonstration and future research development

---

# Hardware

| Component | Purpose |
|---|---|
| Arduino UNO | Main microcontroller |
| TCS3200 RGB Sensor | Measures RGB/color response |
| 0.96" OLED SSD1306 | Displays measurement/result |
| Traffic-Light Module | Provides low/mid/high visual indication |
| Buzzer | Provides audible alert |
| Push Button | Starts the measurement |
| White LEDs | Provides controlled illumination |
| Breadboard / Wiring | Prototype connections |
| Enclosure / Probe | Holds the sensing components |

---

# Software

- Arduino IDE
- Arduino C/C++
- Python 3
- PySerial
- Adafruit SSD1306 library
- Adafruit GFX library

---

# System Architecture

```text
                 ┌─────────────────────┐
                 │     TCS3200 RGB     │
                 │       Sensor        │
                 └──────────┬──────────┘
                            │
                            │ RGB data
                            ▼
                 ┌─────────────────────┐
                 │     Arduino UNO     │
                 │                     │
                 │ Sensor Processing   │
                 │ Calibration         │
                 │ Threshold Decision  │
                 └───────┬─────┬───────┘
                         │     │
              ┌──────────┘     └──────────┐
              ▼                           ▼
       ┌─────────────┐             ┌─────────────┐
       │ OLED Display│             │ Traffic LED │
       │ SSD1306     │             │   Module    │
       └─────────────┘             └──────┬──────┘
                                          │
                                          ▼
                                    ┌───────────┐
                                    │  Buzzer   │
                                    └───────────┘
```

---

# Working Principle

The TCS3200 sensor detects the intensity of different color components reflected from the target surface.

The system obtains:

```text
R = Red component
G = Green component
B = Blue component
```

These measurements are processed by the Arduino.

A yellowness-related metric can then be calculated and compared against experimentally selected thresholds.

One metric explored during development is:

```text
Yellowness = G - B / R
```

The exact mathematical model and threshold values are subject to calibration and validation.

The final prototype maps the processed measurement into three demonstration states:

```text
LOW
 │
 └── Green

MEDIUM
 │
 └── Yellow / Amber

HIGH
 │
 └── Red
```

---

# Measurement Workflow

```text
1. Power ON
       │
       ▼
2. Initialize OLED and sensor
       │
       ▼
3. Place sensing probe over target area
       │
       ▼
4. Press measurement button
       │
       ▼
5. Capture RGB readings
       │
       ▼
6. Process sensor data
       │
       ▼
7. Calculate measurement metric
       │
       ▼
8. Compare against thresholds
       │
       ▼
9. Display result on OLED
       │
       ▼
10. Activate corresponding traffic-light state
       │
       ▼
11. Optional buzzer alert
```

---

# Calibration

Calibration is an important part of the VANGUARD development process.

The project includes a Python-based calibration tool:

```text
Calibration/calibration_tool.py
```

The calibration workflow is intended to help collect sensor readings and determine suitable demonstration thresholds.

### Calibration workflow

```text
Arduino
   │
   ▼
Baseline measurement
   │
   ▼
Test/reference measurement
   │
   ▼
Serial data
   │
   ▼
Python calibration tool
   │
   ▼
Data analysis
   │
   ▼
Candidate threshold values
   │
   ▼
Arduino firmware
```

The calibration tool communicates with the Arduino through a serial connection using Python and PySerial.

---

# Project Development

The project was developed in stages.

### Stage 1 – Sensor testing

The TCS3200 sensor was first tested independently to verify that RGB readings were being received correctly.

```text
TCS3200 → Arduino UNO → Serial Monitor
```

### Stage 2 – Display integration

The OLED was integrated to provide a visual interface for the user.

```text
Sensor → Arduino → OLED
```

### Stage 3 – Result indication

A traffic-light module was added to provide an easy-to-understand visual classification.

```text
Green  → Low
Yellow → Medium
Red    → High
```

### Stage 4 – Calibration

A Python calibration tool was developed to collect measurements and assist in selecting threshold values.

### Stage 5 – Prototype integration

The sensor, illumination system, OLED, traffic-light module, button, and buzzer were integrated into the VANGUARD prototype.

---

# Repository Structure

```text
VANGUARD-Neonatal-Jaundice-Screener/
│
├── Arduino/
│   ├── sensor_test/
│   │   └── sensor_test.ino
│   │
│   └── vanguard_screener/
│       └── vanguard_screener.ino
│
├── Calibration/
│   └── calibration_tool.py
│
├── Documentation/
│   ├── block_diagram.png
│   ├── circuit_diagram.png
│   └── project_photos/
│
├── README.md
├── LICENSE
└── .gitignore
```

---

# Example Output

The prototype uses three demonstration states.

### 🟢 Low

The system detects a value below the lower demonstration threshold.

OLED example:

```text
VANGUARD

RESULT: LOW

STATUS: GREEN
```

### 🟡 Medium

The measurement falls within the intermediate demonstration range.

OLED example:

```text
VANGUARD

RESULT: MEDIUM

STATUS: AMBER
```

A single buzzer alert can be used for this state in the prototype.

### 🔴 High

The measurement exceeds the upper demonstration threshold.

OLED example:

```text
VANGUARD

RESULT: HIGH

STATUS: RED
```

---

# Safety and Clinical Disclaimer

VANGUARD is an experimental prototype developed for educational, hackathon, and research purposes.

The system has **not been established as a clinically validated bilirubin measurement device**.

RGB skin-color measurements can be affected by several factors, including:

- Ambient light
- Sensor distance
- Sensor angle
- Skin characteristics
- Illumination intensity
- Sensor calibration
- Surface properties
- Hardware variation

Therefore, the prototype output should not be interpreted as a medical diagnosis or as a replacement for clinically accepted bilirubin measurement methods.

Clinical use would require appropriate research, validation, ethical oversight, comparison against accepted clinical measurements, and regulatory consideration.

---

# Limitations

Current prototype limitations include:

- RGB color is not equivalent to direct bilirubin measurement.
- Environmental lighting can affect measurements.
- Sensor positioning can affect readings.
- Skin color variation requires careful calibration.
- Thresholds require systematic validation.
- The prototype has not undergone clinical validation.
- The current system is intended for demonstration/research rather than diagnosis.

---

# Future Improvements

Potential future development includes:

- Controlled optical illumination
- Improved probe design
- Better sensor-distance control
- Automated calibration
- Larger datasets
- Statistical model development
- Machine-learning-based classification
- Comparison against clinically measured bilirubin values
- Improved enclosure
- Battery-powered operation
- Data logging
- Bluetooth/Wi-Fi connectivity
- Mobile application
- Clinical validation studies

---

# Hackathon Project

**Event:** BME IGNITE HACKFEST 2026

**Project:** VANGUARD

**Domain:** Biomedical Engineering / Healthcare Technology

**Prototype Platform:** Arduino UNO

The project demonstrates how low-cost embedded electronics and optical sensing can be explored for healthcare-oriented screening applications.

---

# Team

Developed as a collaborative hackathon project.

Team members can be added here:

```text
- Name – Hardware / Embedded Systems
- Name – Software / Calibration
- Name – Research / Documentation
- Name – Design / Prototype

This repository documents an experimental prototype.

The information provided here is for educational and research purposes only and does not constitute medical advice, diagnosis, or treatment.
