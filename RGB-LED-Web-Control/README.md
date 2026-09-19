# 🚗 ESP32 Smart Lighting System for Toy Car

A custom lighting system built to repair and upgrade the damaged headlight system of my brother's Lemogene toy car.

The original headlight system had stopped working, so I designed a replacement lighting controller using an **ESP32-C3 SuperMini**.

Instead of making it just a replacement, I added **Wi-Fi-based web control**, allowing the lights to be controlled from a phone or computer.

---

## 🎯 Project Motivation

My brother's toy car had a damaged headlight system.

Rather than simply replacing the damaged circuit, I decided to build my own lighting controller using an ESP32-C3.

The goal was to:

- Restore the car's lighting
- Add RGB color control
- Add brightness adjustment
- Control the lights wirelessly
- Add a flashing light mode
- Make the system easy to control from a phone

This turned a simple repair into an **ESP32-based IoT/embedded project**.

---

## ✨ Features

### RGB Headlights

- Select any RGB color
- Adjust brightness
- Turn RGB lighting ON/OFF
- Supports a common-anode RGB LED

### Additional LED

- ON
- OFF
- FLASH mode

### Wireless Control

The ESP32 creates its own Wi-Fi network and hosts a control webpage.

No external Wi-Fi router or internet connection is required.

### Persistent Settings

The ESP32 stores lighting settings using the `Preferences` library.

The saved settings include:

- RGB color
- Brightness
- RGB ON/OFF state
- Flash mode

This allows settings to survive an ESP32 restart.

---

## 🧰 Hardware

- ESP32-C3 SuperMini
- Common-anode RGB LED
- Additional LED
- Current-limiting resistors
- Toy car lighting circuit
- Connecting wires

---

## 🔌 ESP32-C3 Pin Configuration

| Function | GPIO |
|---|---:|
| Red | GPIO 3 |
| Green | GPIO 4 |
| Blue | GPIO 5 |
| Additional LED | GPIO 10 |

> The RGB LED is a common-anode type, so the PWM output is inverted.

---

## 🌐 Web Control

The ESP32 operates as a Wi-Fi Access Point.

After powering the system:

1. Connect a phone or computer to the ESP32's Wi-Fi network.
2. Open a web browser.
3. Navigate to:

`http://192.168.4.1`

The control panel provides:

- 🎨 Color picker
- ☀️ Brightness slider
- RGB ON/OFF
- Additional LED ON/OFF
- Flash mode

---

## ⚙️ How It Works

```text
              Phone / Computer
                     │
                  Wi-Fi
                     │
                     ▼
              ┌─────────────┐
              │   ESP32-C3  │
              │  Web Server │
              └──────┬──────┘
                     │
          ┌──────────┴──────────┐
          │                     │
       PWM GPIO              GPIO 10
          │                     │
          ▼                     ▼
     RGB Headlight         Extra LED
