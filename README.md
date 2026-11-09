# weather_thing_esp32 — Adafruit Matrix Portal S3 base project

ESP-IDF (native C/C++, no Arduino) base project for driving a HUB75 RGB LED
matrix panel from the Matrix Portal S3, mirroring the RP2350 `weather_thing`
build (pimoroni Interstate75 + `Hub75` DMA refresh).

- Target: `esp32s3` · ESP-IDF **v5.5.3** (`C:\Espressif\frameworks\esp-idf-v5.5.3`)
- Panel driver: [`esphome/esp-hub75`](https://components.espressif.com/components/esphome/esp-hub75)
  (managed component, pulled in by `main/idf_component.yml`; uses the S3's
  LCD_CAM + GDMA peripherals for background refresh)
- Demo (`main/main.cpp`): cycling R/G/B/white fills + a pixel bouncing around
  the panel border.

## Panel wiring

The Matrix Portal S3's HUB75 connector is fixed-wired (see `main/main.cpp`):

| Signal | GPIO | Signal | GPIO |
|---|---|---|---|
| R1/G1/B1 | 42/41/40 | A/B/C/D | 45/36/48/35 |
| R2/G2/B2 | 38/39/37 | E (64x64 only) | 21 (solder jumper) |
| LAT / OE / CLK | 47 / 14 / 2 | | |

Panel geometry (size, chaining) is configured with the constants at the top of
`main/main.cpp`.

## Building & flashing from CLion

The "ESP-IDF" toolchain must export IDF_PATH to CMake. CLion cannot use
Espressif's `export.bat` directly as the environment file, so use the wrapper
in this repo:

1. **Settings → Build, Execution, Deployment → Toolchains → `ESP-IDF` →
   Environment file:** set it to
   `C:\Users\Jared\Desktop\weather_thing_c\weather_thing_esp32\espidf_clion_env.bat`
2. **Tools → CMake → Reset Cache and Reload Project.**
3. Build as usual. The first configure downloads the `hub75` managed component
   (needs internet once; files land in `managed_components/`).

Flash/monitor (CLion's system toolchain has no built-in flash button): open a
terminal in this directory and run

```powershell
. .\espidf_source.ps1   # loads the ESP-IDF environment into the session
idf.py flash            # auto-detects the COM port
idf.py monitor          # logs on the USB-C port (USB-Serial-JTAG console)
```

## Building without CLion

```powershell
. .\espidf_source.ps1
idf.py build flash monitor
```

## If the panel misbehaves

- **Black screen** → in `main.cpp`, try
  `config.shift_driver = Hub75ShiftDriver::FM6126A;` (most modern panels)
- **Mirrored/duplicated half** → typical of 64x32 panels wired for 1/8 scan;
  try `config.scan_wiring = Hub75ScanWiring::FOUR_SCAN_32PX_HIGH;`
- **Swapped colors** → check the R/G/B pin constants against your panel cable.
