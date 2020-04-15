
//Functions for End_Game

void Distance() {
  //distance between curret location to last
  distance = TinyGPSPlus.distanceBetween(gps.location.lat(), gps.location.lng(), LAT, LONG) + distance;
  LAT = gps.location.lat();
  LONG = gps.location.lng();
  //distance in km
  if (distance > 1000) {
    oled.setCursor(5 * 8, 2 * 2);
    oled.print(distance / 1000, 1);//km
    oled.print("km");
  }
  //distance in m
  else {
    oled.setCursor(6 * 8, 2 * 2);
    oled.print(distance, 0);//m
    oled.print("m");
  }
}

void Timer() {
  readDS3231time(&second, &minute, &hour, &dayOfWeek, &day, &month,  &year);
  if (bool fl == 1) {
    //timer is in sec
    timer = (((day - day_s) * 24 + (hour - hour_s)) * 60 + (minute - minute_s)) * 60 + (second - second_s);
    //Time in sec
    if (timer < 60) {
      oled.setCursor(4 * 8, 1 * 2);
      oled.print("Time ");
      oled.print(timer);
    }
    //Time in min
    else if ((timer < 3600) && (timer > 60)) {
      oled.setCursor(3 * 8, 1 * 2);
      oled.print("Time ");
      oled.print(trunc(timer / 60)); //min
      oled.print(":");
      oled.print(timer - trunc(timer / 60) * 60); //sec
    }
    //Time in hour
    else {
      oled.setCursor(1 * 8, 1 * 2);
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

//float getTemperature()
//{
//  uint8_t msb, lsb;
//  Wire.beginTransmission(DS3231_I2C_ADDRESS);
//  Wire.write(0x11);
//  Wire.endTransmission();
//
//  Wire.requestFrom(DS3231_I2C_ADDRESS, 2);
//  msb = Wire.read();
//  lsb = Wire.read();
//
//  return (float) msb + (lsb >> 6) * 0.25f;
//}

//boolean lostPower()
//{
//  Wire.beginTransmission(DS3231_I2C_ADDRESS);
//  Wire.write(0x0F);
//  Wire.endTransmission();
//
//  Wire.requestFrom(DS3231_I2C_ADDRESS, 1);
//  return Wire.read() >> 7;
//}


//void getDATETIME() {
//  char buff[16];
//  static const char month_names[] = "JanFebMarAprMayJunJulAugSepOctNovDec";
//  sscanf(__DATE__, "%s %d %d", buff, &day, &year);
//  month = (strstr(month_names, buff) - month_names) / 3 + 1;
//  sscanf(__TIME__, "%d:%d:%d", &hour, &minute, &second);
//}
