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

//RTC
byte second, minute, hour, dayOfWeek, day, month, year;
#define DS3231_I2C_ADDRESS 0x68

//GPS
static const int RXPin = 4, TXPin = 3;
static const uint32_t GPSBaud = 9600;
// The TinyGPS++ object
TinyGPSPlus gps;
// The serial connection to the GPS device
SoftwareSerial ss(RXPin, TXPin);

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
  //RTC Set Up
  Wire.begin();
  // Display Set Up
  oled.begin(&Adafruit128x64, CS_PIN, DC_PIN, RST_PIN);
  // Fonts Select
  oled.setFont(ZevvPeep8x16);
  // Clear Dis
  oled.clear();
  // Testing
  oled.println("Hello world!");
  delay(1000);
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
}

//functions
byte decToBcd(byte val)// convert dec to bin
{
  return ((val / 10 * 16) + (val % 10));
}

byte bcdToDec(byte val)// convert bin to dec
{
  return ((val / 16 * 10) + (val % 16));
}

void setDS3231time() { // set RTC time and date with GPS input
  // check for valid data
  if (gps.encode(ss.read())) {
    if (gps.date.isValid() && gps.time.isValid()) {
      Wire.beginTransmission(DS3231_I2C_ADDRESS);
      Wire.write(0);
      // set time
      Wire.write(decToBcd(gps.time.second()));
      Wire.write(decToBcd(gps.time.minute()));
      Wire.write(decToBcd(gps.time.hour()));
      // set date
      Wire.write(decToBcd(dayOfWeek));
      Wire.write(decToBcd(gps.date.day()));
      Wire.write(decToBcd(gps.date.month()));
      Wire.write(decToBcd(gps.date.year()));
      Wire.endTransmission();
    }
    else {
      //if the time and date are not valid rerun the function
      setDS3231time();
    }
  }
}
// read RTC time and date
void readDS3231time(byte *second, byte *minute, byte *hour, byte *dayOfWeek, byte *day, byte *month, byte *year) {
  Wire.beginTransmission(DS3231_I2C_ADDRESS);
  Wire.write(0);
  Wire.endTransmission();
  Wire.requestFrom(DS3231_I2C_ADDRESS, 7);

  *second = bcdToDec(Wire.read() & 0x7f);
  *minute = bcdToDec(Wire.read());
  *hour = bcdToDec(Wire.read() & 0x3f);
  *dayOfWeek = bcdToDec(Wire.read());
  *day = bcdToDec(Wire.read());
  *month = bcdToDec(Wire.read());
  *year = bcdToDec(Wire.read()) + 2000;
}
