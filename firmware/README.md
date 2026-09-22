# Firmware setup

The public sketch preserves the sensing, LoRaWAN transmission, display, and actuator-control logic used in the course prototype while keeping credentials outside version control.

The `examples/` directory also preserves the separate BME680, TSL2561, soil-moisture, and RGB-LCD test sketches used while assembling the prototype.

## Required hardware and libraries

- Seeeduino LoRaWAN with Grove base shield
- BME680 sensor and `Seeed_BME680` library
- Bosch BSEC library
- TSL2561 light sensor and `Digital_Light_TSL2561` library
- Grove RGB LCD and `rgb_lcd` library
- `LoRaWan` library for the Seeeduino board
- `CayenneLPP` library

The sketch reflects the library APIs and board configuration used during the 2025 course project. Newer library releases or different boards may require small changes.

## Configure credentials

1. Copy `secrets.example.h` to `secrets.h` in the same folder as the sketch.
2. Replace the placeholders with the LoRaWAN identifiers and AppKey assigned to your own device.
3. Keep `secrets.h` local. It is excluded by `.gitignore`.

## Calibrate before use

The fan and pump thresholds came from the original enclosure and cultivation experiment. Verify sensor readings, relay polarity, actuator power requirements, and safe operating thresholds before using the sketch with another setup.
