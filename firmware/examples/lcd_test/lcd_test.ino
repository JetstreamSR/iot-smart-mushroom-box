#include <Wire.h>
#include "rgb_lcd.h"
#define PIN_GROVE_POWER 38
rgb_lcd lcd;

const int colorR = 0;
const int colorG = 255;
const int colorB = 0;

void setup() {
    // Powerup Seeeduino LoRaWAN Grove connectors
    pinMode(PIN_GROVE_POWER, OUTPUT);
    digitalWrite(PIN_GROVE_POWER, 1);

    // set up the LCD's number of columns and rows:
    lcd.begin(16, 2);

    lcd.setRGB(colorR, colorG, colorB);

    // Print a message to the LCD.
    lcd.print("hello, world!");

    delay(1000);
}

void loop() {

    // set the cursor to column 0, line 1
    // (note: line 1 is the second row, since counting begins with 0):
    lcd.setCursor(0, 1);
    // print the number of seconds since reset:
    lcd.print(millis() / 1000);

    delay(100);

}
