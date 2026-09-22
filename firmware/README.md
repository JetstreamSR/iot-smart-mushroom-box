# Arduino firmware

The integrated sketch implements the sensing, LoRaWAN transmission, display, and actuator-control logic used by the prototype. The `examples/` directory contains separate BME680, TSL2561, growing-medium moisture, and RGB-LCD tests. Run these small sketches first when assembling or debugging the hardware.

## Required hardware and libraries

- Seeeduino LoRaWAN with Grove base shield
- BME680 sensor and `Seeed_BME680` library
- Bosch BSEC library
- TSL2561 light sensor and `Digital_Light_TSL2561` library
- Grove RGB LCD and `rgb_lcd` library
- `LoRaWan` library for the Seeeduino board
- `CayenneLPP` library

The archived environment contained Seeed BME680 1.0.5, BSEC 1.8.1492, Grove Digital Light Sensor 2.0.0, Grove LCD RGB Backlight 1.0.2, CayenneLPP 1.4.0, and ArduinoJson 7.4.2. It used the legacy Seeed `LoRaWan.h` API with a SAMD-compatible Seeeduino LoRaWAN board.

## Pin and bus assignments

| Function | Connection |
| --- | --- |
| BME680 | I2C address `0x76` |
| TSL2561 and RGB LCD | I2C |
| Growing-medium moisture | `A0` |
| Fan relay | `D4` |
| Pump relay | `D5` |
| Test button | `D2` |
| Test LED | `D13` |

The sketch sets pin 38 high to power the Grove connectors and assumes that a high digital output activates each relay.

## Configure credentials

1. Copy `secrets.example.h` to `secrets.h` in the same folder as the sketch.
2. Replace the placeholders with the LoRaWAN identifiers and AppKey assigned to your own device.
3. Keep `secrets.h` local. It is excluded by `.gitignore`.

## Upload and verify

1. Attach the LoRaWAN antenna.
2. Select the SAMD-compatible Seeeduino board and its serial port in the Arduino IDE.
3. Compile and upload `smart_mushroom_box/smart_mushroom_box.ino`.
4. Open the Serial Monitor at 115200 baud.
5. Confirm `LoRaWAN join successful`, followed by temperature, humidity, moisture, and illuminance readings.

The firmware uses EU868 OTAA, LoRaWAN port 33, and the Cayenne channel assignments documented in the project [reproduction guide](../REPRODUCING.md).

## Calibrate before use

The fan and pump thresholds came from the original enclosure and cultivation experiment. Verify sensor readings, relay polarity, actuator power requirements, and safe operating thresholds before using the sketch with another setup.

The original thresholds were 22.1 °C for the fan and 40% box air humidity for the pump. The archived moisture reference readings were approximately 13 for dry growing medium and 8 for saturated growing medium. Recalibrate these values for a different probe or installation.
