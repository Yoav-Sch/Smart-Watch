#include <Wire.h>

#define DEVICE (0x53)    //ADXL345 device address
#define TO_READ (6)      //num of bytes we are going to read each time (two bytes for each axis)

byte buff[TO_READ];      //6 bytes buffer for saving data read from the device
int regAddress = 0x32;   //first axis-acceleration-data register on the ADXL345
int x, y, z;

int xavg, yavg, zavg, steps = 0, flag = 0;
int threshhold = 60.0;


void setup()
{
  Wire.begin();        // join i2c bus (address optional for master)
  Serial.begin(9600);  // start serial for output
  pinMode(LED_BUILTIN, OUTPUT);

  //Turning on the ADXL345
  writeTo(DEVICE, 0x2D, 0);     // ADXL345_POWER_CTL = Wakeup
  writeTo(DEVICE, 0x2D, 16);	// Auto_Sleep
  writeTo(DEVICE, 0x2D, 8);		// Measure
  
  calibrate();
}

void loop()
{
  readXYZ();

  Serial.print(x);
  Serial.print("\t");
  Serial.print(y);
  Serial.print("\t");
  Serial.print(z);
  Serial.println();
  Serial.print("Steps=");
  Serial.println(ArduinoPedometer());

  delay(10);
}

void readXYZ()
{
  readFrom(DEVICE, regAddress, TO_READ, buff); //read the acceleration data from the ADXL345

  //each axis reading comes in 10 bit resolution, ie 2 bytes.  Least Significat Byte first!!
  //thus we are converting both bytes in to one int
  x = (((int)buff[1]) << 8) | buff[0];
  y = (((int)buff[3]) << 8) | buff[2];
  z = (((int)buff[5]) << 8) | buff[4];
}


//Writes val to address register on device
void writeTo(int device, byte address, byte val) {
  Wire.beginTransmission(device); //start transmission to device
  Wire.write(address);        // send register address
  Wire.write(val);        // send value to write
  Wire.endTransmission(); //end transmission
}

//reads num bytes starting from address register on device in to buff array
void readFrom(int device, byte address, int num, byte buff[]) {
  Wire.beginTransmission(device); //start transmission to device
  Wire.write(address);        //sends address to read from
  Wire.endTransmission(); //end transmission

  Wire.beginTransmission(device); //start transmission to device
  Wire.requestFrom(device, num);    // request 6 bytes from device

  int i = 0;
  while (Wire.available())   //device may send less than requested (abnormal)
  {
    buff[i] = Wire.read(); // receive a byte
    i++;
  }
  Wire.endTransmission(); //end transmission
}

void calibrate() {
  int iter = 15;
  digitalWrite(LED_BUILTIN, HIGH);
  Serial.println("Calibrating......");

  int sumx = 0, sumy = 0, sumz = 0;

  for (int i = 0; i < iter; i++)
  {
    readXYZ();
    sumx = x + sumx;
    sumy = y + sumy;
    sumz = z + sumz;
    delay(30);
  }
  xavg = sumx / iter;
  yavg = sumy / iter;
  zavg = sumz / iter;

  Serial.print("xavg=");
  Serial.println(xavg);
  Serial.print("yavg=");
  Serial.println(yavg);
  Serial.print("zavg=");
  Serial.println(zavg);
  digitalWrite(LED_BUILTIN, LOW);
  Serial.println("Calibration Successful!");
}

//Get pedometer.
int ArduinoPedometer() {
  int totvect[15] = {0};
  int totave[15] = {0};
  for (int i = 0; i < 15; i++)
  {
    readXYZ();
    totvect[i] = sqrt(((x - xavg) * (x - xavg)) + ((y - yavg) * (y - yavg)) + ((z - zavg) * (z - zavg)));
    if (i == 0) {
      totave[i] = totvect[i];
    }
    else {
      totave[i] = (totvect[i] + totvect[i - 1]) / 2 ;
    }
    Serial.print("totave: ");
    Serial.println (totave[i]);
    delay(10);

    //cal steps
    if (totave[i] > threshhold && flag == 0)
    {
      steps = steps + 1;
      flag = 1;
    }
    else if (totave[i] > threshhold && flag == 1)
    {
      //do nothing
    }
    if (totave[i] < threshhold  && flag == 1)
    {
      flag = 0;
    }
    // Serial.print("steps=");
    // Serial.println(steps);
  }
  return (steps);
}
