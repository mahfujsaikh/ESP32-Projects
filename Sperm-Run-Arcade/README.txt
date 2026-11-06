SPERM RUN ARCADE - STANDALONE ESP-IDF PROJECT

IMPORTANT
- This is a separate project, not a modification of the AI companion source.
- Flashing this project replaces the firmware currently running on the ESP32-S3.
- The main.cpp expects the existing board header LGFX_ESP32S3.h and LovyanGFX ESP-IDF component.
- Copy LGFX_ESP32S3.h from your existing board configuration into:
    main/boards/my-ai-companion/LGFX_ESP32S3.h
- Copy your LovyanGFX component directory into this project's components/LovyanGFX.
- Verify that the header declares class LGFX and matches the physical ST7789 wiring.
- Build using ESP-IDF 6.0.2 from this folder: idf.py set-target esp32s3; idf.py build
- Flash with: idf.py -p COMx flash monitor

DISPLAY PINS recalled from the existing board setup:
ST7789 SCLK=GPIO12, MOSI=GPIO11, MISO=GPIO13, DC=GPIO9, CS=GPIO10, RST=GPIO46.

BUTTONS (each connects GPIO to GND):
UP=GPIO1, DOWN=GPIO2, BOOST=GPIO8, PAUSE=GPIO14, MENU=GPIO18.
These avoid known display, microphone (GPIO4/5/6), and speaker (GPIO15/16/17) pins.
Verify all selected GPIOs are exposed and free on your particular ESP32-S3 board before wiring.
