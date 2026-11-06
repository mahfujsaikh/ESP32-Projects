# Sperm Run Arcade 🎮

A small **ESP32-S3 arcade game** built for an **ST7789 TFT display** and five physical buttons, using **ESP-IDF** and **LovyanGFX**. The game is a side-scrolling obstacle-dodging challenge: guide an animated swimmer through moving gaps, survive collisions, and beat your best score.

This repository holds its **own copy** of the display configuration (`main/LGFX_ESP32S3.h`). No code, headers, or include paths from any other personal firmware project are needed.

> **Build status:** The complete supplied **gameplay source and ESP-IDF CMake files** are included here. LovyanGFX itself is an **external dependency** and is **not vendored** in this repository. Compilation and testing on physical hardware have **not** been verified for this GitHub snapshot.

## Gameplay

The game has four states: **TITLE → PLAYING ↔ PAUSED → GAME OVER**.

1. Press **MENU** on the title screen to start.
2. Hold **UP** or **DOWN** to steer your swimmer vertically. Input is smoothed rather than instantly teleporting the player.
3. Avoid green pipe-like obstacles by moving through the gap. There are **three obstacles** managed at a time.
4. Each successfully passed obstacle adds **1 point**.
5. Touching a pipe or the upper/lower boundary costs **1 life**. You begin with **3 lives**. After a nonfatal collision, the swimmer is reset to the centre.
6. Press **PAUSE** to pause or resume. Press **MENU** on the game-over screen to restart.

### Special features already present in the source

| Feature | Code behavior |
| --- | --- |
| **Animated player** | A cyan circular swimmer with a sine-wave animated tail |
| **BOOST button** | While held, obstacle scrolling speed becomes **170 px/s**; this is a *faster challenge*, not an invincibility or extra-life button |
| **Difficulty progression** | Normal obstacle scrolling starts at **105 px/s** and rises with score, capped at **133 px/s** |
| **Random gaps** | Recycled pipes receive a pseudo-random vertical gap position |
| **3-life system** | Obstacles and screen boundaries remove a life; game ends when lives reach zero |
| **On-screen HUD** | Shows current SCORE, BEST and LIVES |
| **Best score** | Tracked **in RAM only** during the running firmware session; it does **not** survive a reset or power cycle |
| **Pause / restart** | Dedicated physical controls, with edge detection for PAUSE and MENU |
| **Graphics** | ST7789 colour TFT, simple star/dot background, outlines, and game-state screens |

**Not implemented in this version:** sound, saved highscores in flash, multiple selectable levels, Wi-Fi leaderboard, power-up inventory, or menu settings. Button edge detection exists, but no time-based debouncing is implemented.

## Parts and hardware configuration

