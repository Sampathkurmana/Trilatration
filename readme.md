# Wi‑Fi RSSI-Based Triangulation Using ESP32
 
## Overview

This project implements a **real-time mobile Tracking with ISP towers Using Trilatration** using:

* ESP32 Dev Modules
* Wi‑Fi RSSI (Received Signal Strength Indicator)
* Trilateration algorithms
* Firebase Realtime Database
* Web Dashboard Visualization

The system estimates the approximate indoor position of a target mobile hotspot by collecting RSSI values from multiple ESP32 localization towers and processing them through distance estimation and trilateration techniques.

---

## Features

* Real-time RSSI monitoring
* Indoor localization using trilateration
* Firebase cloud synchronization
* ESP32-based wireless sensing
* Web dashboard visualization
* Distance estimation using RSSI path loss model
* Kalman filtering for RSSI stabilization
* Dynamic localization tracking
* Signal analytics and device monitoring

---

# System Architecture

```text
Mobile Hotspot (Target)
        │
        ▼
Wi‑Fi Signal Transmission
        │
 ┌───────────────┬───────────────┬───────────────┐
 ▼               ▼               ▼
ESP32 A        ESP32 B        ESP32 C
 │               │               │
 └───────────────┴───────────────┘
                 │
                 ▼
      Firebase Realtime Database
                 │
                 ▼
          Web Dashboard
                 │
                 ▼
     Estimated Indoor Position
```

---

# Hardware Components

| Component        | Purpose                               |
| ---------------- | ------------------------------------- |
| ESP32 Dev Module | RSSI scanning and Wi‑Fi communication |
| Mobile Hotspot   | Target signal source                  |
| USB Interface    | Programming and debugging             |
| Power Supply     | ESP32 power source                    |
| Wi‑Fi Network    | Communication infrastructure          |

---

# Software Stack

| Technology                 | Purpose                        |
| -------------------------- | ------------------------------ |
| Arduino IDE                | ESP32 programming              |
| Firebase Realtime Database | Cloud synchronization          |
| HTML                       | Dashboard structure            |
| CSS / Tailwind CSS         | Dashboard styling              |
| JavaScript                 | Real-time updates              |
| PHP                        | Backend processing             |
| Chart.js                   | Signal analytics visualization |
| React.js                   | Frontend dashboard             |

---

# Working Principle

## Step 1 — RSSI Scanning

Each ESP32 scans nearby Wi‑Fi networks and detects the target hotspot using its SSID.

## Step 2 — RSSI Filtering

Kalman filtering is applied to reduce RSSI fluctuations caused by:

* Environmental interference
* Obstacles
* Signal reflection
* Multipath propagation

## Step 3 — Distance Estimation

Filtered RSSI values are converted into estimated distances using the RSSI path loss model.

### RSSI Path Loss Equation

```math
RSSI = -10n log10(d) + A
```

### Distance Estimation Formula

```math
d = 10^((A - RSSI)/(10n))
```

Where:

| Symbol | Meaning                  |
| ------ | ------------------------ |
| RSSI   | Received signal strength |
| d      | Estimated distance       |
| A      | RSSI at 1 meter          |
| n      | Path loss exponent       |

---

## Step 4 — Trilateration

Three ESP32 towers are placed at fixed coordinates:

| Tower   | Coordinates |
| ------- | ----------- |
| Tower A | (0,0)       |
| Tower B | (4,0)       |
| Tower C | (0,3)       |

The intersection point of the estimated distance circles determines the approximate target location.

### Trilateration Equations

```math
(x-0)^2 + (y-0)^2 = r_1^2
```

```math
(x-4)^2 + (y-0)^2 = r_2^2
```

```math
(x-0)^2 + (y-3)^2 = r_3^2
```

---

## Step 5 — Firebase Synchronization

ESP32 nodes upload:

* RSSI values
* Estimated distances
* Coordinates
* Timestamps

