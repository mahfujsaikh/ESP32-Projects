# ⚡ ESP32 Smart Switch

An ESP32-based IoT smart switch designed to control multiple electrical loads through Blynk, with additional PIR-based automatic control and state persistence.

## 📌 Overview

This project uses an ESP32 with a 4-channel relay module to provide wireless control of connected loads through the Blynk IoT platform.

A PIR motion sensor is also integrated into the system. When motion is detected, Relay 1 can be automatically activated and switched off after a configurable timeout.

The ESP32 also stores the relay states using its non-volatile Preferences storage, allowing the system to restore the previous states after a restart.

## ✨ Features

- 📱 Control 4 relays through Blynk
- 🔌 Four independent relay outputs
- 🚶 PIR-based automatic switching
- ⏱️ Configurable motion timeout
- 💾 Persistent relay states using ESP32 Preferences
- 🔄 Restore relay states after reboot
- 📡 Wi-Fi connectivity
- 🔁 Synchronization between ESP32 and Blynk after reconnection
- 💡 Real-time PIR status indication in Blynk

## 🧰 Hardware

| Component | Purpose |
|---|---|
| ESP32 | Main controller |
| 4-Channel Relay Module | Controls electrical loads |
| PIR Motion Sensor | Detects movement |
| Wi-Fi Network | Internet connectivity |
| Blynk | Remote control and monitoring |

## 🔌 Pin Configuration

| Component | ESP32 GPIO |
|---|---:|
| Relay 1 | GPIO 23 |
| Relay 2 | GPIO 22 |
| Relay 3 | GPIO 21 |
| Relay 4 | GPIO 19 |
| PIR Sensor | GPIO 33 |

The relay module used in this project is configured as **active LOW**.

## 📱 Blynk Controls

The project uses the following virtual pins:

| Virtual Pin | Function |
|---|---|
| V0 | Relay 1 |
| V1 | Relay 2 |
| V2 | Relay 3 |
| V3 | Relay 4 |
| V10 | Motion control enable/disable |
| V11 | Motion timeout |
| V12 | PIR status indicator |

## 🚶 Motion-Based Automation

When motion control is enabled:

1. The PIR sensor detects motion.
2. Relay 1 is automatically switched ON.
3. The motion timer is reset whenever motion is detected.
4. After the configured timeout without motion, Relay 1 is switched OFF.
5. The Blynk interface is updated accordingly.

## 💾 State Persistence

Relay states and the motion-control setting are stored using the ESP32 `Preferences` library.

This allows the controller to restore the saved states after restarting.

## 🧠 Software

- Arduino framework
- C++
- ESP32 Arduino Core
- Blynk
- Preferences library

## 🔐 Configuration

Before uploading the code, configure:

- Blynk Template ID
- Blynk Template Name
- Blynk Auth Token
- Wi-Fi SSID
- Wi-Fi password

**Do not publish your Blynk Auth Token or Wi-Fi credentials in a public repository.**

A safe approach is to replace them with placeholders before committing the code.

## 🚀 Possible Improvements

Future versions could include:

- Energy/power monitoring
- Scheduling and timers
- Manual physical switches
- MQTT support
- Local web interface
- Over-the-air firmware updates
- Temperature and environmental monitoring

---

## 📄 Project Status

A personal ESP32 IoT project developed while experimenting with wireless control, relay automation, sensors and embedded systems.
