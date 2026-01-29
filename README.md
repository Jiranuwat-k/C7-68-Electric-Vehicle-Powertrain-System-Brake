# ESP32 ESC Drive & Water Pressure Sensor Setup

This project implements an Electronic Speed Controller (ESC) driver for a Brushless DC motor using an ESP32, integrated with an analog Water Pressure Sensor monitoring system.

## Features

- **ESC Auto-Arming**: automatically sends the calibration signal (Min PWM) at startup to initialize the ESC.
- **Serial Motor Control**: Adjust motor speed dynamically by sending PWM values via Serial Monitor.
- **Pressure Monitoring Display**: Real-time display of ADC value, Voltage, and calculated Pressure.
- **Noise Filtering**: Implements a moving average filter (20 samples) for stable sensor readings.

## Hardware Requirements

- **Microcontroller**: ESP32 (Tested on ESP32 DEVKIT V1)
- **Actuator**: Brushless DC Motor with ESC
- **Sensor**: Analog Water Pressure Sensor (e.g., DFRobot Gravity Series or compatible 0-5V/0-3.3V sensors)

## Wiring Map

| Component  | Pin Function | ESP32 GPIO  | Note                                         |
| ---------- | ------------ | ----------- | -------------------------------------------- |
| **ESC**    | Signal (PWM) | **GPIO 16** | Connect ESC Ground to ESP32 Ground           |
| **Sensor** | Analog Out   | **GPIO 32** | Ensure output voltage does not exceed 3.3V\* |

_(If using a 5V sensor, ensure a Voltage Divider is used or the sensor output is within ESP32 safe limits. The current code includes a calculation for a voltage divider factor `31.0/20.0`)_

## Installation & Setup

### 1. Install Software

If you haven't already, you need to install the development environment:

1. **Visual Studio Code**: Download and install from [code.visualstudio.com](https://code.visualstudio.com/).
2. **PlatformIO Extension**:
   - Open VS Code.
   - Go to the **Extensions** view (Ctrl+Shift+X).
   - Search for **"PlatformIO IDE"**.
   - Click **Install**.

### 2. Clone the Repository

Open a terminal (Git Bash or Command Prompt) and run:

```sh
git clone https://github.com/Jiranuwat-k/C7-68-Electric-Vehicle-Powertrain-System-Brake.git
cd ESP32_ESCDrive
```

### 3. Open in PlatformIO

1. Open VS Code.
2. Click **File > Open Folder**.
3. Select the `ESP32_ESCDrive` folder you just cloned.
4. Wait for PlatformIO to initialize (it will automatically download necessary libraries like `ESP32Servo`).

### 4. Upload Code

1. Connect your ESP32 board via USB.
2. Click the **PlatformIO Alien Icon** on the left sidebar.
3. Under **Project Tasks**, expand `env:main`.
4. Click **Upload**.
5. After a successful upload, click **Monitor** to see the Serial output.

## Usage Guide

1. **Safety First**: Remove any propellers from the motor before powering up.
2. **Start Up**:
   - Open the **Serial Monitor** (Baud Rate: **115200**).
   - Reset the board.
   - You will see the message `Arming ESC...`.
   - Wait for `ESC Armed & Ready`.

3. **Control Motor**:
   - Type a PWM value between **1000** and **2000** in the input field and press Enter.
   - **`1100`**: Stop / Idle
   - **`1500`**: Half Speed
   - **`2000`**: Full Speed

4. **Monitor Pressure**:
   - The Serial Monitor will output pressure sensor data every 500ms.
   - Data format: `ADC Value`, `Voltage (Vin)`, `Pressure`.

## Calibration

To calibrate the pressure sensor, adjust the constants at the top of `src/main.cpp`:

```cpp
const float OffSet = 0.320;   // Voltage reading when pressure is 0
const float Slope = 400;      // Conversion slope (depends on sensor spec)
const float SpanFactor = 1;   // Fine-tune multiplier for accuracy
```

**How to Calibrate Zero:**

1. Ensure the system has **0 pressure** (pump off, no water flow).
2. Read the `Vin` value displayed in the Serial Monitor.
3. Update the `OffSet` variable in the code with this value.
4. Re-upload the code.
