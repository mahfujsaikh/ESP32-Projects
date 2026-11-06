SPERM RUN ARCADE - STANDALONE ESP-IDF PROJECT

This standalone game's own display header is: main/LGFX_ESP32S3.h
Game source: main/main.cpp
Main component file: main/CMakeLists.txt

The source already uses:
    #include "LGFX_ESP32S3.h"
The main component has INCLUDE_DIRS "." and
REQUIRES LovyanGFX esp_driver_gpio esp_timer.
There are no source includes or build paths into any other personal project.

External dependency:
- Install the LovyanGFX ESP-IDF component (usually under components/LovyanGFX).
- LovyanGFX itself is not included in this repository snapshot.

Original notes specified ESP-IDF 6.0.2.
To build from the game directory:
    idf.py set-target esp32s3
    idf.py build
Flash (replace COMx with your serial port):
    idf.py -p COMx flash monitor

ST7789 DISPLAY:
    SCLK=GPIO12, MOSI=GPIO11, MISO=GPIO13,
    DC=GPIO9, CS=GPIO10, RST=GPIO46.
    LCD panel configured 240 x 320 (game runs at rotation 1: 320 x 240).
    Confirm TFT VCC and backlight requirements; these are not recorded.
    IMPORTANT: GPIO46 is an ESP32-S3 strapping pin; verify compatibility.

ACTIVE-LOW BUTTONS (each GPIO connects to GND when pressed):
    UP=GPIO1, DOWN=GPIO2, BOOST=GPIO8,
    PAUSE=GPIO14, MENU=GPIO18.
    Code enables internal pull-ups.

GAMEPLAY:
- MENU starts/restarts, UP/DOWN steer, BOOST increases scrolling speed,
  PAUSE toggles pause/resume.
- Three obstacles with randomized gaps, 3 lives, score and best score.
- Best score is RAM-only; it is lost when firmware restarts.
- The source is supplied, but compilation and hardware tests are not
  verified for this repository snapshot.

SAFETY:
- Flashing replaces currently installed application firmware.
- Verify all GPIOs, power rails, TFT module, and common GND before wiring.
