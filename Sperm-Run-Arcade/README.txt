SPERM RUN ARCADE - STANDALONE ESP-IDF PROJECT

IMPORTANT
- This is a standalone game project; it does not require source files from any other personal project.
- Flashing this project replaces the firmware currently running on the ESP32-S3.
- The restored main/main.cpp should include the game-local LGFX_ESP32S3.h and use the LovyanGFX ESP-IDF component.
- The included game-local header is located at:
    main/LGFX_ESP32S3.h
- In main/main.cpp, use: #include "LGFX_ESP32S3.h"
- Do not include the header through another project's directory.
- Install LovyanGFX as an ESP-IDF component in components/LovyanGFX (or configure an equivalent component dependency).
- Note: main/main.cpp and the LovyanGFX component are not in this snapshot, so it is not buildable yet.
- Verify that the header declares class LGFX and matches the physical ST7789 wiring.
- Build using ESP-IDF 6.0.2 from this folder: idf.py set-target esp32s3; idf.py build
- Flash with: idf.py -p COMx flash monitor

DISPLAY PINS recalled from the existing board setup:
ST7789 SCLK=GPIO12, MOSI=GPIO11, MISO=GPIO13, DC=GPIO9, CS=GPIO10, RST=GPIO46.

BUTTONS (each connects GPIO to GND):
UP=GPIO1, DOWN=GPIO2, BOOST=GPIO8, PAUSE=GPIO14, MENU=GPIO18.
These avoid known display, microphone (GPIO4/5/6), and speaker (GPIO15/16/17) pins.
Verify all selected GPIOs are exposed and free on your particular ESP32-S3 board before wiring.
