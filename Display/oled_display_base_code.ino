// Iclude Libs
#include <SPI.h>
#include "SSD1306Ascii.h"
#include "SSD1306AsciiSpi.h"
// Define Pins
#define CS_PIN    7
#define RST_PIN   8
#define DC_PIN    9
#define MOSI_PIN 11
#define CLK_PIN  13

SSD1306AsciiSpi oled;
//rows 4
//In row 16

//val
int Year = 2020;
int Month = 12;
int Date = 23;
int Hour = 17;
int Minute = 43;
int Second = 26;
float distance = 123;
int steps = 12345;

void setup() {
  // Set Up
  oled.begin(&Adafruit128x64, CS_PIN, DC_PIN, RST_PIN);
  // Fonts Select
  oled.setFont(ZevvPeep8x16);
  // Clear Dis
  oled.clear();
  // Testing
  oled.println("1234567890123456");
  oled.println("6543210987654321");
  oled.println("1234567890123456");
  oled.println("6543210987654321");
  delay(1000);
  oled.clear();32
  oled.clear(0 * 2, 16 * 8, 0 * 2, 1 * 3);
    oled.setCursor(4 * 8, 0 * 2);
    oled.print(Hour);
    oled.print(":");
    oled.print(Minute);
    oled.print(":");
    oled.print(Second);
  
    oled.setCursor(3 * 8, 1 * 2);
    oled.print(Date);
    oled.print("/");
    oled.print(Month);
    oled.print("/");
    oled.print(Year);
  
    oled.setCursor(2 * 8, 2 * 2);
    oled.print("Steps:");
    oled.print(steps);
  
    if (distance > 1000) {
      oled.setCursor(5 * 8, 3 * 2);
      oled.print(distance / 1000, 1);
      oled.print(" km");
    }
    else {
      oled.setCursor(6 * 8, 3 * 2);
      oled.print(distance, 0);
      oled.print(" m");
    }
}

void loop() {

}