into Firebase Realtime Database.

---

## Step 6 — Dashboard Visualization

The dashboard displays:

* Live RSSI monitoring
* Signal analytics
* Tower status
* Distance estimation
* Trilateration map
* Real-time target coordinates

---

# Tower Arrangement

```text
 Tower C (0,3)
      ●
      │
      │
      │
      │
●────────────────────────●
Tower A (0,0)      Tower B (4,0)
```

---

# Firebase Database Structure

```json
{
  "nodes": {
    "ESP32_NODE_01": {
      "rssi": -52,
      "distance_m": 1.25,
      "distance_cm": 125,
      "timestamp": 1704123456
    },
    "ESP32_NODE_02": {
      "rssi": -50,
      "distance_m": 1.08,
      "distance_cm": 108,
      "timestamp": 1704123457
    },
    "ESP32_NODE_03": {
      "rssi": -48,
      "distance_m": 1.00,
      "distance_cm": 100,
      "timestamp": 1704123458
    }
  },
  "tower_positions": {
    "A": { "x": 0, "y": 0 },
    "B": { "x": 4, "y": 0 },
    "C": { "x": 2, "y": 3 }
  },
  "config": {
    "rssi": {
      "A": -42,
      "B": -42,
      "C": -42,
      "n": 2.5
    },
    "target_position": {
      "x": 2.0,
      "y": 1.5
    }
  }
}
```

---

# Calibration Parameters

| Parameter          | Value   |
| ------------------ | ------- |
| RSSI at 1 meter    | -42 dBm |
| Path Loss Exponent | 2.5     |
| Kalman Q           | 0.05    |
| Kalman R           | 0.5     |
| Kalman P           | 1.0     |

---

# Localization Workflow

```text
Start System
     ↓
Activate Hotspot
     ↓
ESP32 RSSI Scanning
     ↓
Kalman Filtering
     ↓
Distance Estimation
     ↓
Trilateration
     ↓
Coordinate Calculation
     ↓
Firebase Synchronization
     ↓
Dashboard Visualization
     ↓
Realtime Localization Output
```

---

# Advantages

* Low-cost implementation
* Real-time indoor localization
* Easy deployment
* Cloud-based monitoring
* IoT integration
* Scalable architecture
* Wireless communication support
* Dynamic dashboard visualization

---

# Applications

* Indoor navigation
* Smart surveillance systems
* Asset tracking
* Smart buildings
* IoT monitoring systems
* Security monitoring
* Warehouse management
* Human/device tracking

---

# Limitations

* RSSI fluctuations
* Signal interference
* Multipath propagation
* Environmental noise
* Lower accuracy compared to UWB systems

---

# Future Enhancements

* BLE integration
* AI-based prediction
* Mobile application development
* GPS + RSSI hybrid localization
* UWB integration
* Advanced filtering algorithms
* Improved localization accuracy

---
# Expected Localization Accuracy

The implemented system achieved:

* Approximate indoor localization
* Real-time target tracking
* Localization accuracy around **1–3 meters** in controlled environments

---

# Technologies Used

* ESP32
* Firebase
* Arduino IDE
* HTML
* CSS
* JavaScript
* PHP
* React.js
* Tailwind CSS
* Chart.js

---

# Conclusion

This project demonstrates a practical implementation of a low-cost Wi‑Fi RSSI-based indoor localization system using ESP32 localization nodes, trilateration algorithms, Firebase cloud synchronization, and real-time dashboard visualization.

The system successfully combines:

* IoT technologies
* Wireless communication
* Cloud databases
* RSSI signal processing
* Trilateration mathematics
* Web-based monitoring

to provide an efficient and scalable indoor positioning solution suitable for modern smart monitoring applications.

---

# Author

Developed as an IoT and Indoor Localization research/project implementation using ESP32 and Firebase realtime technologies.

---

# License

This project is intended for educational, research, and learning purposes.
