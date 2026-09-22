# Reproducing the prototype

This guide describes how to rebuild the sensing, control, transmission, storage, and visualization workflow used by the 2025 course prototype. Complete the stages in order and verify each stage before connecting the next one.

## 1. System boundary

The prototype has four connected parts:

1. A Seeeduino LoRaWAN reads the sensors and controls two relays.
2. The board sends five measurements as a Cayenne Low Power Payload over an EU868 LoRaWAN network.
3. A bridge maps each Cayenne channel to a FROST SensorThings Datastream.
4. Grafana reads the FROST data. A scheduled Python job adds outdoor temperature and humidity from WeatherAPI.

You need your own LoRaWAN device registration, FROST write access, WeatherAPI key, and Grafana instance to reproduce the complete live pipeline. The repository contains the Arduino firmware, mapping contract, API metadata, download utility, weather uploader, and dashboard panel specification.

## 2. Hardware

### Bill of materials

| Quantity | Component | Function |
| ---: | --- | --- |
| 1 | Seeeduino LoRaWAN with Grove base shield and antenna | Controller and EU868 transmission |
| 1 | Grove BME680 temperature, humidity, pressure, and gas sensor | Air measurements and IAQ input |
| 1 | Grove TSL2561 digital light sensor | Illuminance |
| 1 | Grove capacitive moisture sensor | Relative moisture of the growing medium |
| 1 | Grove RGB LCD, 16 × 2 | Local status display |
| 2 | Relay modules | Switch the fan and pump |
| 1 | Ventilation fan | Cooling and air exchange |
| 1 | Water pump with tubing and reservoir | Humidity control |
| 1 | Plastic growing enclosure | Mushroom chamber |
| as needed | Suitable external power supplies and cables | Power for the controller and actuators |

The fan and pump draw power from suitable external supplies and are switched through relays. Check the relay contact rating, actuator voltage, polarity, and grounding before powering the system. Keep the controller, relays, connectors, and mains-powered equipment outside the humid chamber.

### Connections used by the firmware

| Device or signal | Connection |
| --- | --- |
| BME680 | I2C, address `0x76` |
| TSL2561 | I2C through the Grove base shield |
| RGB LCD | I2C through the Grove base shield |
| Capacitive moisture sensor | Analog input `A0` |
| Fan relay input | Digital pin `D4` |
| Pump relay input | Digital pin `D5` |
| Test button | Digital pin `D2` |
| Built-in/test LED | Digital pin `D13` |
| Grove connector power | Pin `38`, set `HIGH` by the firmware |
| LoRaWAN antenna | Board RF connector; attach before transmission |

The control code assumes that `HIGH` activates each relay. If a relay board is active-low, invert the two relay outputs before connecting the fan or pump.

## 3. Arduino environment

The archived project used the following library versions:

| Library | Recorded version |
| --- | ---: |
| Seeed BME680 | 1.0.5 |
| BSEC Software Library | 1.8.1492 |
| Grove - Digital Light Sensor | 2.0.0 |
| Grove - LCD RGB Backlight | 1.0.2 |
| CayenneLPP | 1.4.0 |
| ArduinoJson | 7.4.2 |

The firmware also requires the legacy Seeed `LoRaWan.h` library used with the Seeeduino LoRaWAN board. Select the SAMD-compatible board package for the board and choose its serial port in the Arduino IDE.

Use the small sketches in [`firmware/examples/`](firmware/examples/) before the integrated firmware:

1. Upload `lcd_test.ino` and confirm that the display initializes.
2. Upload `bme680_test.ino` and check temperature, humidity, pressure, and gas readings in the Serial Monitor.
3. Upload `light_sensor_test.ino` and confirm that the lux value changes when the sensor is covered.
4. Upload `soil_moisture_test.ino` and confirm that the raw value changes between dry and wet growing medium.

These checks isolate wiring and library problems before LoRaWAN and actuator control are added.

## 4. LoRaWAN credentials and firmware upload

Register the board as an OTAA device with an EU868 LoRaWAN network. Obtain a Device EUI, Application EUI, and Application Key.

Create the local credential file:

```bash
cp firmware/smart_mushroom_box/secrets.example.h \
   firmware/smart_mushroom_box/secrets.h
```

Replace the three placeholders in `secrets.h`:

```cpp
#define LORAWAN_DEV_EUI "YOUR_DEVICE_EUI"
#define LORAWAN_APP_EUI "YOUR_APPLICATION_EUI"
#define LORAWAN_APP_KEY "YOUR_APPLICATION_KEY"
```

Open [`smart_mushroom_box.ino`](firmware/smart_mushroom_box/smart_mushroom_box.ino), compile it for the Seeeduino board, and upload it. Open the Serial Monitor at `115200` baud. A successful startup reports the LoRa module information followed by `LoRaWAN join successful`. Each measurement cycle then prints temperature, air humidity, raw growing-medium moisture, and illuminance.

The course prototype transmitted approximately every 30 seconds. The constant `LOOP_DELAY_MS` controls this interval.

