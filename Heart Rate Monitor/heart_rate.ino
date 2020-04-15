
//It's a modified version of the HeartRate library example
//Refer to www.surtrtech.com for more details or SurtrTech YouTube channel

#include <Wire.h>
#include "MAX30105.h"//MAX3010x library
#include "heartRate.h"//Heart rate calculating algorithm

MAX30105 HR_Sensor;

const byte RATE_SIZE = 4; //Increase this for more averaging. 4 is good.
byte rates[RATE_SIZE]; //Array of heart rates
byte rateSpot = 0;
long lastBeat = 0; //Time at which the last beat occurred
float beatsPerMinute;
int beatAvg;

void setup() {
  // Initialize sensor
  Serial.begin(9600);
  HR_Sensor.begin(Wire, I2C_SPEED_FAST); //Use default I2C port, 400kHz speed
  HR_Sensor.setup(); //Configure sensor with default settings
  HR_Sensor.setPulseAmplitudeRed(0x0A); //Turn Red LED to low to indicate sensor is running

}

void loop() {
  long irValue = HR_Sensor.getIR();//Reading the IR value it will permit us to know if there's a finger on the sensor or not
  //Also detecting a heartbeat
  if (irValue > 7000) {//If a finger is detected
    Serial.print("BPM:");
    Serial.println(beatAvg);

    if (checkForBeat(irValue) == true)//If a heart beat is detected
    {
      Serial.print("BPM:");
      Serial.println(beatAvg);
      //We sensed a beat!
      long delta = millis() - lastBeat;                   //Measure duration between two beats
      lastBeat = millis();

      beatsPerMinute = 60 / (delta / 1000.0);           //Calculating the BPM

      if (beatsPerMinute < 255 && beatsPerMinute > 20)               //To calculate the average we strore some values (4) then do some math to calculate the average
      {
        rates[rateSpot++] = (byte)beatsPerMinute; //Store this reading in the array
        rateSpot %= RATE_SIZE; //Wrap variable

        //Take average of readings
        beatAvg = 0;
        for (byte x = 0 ; x < RATE_SIZE ; x++)
          beatAvg += rates[x];
        beatAvg /= RATE_SIZE;
      }
    }

  }
  if (irValue < 7000) {      //If no finger is detected it inform the user and put the average BPM to 0 or it will be stored for the next measure
    beatAvg = 0;
    Serial.println("Please Place your finger");
  }

}
