// Date and time functions using a DS3231 RTC with Wire lib
#include "Wire.h"
#define DS3231_I2C_ADDRESS 0x68

byte second, minute, hour, dayOfWeek, day, month, year;

void setup () {
  Serial.begin(9600);
  Wire.begin();
}

void loop () {
  readDS3231time(&second, &minute, &hour, &dayOfWeek, &day, &month,  &year);

  // First line - Date
  Serial.print(day < 10 ? "0" : "");
  Serial.print(day);
  Serial.print("/");
  Serial.print(month < 10 ? "0" : "");
  Serial.print(month);
  Serial.print("/");
  Serial.println(2000 + year);

  // Second line - Time
  Serial.print(hour < 10 ? "0" : "");
  Serial.print(hour);
  Serial.print(":");
  Serial.print(minute < 10 ? "0" : "");
  Serial.print(minute);
  Serial.print(":");
  Serial.print(second < 10 ? "0" : "");
  Serial.println(second);

  delay(1000);
}

byte decToBcd(byte val)
{
  return ((val / 10 * 16) + (val % 10));
}

byte bcdToDec(byte val)
{
  return ((val / 16 * 10) + (val % 16));
}

void setDS3231time(byte second, byte minute, byte hour, byte dayOfWeek, byte dayOfMonth, byte month, byte year)
{
  Wire.beginTransmission(DS3231_I2C_ADDRESS);
  Wire.write(0);
  Wire.write(decToBcd(second));
  Wire.write(decToBcd(minute));
  Wire.write(decToBcd(hour));
  Wire.write(decToBcd(dayOfWeek));
  Wire.write(decToBcd(dayOfMonth));
  Wire.write(decToBcd(month));
  Wire.write(decToBcd(year));
  Wire.endTransmission();
}
void readDS3231time(byte *second, byte *minute, byte *hour, byte *dayOfWeek, byte *dayOfMonth, byte *month, byte *year)
{
  Wire.beginTransmission(DS3231_I2C_ADDRESS);
  Wire.write(0);
  Wire.endTransmission();
  Wire.requestFrom(DS3231_I2C_ADDRESS, 7);

  *second = bcdToDec(Wire.read() & 0x7f);
  *minute = bcdToDec(Wire.read());
  *hour = bcdToDec(Wire.read() & 0x3f);
  *dayOfWeek = bcdToDec(Wire.read());
  *dayOfMonth = bcdToDec(Wire.read());
  *month = bcdToDec(Wire.read());
  *year = bcdToDec(Wire.read());
}
