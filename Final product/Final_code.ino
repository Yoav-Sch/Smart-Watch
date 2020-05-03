/*
  זה הוא קוד לא גמור הנועד לפרויקט הגמור
  COVID-19 נמנה מעיתנו לסיים ולבחון את הקוד על הפרוקט בגלל
  מאת: יואב שכנר וטל גורל
*/

//// Included Libs
// RTC Libs
#include <Wire.h>
// GPS Libs
#include <TinyGPS++.h>
#include <SoftwareSerial.h>
// Display Libs
#include <SPI.h>
#include "SSD1306Ascii.h"
#include "SSD1306AsciiSpi.h"

////pre setup
////general
#define Start_button_pin 12
#define Switch_buton_pin 8
#define Lap_buton_pin 6
int Start;
bool runing = 0;
////RTC
byte second, minute, hour, dayOfWeek, day, month, year;
#define DS3231_I2C_ADDRESS 0x68
////GPS
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
////display
// Define Pins
#define CS_PIN    7
#define RST_PIN   8
#define DC_PIN    9
#define MOSI_PIN 11
#define CLK_PIN  13
SSD1306AsciiSpi oled;

void setup() {
  ////GPS Set Up
  Serial.begin(9600);
  ss.begin(GPSBaud);
  ////RTC set up
  Wire.begin();
  //// Display Set Up
  oled.begin(&Adafruit128x64, CS_PIN, DC_PIN, RST_PIN);
  // Select Font
  oled.setFont(ZevvPeep8x16);
  // Clear Display
  oled.clear();
  oled.println("Starting...");
  // set RTC time and date with GPS input
  setDS3231time();
  oled.clear();
}
void loop() {
  //read time from RTC to update time and val
  readDS3231time(&second, &minute, &hour, &dayOfWeek, &day, &month,  &year);
  //display time in line 0
  oled.setCursor(4 * 8, 0 * 2); //(x * 8, y * 2) x-column y-row
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
  if (digitalRead(Start_button_pin) == 1) {
    runing = ( runing == 1 ? "0" : "1");
    Start = millis();
    if (gps.encode(ss.read())) {
      LAT = gps.location.lat();
      LONG = gps.location.lng();
    }
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
    oled.setCursor(3 * 8, 1 * 2); //(x * 8, y * 2) x-column y-row
    oled.print(day < 10 ? "0" : "");
    oled.print(day);
    oled.print("/");
    oled.print(month < 10 ? "0" : "");
    oled.print(month);
    oled.print("/");
    oled.print(year);
    //display steps in line 2
    oled.setCursor(2 * 8, 2 * 2); //(x * 8, y * 2) x-column y-row
    oled.print("Steps:");
    oled.print(steps);
  }
}

//Functions

void Distance() { //distance between curret location to last
  if (gps.encode(ss.read())) { // encode data
    distance = TinyGPSPlus.distanceBetween(gps.location.lat(), gps.location.lng(), LAT, LONG) + distance;
    LAT = gps.location.lat();
    LONG = gps.location.lng();
  }
  if (distance > 1000) { //distance in km
    oled.setCursor(5 * 8, 2 * 2); //(x * 8, y * 2) x-column y-row
    oled.print(distance / 1000, 1); //km
    oled.print("km");
  }
  else { //distance in m
    oled.setCursor(6 * 8, 2 * 2); //(x * 8, y * 2) x-column y-row
    oled.print(distance, 0); //m
    oled.print("m");
  }
}

void Timer() {
  readDS3231time(&second, &minute, &hour, &dayOfWeek, &day, &month,  &year);
  if (bool fl == 1) { //timer is in sec
    timer = (((day - day_s) * 24 + (hour - hour_s)) * 60 + (minute - minute_s)) * 60 + (second - second_s);
    if (timer < 60) { //Time in sec range
      oled.setCursor(4 * 8, 1 * 2); //(x * 8, y * 2) x-column y-row
      oled.print("Time ");
      oled.print(timer);
    }
    else if ((timer < 3600) && (timer > 60)) { //Time in min range
      oled.setCursor(3 * 8, 1 * 2); //(x * 8, y * 2) x-column y-row
      oled.print("Time ");
      oled.print(trunc(timer / 60)); //min
      oled.print(":");
      oled.print(timer - trunc(timer / 60) * 60); //sec
    }
    else { //Time in hour range
      oled.setCursor(1 * 8, 1 * 2); //(x * 8, y * 2) x-column y-row
      oled.print("Time ");
      oled.print(trunc(timer / 3600)); //hour
      oled.print(":");
      oled.print(trunc((timer - trunc(timer / 3600) * 3600) / 60)); //min
      oled.print(":");
      oled.print(timer - (trunc(timer / 3600) * 3600) + trunc((timer - trunc(timer / 3600) * 3600) / 60) * 60); //sec
    }
  }
  else {
    fl = 1;
    int second_s, minute_s, hour_s, day_s;
    second_s = second;
    minute_s = minute;
    hour_s = hour;
    day_s = day;
  }
}

byte decToBcd(byte val) { // convert dec to bin
  return ((val / 10 * 16) + (val % 10));
}

byte bcdToDec(byte val) { // convert bin to dec
  return ((val / 16 * 10) + (val % 16));
}

void setDS3231time() { // set RTC time and date with GPS input
  if (gps.encode(ss.read())) { // encode data
    // check for valid data
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
