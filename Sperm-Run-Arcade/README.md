# Sperm Run Arcade — ESP32-S3 + ST7789 TFT

An experimental **ESP32-S3 arcade game** designed for a physical TFT display and push-buttons. This is a standalone hobby embedded project with its own local display configuration.

> [!IMPORTANT]
> **Documentation / partial-source snapshot.** The files supplied for this upload contain the ESP-IDF root `CMakeLists.txt`, the LovyanGFX board configuration, and original setup notes. **They do not include the gameplay implementation (`main/main.cpp`) or the LovyanGFX component source. This folder is not yet a complete, buildable game.** The README describes the recovered configuration faithfully, rather than inventing missing code or gameplay behavior.

## Hardware at a glance

| Item | Recorded configuration |
| --- | --- |
| Controller | ESP32-S3 (exact development-board model not specified) |
| Display | ST7789 SPI TFT, configured for **240 × 320** pixels |
| Display library | [LovyanGFX](https://github.com/lovyan03/LovyanGFX) |
| Firmware system | ESP-IDF; **6.0.2** specified in the original notes |
| Display SPI peripheral | `SPI2_HOST`, SPI mode 0 |
| Configured SPI clock | 40 MHz write / 16 MHz read |
| Input method | Five push-buttons, each connecting a GPIO to GND |

The ST7789 panel and SPI settings above come directly from `main/LGFX_ESP32S3.h`. The control assignments come from the supplied `README.txt`, **not** from source-code inspection.

## GPIO / pin configuration

### ST7789 display

| Display pin / signal | ESP32-S3 GPIO | Purpose |
| --- | ---: | --- |
| `SCLK` / `SCK` | **GPIO12** | SPI clock |
| `MOSI` / `SDA` / `DIN` | **GPIO11** | SPI data out to display |
| `MISO` / `SDO` | **GPIO13** | Configured SPI read line, if the panel exposes it |
| `DC` / `A0` | **GPIO9** | Data/command selection |
| `CS` | **GPIO10** | Chip select |
| `RST` / `RESET` | **GPIO46** | Display reset in the supplied config |
| `VCC` | **Not specified** | Check your exact TFT module voltage requirements |
| `GND` | **GND** | Common electrical ground |
| `BL` / `LED` | **Not specified** | Backlight wiring/control depends on the module |

**Important:** GPIO46 is an **ESP32-S3 strapping pin** and affects boot behavior. The stored config assigns it to display RESET, but that connection must be reviewed for your specific board/panel before rebuilding. If a redesigned build needs a different safe reset GPIO, change both the wiring and `cfg.pin_rst` in the header. Do not assume GPIO46 is universally available or that every TFT can be wired directly to it.

References: [Espressif ESP32-S3 hardware design guidelines](https://docs.espressif.com/projects/esp-hardware-design-guidelines/en/latest/esp32s3/schematic-checklist.html) and [ESP-IDF GPIO documentation](https://docs.espressif.com/projects/esp-idf/en/latest/esp32s3/api-reference/peripherals/gpio.html).

### Push-button controls

| Button label | ESP32-S3 GPIO | Physical connection |
| --- | ---: | --- |
| **UP** | **GPIO1** | GPIO1 ↔ momentary button ↔ GND |
| **DOWN** | **GPIO2** | GPIO2 ↔ momentary button ↔ GND |
| **BOOST** | **GPIO8** | GPIO8 ↔ momentary button ↔ GND |
| **PAUSE** | **GPIO14** | GPIO14 ↔ momentary button ↔ GND |
| **MENU** | **GPIO18** | GPIO18 ↔ momentary button ↔ GND |

Every button is wired **between the named GPIO and GND**, not to 3.3 V. This is an intended active-low input arrangement: when implementing/restoring the firmware, enable suitable pull-ups and handle button debounce. The missing `main.cpp` means the exact input-handling code and whether the named actions function in gameplay cannot yet be verified.

### Quick wiring sketch

    ESP32-S3                   ST7789 TFT
    --------                   ----------
    GPIO12  -----------------> SCLK
    GPIO11  -----------------> MOSI/SDA
    GPIO13  <----------------- MISO/SDO (only if present)
    GPIO9   -----------------> DC/A0
    GPIO10  -----------------> CS
    GPIO46  -----------------> RST  (BOOT-STRAP CAUTION)
    GND     ------------------ GND
    Supply  ------------------ VCC  (verify module requirements)
                               BL   (not recorded)

    GPIO1  ----[ UP button    ]---- GND
    GPIO2  ----[ DOWN button  ]---- GND
    GPIO8  ----[ BOOST button ]---- GND
    GPIO14 ----[ PAUSE button ]---- GND
    GPIO18 ----[ MENU button  ]---- GND

The earlier hardware configuration reserved **GPIO4/5/6** for microphone wiring and **GPIO15/16/17** for speaker wiring. Those pins are not used by this game's documented display or buttons; check for conflicts before assigning them to other devices.

**Electrical checks:** Share ground between the ESP32-S3 and TFT; verify the TFT module's power and logic levels, any onboard regulator/level shifters, the backlight connection, and the actual exposed GPIO pins. Do not connect GPIO inputs directly to 5 V.

## Interesting design points

- **Dedicated physical controls:** five labelled controls including separate BOOST, PAUSE and MENU inputs in the *wiring plan*. Their exact behavior must be confirmed against the missing game code.
- **Colour TFT graphics stack:** an ST7789 display driven through the LovyanGFX abstraction rather than a text-only display.
- **High-speed configured SPI bus:** `SPI2_HOST` with 40 MHz write clock, SPI mode 0, and DMA configured as `SPI_DMA_CH_AUTO`.
- **Standalone ESP-IDF firmware layout:** a dedicated project that can be flashed independently of other firmware.
- **Self-contained display definition:** `main/LGFX_ESP32S3.h` contains the `LGFX` class and all saved display pin, SPI bus, and 240 × 320 panel settings; no header is fetched from any other personal project.

No claims are made here about levels, obstacles, scores, graphics effects, collision logic, or saved high scores: **those details cannot be verified without `main.cpp`.**

## Files in this repository snapshot

    Sperm-Run-Arcade/
    ├── README.md                        # This guide
    ├── README.txt                       # Original supplied setup notes
    ├── CMakeLists.txt                    # Original ESP-IDF project entry
    ├── .gitignore
    └── main/
        ├── CMakeLists.txt                # ESP-IDF main-component declaration
        └── LGFX_ESP32S3.h               # Standalone LovyanGFX ST7789 pin setup

**Still needed for a functioning clone:**

- `main/main.cpp` — the actual game loop, input handling, rendering and gameplay;
- `components/LovyanGFX/` or an equivalent correctly configured ESP-IDF LovyanGFX dependency.

Some platforms or versions might need additional ESP-IDF configuration or component metadata; that cannot be determined from the three supplied files.

## Standalone display-header usage

This project keeps its **own copy** of the TFT configuration at `main/LGFX_ESP32S3.h`, beside the planned game source file. It has no source-level or folder dependency on any other personal firmware project.

When restoring `main/main.cpp`, the include should be:

```cpp
#include "LGFX_ESP32S3.h"

// Example display instance declaration (integrate with your actual game code):
LGFX tft;
```

Do **not** use an include path pointing through a different project directory. The old nested directory is deleted from the current repository tree. If your own local copy of `main.cpp` still uses that old include, change that line **in the game's source only** before compiling. This GitHub update cannot modify the missing local game source.

## How to recreate the original project

1. Use an **ESP32-S3** and an **ST7789 240 × 320 TFT** compatible with the saved LovyanGFX settings. Verify the exact board and display module before wiring.
2. Follow the **GPIO tables and wiring sketch** above. Connect each button between its GPIO and GND.
3. Install and activate an ESP-IDF environment. The original project notes specify **ESP-IDF 6.0.2**; compatibility has not been independently build-tested.
4. Restore the missing game source at `main/main.cpp`. The display header is already in this game's own `main/LGFX_ESP32S3.h`, and ESP-IDF component registration is already provided in `main/CMakeLists.txt`. In the restored `main.cpp`, include the header with `#include "LGFX_ESP32S3.h"`.
5. Install the correct [LovyanGFX](https://github.com/lovyan03/LovyanGFX) ESP-IDF component in `components/LovyanGFX/` as directed in the original notes, or adapt the component setup for the installed library version.
6. Once the missing files and dependencies are present, from the project directory run:

       idf.py set-target esp32s3
       idf.py build
       idf.py -p COMx flash monitor

   Replace `COMx` with your actual device port (for example, `COM5` on Windows). Do not expect `idf.py build` to work using **this snapshot alone**.

> **Flashing warning:** This is standalone firmware. Flashing it to a board running any other application replaces that currently installed application. Keep a backup of any firmware/configuration you intend to restore later.

## Troubleshooting / checkpoints

| Problem | What to check |
| --- | --- |
| Build cannot find `main.cpp` | The gameplay source `main/main.cpp` has not been uploaded |
| Build cannot find `LovyanGFX.hpp` | Install/configure the LovyanGFX ESP-IDF component |
| Display blank / wrong colors | Display power, backlight, wiring, reset pin, panel compatibility, `invert` and `rgb_order` settings |
| Display shows shifted picture | Panel variant, resolution and offsets in `LGFX_ESP32S3.h` |
| Upload/boot problem | USB/serial port, boot mode, strap pins, especially the GPIO46 RESET connection |
| Buttons unresponsive | Confirm button-to-GND wiring, GPIO availability, pull-ups, and later firmware mappings |

## Possible improvements / modifications

These are **future ideas, not verified current features**:

- Improve animation smoothness, sprite work, UI layout and transitions.
- Add collision effects, obstacles, difficulty stages and adjustable speed.
- Define more distinctive BOOST mechanics, power-ups and cooldown indicators.
- Add a high-score screen with persistent storage using ESP32 NVS.
- Add audio feedback or music using an available speaker/audio interface.
- Make the game menu-driven, with start/restart, options and pause/resume screens.
- Implement configurable GPIO mapping and proper software debouncing.
- Support alternative ST7789 panels or an alternate ESP32-S3 board.
- Add wiring photos, a schematic, screenshots and a gameplay video.
- Experiment with Wi-Fi leaderboard/synchronization as an optional extension.

## Current state and provenance

- **Uploaded from user-supplied files:** root `CMakeLists.txt`, `LGFX_ESP32S3.h` and the initial `README.txt`. The header now lives directly in this game's own `main/` directory; the local component registration file was added for a simpler project layout.
- **Reconstructed here:** human-readable hardware documentation, replayable setup steps, local `main/CMakeLists.txt` component registration, risks and suggested extensions.
- **Missing / not tested:** `main.cpp`, LovyanGFX dependency, successful compilation, flashing and hardware operation.

Once the original gameplay files are available, this README can be expanded to document *actual* gameplay rules, special mechanics, and screenshots instead of inferred labels.

---
*An experimental ESP32 hobby project, archived so the hardware and code can be rebuilt and improved later.*
