// todo list
// add heart rate
// add acc
// start/stop button
// set button
// lap button maybe

// Iclude Libs
// Libs RTC
#include <Wire.h>
// Libs GPS
#include <TinyGPS++.h>
#include <SoftwareSerial.h>
// Libs Display
#include <SPI.h>
#include "SSD1306Ascii.h"
#include "SSD1306AsciiSpi.h"
//#include "ED_Functions.ccp"

//pre setup
//general
#define Button_pin ???
int Start;
bool runing = 0;

//RTC
byte second, minute, hour, dayOfWeek, day, month, year;
#define DS3231_I2C_ADDRESS 0x68

//GPS
double LAT = 0, LONG = 0;
float distance = 0;
// Define Pins
#define RXPin 4
#define TXPin 3
static const uint32_t GPSBaud = 9600;
// The TinyGPS++ object
TinyGPSPlus gps;
// The serial connection to the GPS device
SoftwareSerial ss(RXPin, TXPin);

//display
// Define Pins
#define CS_PIN    7
#define RST_PIN   8
#define DC_PIN    9
#define MOSI_PIN 11
#define CLK_PIN  13

SSD1306AsciiSpi oled;

void setup() {
  //GPS Set Up
  Serial.begin(9600);
  ss.begin(GPSBaud);
  //i2c set up
  Wire.begin();
  // Display Set Up
  oled.begin(&Adafruit128x64, CS_PIN, DC_PIN, RST_PIN);
  // Select Font
  oled.setFont(ZevvPeep8x16);
  // Clear Display
  oled.clear();
  oled.println("Starting...");
  if (millis() > 5000 && gps.charsProcessed() < 10)
  {
    oled.println("Error: No GPS detected");
    while (true);
  }
  // set RTC time and date with GPS input
  setDS3231time();
  oled.clear();
}
void loop() {
  //read time from RTC to update time and val
  readDS3231time(&second, &minute, &hour, &dayOfWeek, &day, &month,  &year);
  //display time in line 0
  oled.setCursor(4 * 8, 0 * 2);
  oled.print(hour < 10 ? "0" : "");
  oled.print(hour);
  oled.print(":");
  oled.print(minute < 10 ? "0" : "");
  oled.print(minute);
  oled.print(":");
  oled.print(second < 10 ? "0" : "");
  oled.print(second);
  //display Heart rate in line 3

  ////////
  if (digitalRead(Button_pin) == 1) {
    runing = ( runing == 1 ? "0" : "1");
    Start = millis();
    LAT = gps.location.lat();
    LONG = gps.location.lng();
  }
  ///////
  //runing mode
  if (runing == 1) {
    //display Timer in line 1
    Timer();
    //display Distance in line 2
    Distance();
  }
  else {
    //display date in line 1
    oled.setCursor(3 * 8, 1 * 2);
    oled.print(day < 10 ? "0" : "");
    oled.print(day);
    oled.print("/");
    oled.print(month < 10 ? "0" : "");
    oled.print(month);
    oled.print("/");
    oled.print(year);
    //display steps in line 2
    oled.setCursor(2 * 8, 2 * 2);
    oled.print("Steps:");
    oled.print(steps);
  }
}