## 5. Measurement and control contract

The firmware sends the following Cayenne channels. The channel numbers must match the bridge mapping exactly.

| Channel | Cayenne value type | Measurement | FROST Datastream used by the project |
| ---: | --- | --- | ---: |
| 1 | Temperature | Box temperature | 1507 |
| 2 | Relative humidity encoding | Raw relative growing-medium moisture | 1508 |
| 3 | Luminosity | Illuminance in lux | 1670 |
| 4 | Analog input | BSEC indoor air quality value | 1671 |
| 5 | Analog input | Box air humidity | 1673 |

The integrated control rules are:

- fan relay on when box temperature is above `22.1 °C`;
- pump relay on when box air humidity is below `40%`;
- both relays off when their respective trigger condition is false.

The course test recorded relative moisture reference readings of approximately `13` in dry growing medium and `8` in saturated growing medium. These values describe the original sensor and installation. Repeat the dry and saturated tests after rebuilding the box and use the new readings as local reference points.

Before connecting the actuators, observe the relay LEDs or measure the switched output while changing the thresholds temporarily. This confirms relay polarity and prevents an unexpectedly continuous pump run.

## 6. FROST SensorThings setup

Create one SensorThings `Thing` for the box and five Datastreams for the measurements in the table above. Each Datastream also needs a Sensor, ObservedProperty, and unit definition. The saved project metadata in [`frost/snapshots/`](frost/snapshots/) provides concrete examples of those entities.

Configure the LoRaWAN-to-FROST bridge with [`frost/channel_mapping.js`](frost/channel_mapping.js):

```bash
export DEVICE_EUI="YOUR_DEVICE_EUI"
```

For a new FROST instance, replace `FROST_BASE_URL` and every `datastream_iot_id` with the values from that instance. The essential mapping is `lpp_id 1–5` to the corresponding Datastream IDs. The course deployment stored this mapping in MongoDB; another bridge can use the same JavaScript object or translate it into its own configuration format.

After the device transmits, confirm that a Datastream returns observations:

```text
https://YOUR_FROST_HOST/v1.1/Datastreams(YOUR_ID)/Observations?$top=5&$orderby=phenomenonTime%20desc
```

For the archived public Datastreams, download the available observations with:

```bash
python frost/download_observations.py --output data/frost_observations.csv
```

## 7. Outdoor weather automation

Create two additional FROST Datastreams for outdoor Munich temperature and humidity. Configure these variables locally or as GitHub repository secrets:

```text
WEATHERAPI_KEY
FROST_BASE_URL
FROST_TEMPERATURE_DATASTREAM_ID
FROST_HUMIDITY_DATASTREAM_ID
FROST_API_TOKEN              # only when the server requires a bearer token
```

Test payload generation before writing data:

```bash
python automation/weather_uploader.py --location Munich
```

After checking the printed payload and Datastream IDs, submit one pair of observations:

```bash
python automation/weather_uploader.py --location Munich --submit
```

To run it every five minutes, copy `automation/weather-upload.yml` to `.github/workflows/weather-upload.yml` in your fork and add the variables above as repository secrets. The workflow also supports manual execution.

## 8. Grafana dashboard

Connect Grafana to the FROST SensorThings API and create the nine panels listed in [`grafana/dashboard_data_mapping.json`](grafana/dashboard_data_mapping.json):

- gauges for current indoor and outdoor temperature and humidity;
- paired time series for indoor versus outdoor temperature and humidity;
- time series for growing-medium moisture, illuminance, and indoor air quality.

For each panel, query the relevant Datastream's `Observations`, use `phenomenonTime` as the time field, and use `result` as the numeric field. Set the dashboard time range to the period being inspected. The project dashboard used 8–13 July 2025 for the final indoor/outdoor comparison.

## 9. End-to-end verification

Check the completed system in this order:

1. All four Arduino example sketches return plausible readings.
2. The integrated firmware joins the LoRaWAN network.
3. The Serial Monitor prints one complete measurement cycle.
4. The five Cayenne channels appear in their corresponding FROST Datastreams.
5. A temperature above `22.1 °C` activates the fan relay.
6. Air humidity below `40%` activates the pump relay.
7. The weather uploader creates one temperature and one humidity observation.
8. Grafana shows current values and aligned indoor/outdoor time series.

If values appear in the wrong Datastream, compare the Cayenne channel table with `channel_mapping.js`. If the board does not join the network, check the three OTAA identifiers, EU868 coverage, antenna connection, and serial output. If a sensor fails, return to its individual example sketch before debugging the complete firmware.

## 10. Experimental procedure

Place the BME680 and TSL2561 in the chamber air and insert the capacitive probe into the growing medium. Keep the sensor electronics above the wet surface. Record the dry and saturated moisture reference points, then run the complete system while logging observations to FROST.

The original cultivation record covered 24 June to 10 July 2025. During operation, the team inspected daily growth photographs together with temperature and moisture trends. The thresholds came from this enclosure and should be recalibrated when the chamber, substrate, airflow, pump, or sensor position changes.