| Item | Specification |
| --- | --- |
| Microcontroller | **ESP32-S3**; exact board variant not recorded |
| TFT module | **ST7789**, configured for a physical **240 × 320** panel |
| Gameplay coordinates | **320 × 240 landscape**, set with `gfx.setRotation(1)` |
| Display driver | [LovyanGFX](https://github.com/lovyan03/LovyanGFX) |
| Firmware framework | ESP-IDF; **v6.0.2** specified in original project notes |
| TFT interface | SPI2 (`SPI2_HOST`), mode 0 |
| SPI clock settings | 40 MHz write, 16 MHz read |
| Input | 5 momentary buttons, **GPIO to GND** with internal pull-ups |
| Display backlight & supply | Not recorded; confirm your specific TFT module's requirements |

### TFT display wiring

| TFT signal | ESP32-S3 GPIO | Usage |
| --- | --- | --- |
| SCLK / SCK | **12** | SPI clock |
| MOSI / SDA / DIN | **11** | SPI data to TFT |
| MISO / SDO | **13** | Configured SPI data from TFT, if module provides it |
| DC / A0 | **9** | Data / command |
| CS | **10** | Chip select |
| RST / RESET | **46** | Display reset; **see warning below** |
| GND | GND | Shared ground |
| VCC | **Module dependent** | Verify supply voltage and level shifting |
| BL / LED | **Not specified** | Check TFT backlight pin / board circuit |

> **GPIO46 caution:** On the ESP32-S3, GPIO46 is a strapping pin with boot-related constraints. The saved header uses it for TFT RESET because that was the original configuration. Do not assume this is a universally safe wiring choice: check your particular ESP32-S3 board, display module, and boot circuitry. If the display must use a different reset pin, change **both** the physical connection and `cfg.pin_rst`.

### Button wiring

| Button | GPIO | Connection |
| --- | ---: | --- |
| **UP** | **1** | GPIO1 → push-button → GND |
| **DOWN** | **2** | GPIO2 → push-button → GND |
| **BOOST** | **8** | GPIO8 → push-button → GND |
| **PAUSE** | **14** | GPIO14 → push-button → GND |
| **MENU** | **18** | GPIO18 → push-button → GND |

The code explicitly configures these five pins with `GPIO_MODE_INPUT` and `GPIO_PULLUP_ENABLE`. A pressed button therefore reads **LOW (0)**. The code does not use external interrupt handling. Verify pin availability on your specific ESP32-S3 board and do **not** apply 5 V directly to GPIOs.

### Wiring reference

```text
ESP32-S3                      ST7789
---------                     -------------
GPIO12  --------------------> SCLK
GPIO11  --------------------> MOSI / SDA
GPIO13  <-------------------- MISO / SDO (if present)
GPIO9   --------------------> DC
GPIO10  --------------------> CS
GPIO46  --------------------> RST (strapping-pin caution)
GND     --------------------- GND
supply  --------------------- VCC (check TFT module)
                              BL  (unspecified)

GPIO1   --- [ UP ] ---------- GND
GPIO2   --- [ DOWN ] -------- GND
GPIO8   --- [ BOOST ] ------- GND
GPIO14  --- [ PAUSE ] ------- GND
GPIO18  --- [ MENU ] -------- GND
```

An earlier hardware layout also assigned GPIO4/5/6 to microphone connections and GPIO15/16/17 to speaker connections. These do not form part of this game's code; check your actual wiring before reusing any pins.

## Source tree

```text
Sperm-Run-Arcade/
├── CMakeLists.txt                     # ESP-IDF project entry
├── .gitignore
├── README.md
├── README.txt                         # Short setup / wiring notes
└── main/
    ├── CMakeLists.txt                 # Main component and required dependencies
    ├── main.cpp                       # Complete supplied gameplay source
    └── LGFX_ESP32S3.h                # Standalone custom ST7789 display config
```

**LovyanGFX is not included** in the tree shown above. Install it as an ESP-IDF component before building.

### Local display-header include

The game source includes the display config directly, with **no old nested board-directory reference**:

```cpp
#include "LGFX_ESP32S3.h"

static LGFX gfx;
```

The component file uses:

```cmake
idf_component_register(
    SRCS "main.cpp"
    INCLUDE_DIRS "."
    REQUIRES LovyanGFX esp_driver_gpio esp_timer
)
```

These dependencies are taken from the uploaded component configuration; only the obsolete nested include directory was removed.

## Build and flash

1. Install and activate the [Espressif ESP-IDF](https://docs.espressif.com/projects/esp-idf/en/latest/esp32s3/get-started/index.html) environment. The original notes specify **ESP-IDF 6.0.2**, but the GitHub snapshot has not been compile-tested against it.
2. Clone or download the project, then open a terminal in `Sperm-Run-Arcade`.
3. Install a **LovyanGFX ESP-IDF component** compatible with your installed IDF version. The original setup expects the component at `components/LovyanGFX/`; consult the [LovyanGFX upstream repository](https://github.com/lovyan03/LovyanGFX) for current component setup. Check that its registered component name is `LovyanGFX` to match the CMake dependency.
4. Check your specific display module power, backlight and pin wiring before connecting it.
5. Build:

   ```bash
   idf.py set-target esp32s3
   idf.py build
   ```

6. Flash and monitor:

   ```bash
   idf.py -p COMx flash monitor
   ```

   Replace `COMx` with the actual serial port (for example `COM5` on Windows, or the appropriate `/dev/tty*` device on Linux).

> **Firmware warning:** Flashing a new game image to an ESP32-S3 replaces its existing application firmware. Make backups of anything you need to restore later.

## Source map and tuning

| Code symbol | Default | What it controls |
| --- | --- | --- |
| `W` / `H` | 320 / 240 | Landscape game coordinate system |
| `PLAYER_X` / `PLAYER_R` | 72 / 8 | Player horizontal position / collision radius |
| `PIPE_W` / `GAP_H` | 25 / 88 | Obstacle width / vertical opening |
| `HUD_H` / `FLOOR_Y` | 25 / 228 | Top HUD and lower play boundary |
| `lives` | 3 at game reset | Lives per attempt |
| `speed` | 105 base; 170 while boosting | Obstacle movement speed |
| `newGap()` | 76–165 | Randomized obstacle opening centre |
| `vTaskDelay(...)` | 20 ms | Nominal main-loop sleep (not a guaranteed 50 FPS) |
| `gfx.setRotation(1)` | Landscape | TFT orientation |

To customize gameplay, adjust these values in `main/main.cpp` and the actual pin/SPI settings in `main/LGFX_ESP32S3.h`. Make sure any new obstacle dimensions and player positions remain inside the screen boundaries.

## Known limitations and possible modifications

- **Persistent best score:** Save `best` in ESP32 NVS, with an option to clear the record.
- **Better input handling:** Add switch debouncing, key remapping, and more intuitive button feedback.
- **More varied obstacles:** Different openings, shapes, patterns or stage designs.
- **BOOST redesign:** Energy meter, limited boost duration, cooldown, or obstacle slow-down instead of the current speed-up.
- **Improved graphics:** Sprites, animations, a dedicated background buffer, smoother rendering and optimized dirty regions.
- **Sound effects:** Integrate an optional speaker / I2S amplifier with appropriately assigned GPIOs.
- **Menu upgrades:** Settings, difficulty selection, restart confirmation, and better accessibility.
- **Wi-Fi extras:** Optional leaderboard, score sharing, or OTA updates.
- **Reproducibility:** Add tested ESP-IDF/LovyanGFX versions, actual board model, wiring photos, screenshots, and a successful build log.

## Troubleshooting

| Symptom | Check |
| --- | --- |
| `LovyanGFX` component missing | Install/configure the required ESP-IDF component and confirm its component name |
| Display blank | TFT supply, backlight, ground, SPI wiring, reset, rotation and module compatibility |
| Boot or flashing failure | USB port and boot configuration, including possible GPIO46 reset/strapping conflict |
| Buttons don't respond | Active-low wiring to GND, internal pull-ups, and board GPIO availability |
| Colours/offset look wrong | `rgb_order`, `invert`, ST7789 variant, and panel config in the header |
| High score disappears after reboot | Expected: `best` is RAM-only in this source version |

## Project status

- ✅ **Complete user-supplied game source uploaded:** `main/main.cpp`
- ✅ **Standalone custom TFT header:** `main/LGFX_ESP32S3.h`
- ✅ **Root and main ESP-IDF CMake configuration**
- ✅ **Gameplay documentation and verified source-derived GPIO assignments**
- ⚠️ **External LovyanGFX component must be installed separately**
- ⚠️ **No successful compilation, flash, or physical device test has been performed as part of this upload**

*Personal ESP32-S3 hobby project — preserved to make future remakes and improvements easier.*
