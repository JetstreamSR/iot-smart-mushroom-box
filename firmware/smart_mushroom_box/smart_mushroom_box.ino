#include <Wire.h>
#include <LoRaWan.h>
#include <CayenneLPP.h>
#include <Digital_Light_TSL2561.h>
#include "seeed_bme680.h"
#include "bsec.h"
#include "rgb_lcd.h"
#include "secrets.h"

#define PIN_GROVE_POWER 38
#define SerialUSB Serial
#define BME680_I2C_ADDRESS uint8_t(0x76)

const int BUTTON_PIN = 2;
const int LED_PIN = 13;
const int FAN_RELAY_PIN = 4;
const int PUMP_RELAY_PIN = 5;

const float FAN_ON_TEMPERATURE_C = 22.1;
const float PUMP_ON_HUMIDITY_PERCENT = 40.0;
const unsigned long LOOP_DELAY_MS = 30000;

rgb_lcd lcd;
CayenneLPP lpp(51);
Seeed_BME680 bme680(BME680_I2C_ADDRESS);
Bsec iaqSensor;

bsec_virtual_sensor_t bsecOutputs[] = {
  BSEC_OUTPUT_RAW_TEMPERATURE,
  BSEC_OUTPUT_RAW_PRESSURE,
  BSEC_OUTPUT_RAW_HUMIDITY,
  BSEC_OUTPUT_RAW_GAS,
  BSEC_OUTPUT_IAQ,
  BSEC_OUTPUT_STATIC_IAQ,
  BSEC_OUTPUT_CO2_EQUIVALENT,
  BSEC_OUTPUT_BREATH_VOC_EQUIVALENT
};

bool ledState = LOW;
bool previousButtonState = LOW;
unsigned int loopCount = 0;

void joinLoRaWAN() {
  char buffer[256];

  lora.init();
  lora.getVersion(buffer, sizeof(buffer), 1);
  SerialUSB.print(buffer);
  lora.getId(buffer, sizeof(buffer), 1);
  SerialUSB.print(buffer);

  lora.setId(NULL, LORAWAN_DEV_EUI, LORAWAN_APP_EUI);
  lora.setKey(NULL, NULL, LORAWAN_APP_KEY);
  lora.setDeciveMode(LWOTAA);  // Method name used by the original Seeed library.
  lora.setDataRate(DR0, EU868);
  lora.setAdaptiveDataRate(true);

  const float frequencies[] = {
    868.1, 868.3, 868.5, 867.1, 867.3, 867.5, 867.7, 867.9
  };
  for (int channel = 0; channel < 8; channel++) {
    lora.setChannel(channel, frequencies[channel]);
  }

  lora.setDutyCycle(false);
  lora.setJoinDutyCycle(false);
  lora.setPower(14);
  lora.setPort(33);

  while (!lora.setOTAAJoin(JOIN, 20)) {
    SerialUSB.println("LoRaWAN join failed; retrying...");
    delay(2000);
  }
  SerialUSB.println("LoRaWAN join successful");
}

void setup() {
  SerialUSB.begin(115200);
  Wire.begin();

  pinMode(PIN_GROVE_POWER, OUTPUT);
  digitalWrite(PIN_GROVE_POWER, HIGH);
  pinMode(BUTTON_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(FAN_RELAY_PIN, OUTPUT);
  pinMode(PUMP_RELAY_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  digitalWrite(FAN_RELAY_PIN, LOW);
  digitalWrite(PUMP_RELAY_PIN, LOW);

  lcd.begin(16, 2);
  lcd.print("Hello Mushroom!");

  while (!bme680.init()) {
    SerialUSB.println("BME680 initialization failed; retrying...");
    delay(10000);
  }

  iaqSensor.begin(BME680_I2C_ADDRESS, Wire);
  iaqSensor.updateSubscription(
    bsecOutputs,
    sizeof(bsecOutputs) / sizeof(bsecOutputs[0]),
    BSEC_SAMPLE_RATE_LP
  );

  TSL2561.init();
  joinLoRaWAN();
}

void transmitMeasurements(
  float temperature,
  float humidity,
  int soilMoisture,
  int visibleLux
) {
  lpp.reset();
  lpp.addTemperature(1, temperature);
  lpp.addRelativeHumidity(2, humidity);
  lpp.addAnalogInput(3, soilMoisture);
  lpp.addLuminosity(4, visibleLux);
  lpp.addAnalogInput(5, iaqSensor.iaq);
  lpp.addAnalogInput(6, iaqSensor.co2Equivalent);
  lpp.addAnalogInput(7, iaqSensor.breathVocEquivalent);

  lora.transferPacket(lpp.getBuffer(), lpp.getSize(), 5);
}

void updateLocalControls(float temperature, float humidity) {
  digitalWrite(
    FAN_RELAY_PIN,
    temperature > FAN_ON_TEMPERATURE_C ? HIGH : LOW
  );
  digitalWrite(
    PUMP_RELAY_PIN,
    humidity < PUMP_ON_HUMIDITY_PERCENT ? HIGH : LOW
  );

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("T:");
  lcd.print(temperature, 1);
  lcd.print("C H:");
  lcd.print(humidity, 0);
  lcd.print("%");
  lcd.setCursor(0, 1);
  lcd.print(temperature > FAN_ON_TEMPERATURE_C ? "FAN: ON" : "FAN: OFF");
}

void updateButton() {
  bool currentButtonState = digitalRead(BUTTON_PIN);
  if (currentButtonState == HIGH && previousButtonState == LOW) {
    ledState = !ledState;
    digitalWrite(LED_PIN, ledState);
    while (digitalRead(BUTTON_PIN) == HIGH) {
      delay(10);
    }
  }
  previousButtonState = currentButtonState;
}

void loop() {
  loopCount++;
  SerialUSB.print("Measurement cycle ");
  SerialUSB.println(loopCount);

  if (bme680.read_sensor_data()) {
    SerialUSB.println("BME680 reading failed");
    delay(LOOP_DELAY_MS);
    return;
  }

  const float temperature = bme680.sensor_result_value.temperature;
  const float humidity = bme680.sensor_result_value.humidity;
  const int soilMoisture = analogRead(A0);
  const int visibleLux = TSL2561.readVisibleLux();

  iaqSensor.run();

  SerialUSB.print("Temperature: ");
  SerialUSB.println(temperature);
  SerialUSB.print("Humidity: ");
  SerialUSB.println(humidity);
  SerialUSB.print("Soil moisture (raw): ");
  SerialUSB.println(soilMoisture);
  SerialUSB.print("Illuminance (lux): ");
  SerialUSB.println(visibleLux);

  transmitMeasurements(temperature, humidity, soilMoisture, visibleLux);
  updateLocalControls(temperature, humidity);
  updateButton();

  delay(LOOP_DELAY_MS);
}
