# PX4 Air Data Boom (FTB-1) by Swiss-Gear Driver

## Overview
This repository contains a custom driver for integrating an Air Data Boom sensor with the PX4 Autopilot. The driver enables communication between the PX4 firmware and the sensor via UART, allowing it to read and process air data. The sensor data is then used for flight control and can be displayed in real-time via QGroundControl.

## Features
- Communicates with the Air Data Boom sensor via UART.
- Reads airspeed, angle of attack, and other crucial flight parameters.
- Publishes sensor data to uORB topics for use in PX4 flight control.

## Setup Instructions

### 1. Clone the PX4-Autopilot Repository
To begin, clone the PX4-Autopilot repository:
```bash
git clone https://github.com/PX4/PX4-Autopilot.git --recursive
cd PX4-Autopilot
```

### 2. Create a New Folder for the Driver
```bash
mkdir PX4-Autopilot/src/drivers/air_data_boom
```

### 3. Copy the Driver Files
```bash
cp path-to-this-repo/* PX4-Autopilot/src/drivers/air_data_boom/
```

### 4. Compile the PX4 Firmware
