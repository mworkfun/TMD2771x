// Distributed with a free-will license.
// Use it any way you want, profit or free, provided it fits in the licenses of its associated works.
// http://www.mosiwi

#include<Wire.h>

// TMD2771 I2C address is 39(57)
#define Addr 0x39

void setup()
{
  // Initialise I2C communication as MASTER
  Wire.begin();
  // Initialise Serial Communication, set baud rate = 9600
  Serial.begin(9600);
  
  // Start I2C Transmission
  Wire.beginTransmission(Addr);
  // Select enable register
  Wire.write(0x00 | 0xA0);
  // Set power on, proximity and ALS enabled
  Wire.write(0x0F);
  // Stop I2C Transmission
  Wire.endTransmission();

  // Start I2C Transmission
  Wire.beginTransmission(Addr);
  // Select ALS time register
  Wire.write(0x01 | 0xA0);
  // Atime = 101 ms
  Wire.write(0xDB);
  // Stop I2C Transmission
  Wire.endTransmission();

  // Start I2C Transmission
  Wire.beginTransmission(Addr);
  // Select proximity time register
  Wire.write(0x02 | 0xA0);
  // Ptime = 2.72 ms
  Wire.write(0xFF);
  // Stop I2C Transmission
  Wire.endTransmission();

  // Start I2C Transmission
  Wire.beginTransmission(Addr);
  // Select Wait time register
  Wire.write(0x03 | 0xA0);
  // Wtime = 2.72 ms
  Wire.write(0xFF);
  // Stop I2C Transmission
  Wire.endTransmission();
  
  // Start I2C Transmission
  Wire.beginTransmission(Addr);
  // Select pulse count register
  Wire.write(0x0E | 0xA0);
  // Pulse count = 4
  Wire.write(0x04);
  // Stop I2C Transmission
  Wire.endTransmission();
  
  // Start I2C Transmission
  Wire.beginTransmission(Addr);
  // Select control register
  Wire.write(0x0F | 0xA0);
  // 120 mA LED strength, Proximtiy uses CH1 diode, 1x PGAIN, 1x AGAIN
  Wire.write(0x20);
  // Stop I2C Transmission
  Wire.endTransmission();
  delay(800);  
}

void loop()
{
  unsigned int data[6];
  
  // Start I2C Transmission
  Wire.beginTransmission(Addr);
  // Select data register
  Wire.write(0x14 | 0xA0);
  // Stop I2C Transmission
  Wire.endTransmission();
  
  // Request 6 bytes of data
  Wire.requestFrom(Addr, 6);
  
  // Read 6 bytes of data
  // c0Data lsb, c0Data msb, c1Data lsb, c1Data msb, proximity lsb, proximity msb
  if(Wire.available() == 6)
  {
    data[0] = Wire.read();
    data[1] = Wire.read();
    data[2] = Wire.read();
    data[3] = Wire.read();
    data[4] = Wire.read();
    data[5] = Wire.read();
  }
  
  // Convert the data
  int c0Data = (data[1] * 256) + data[0];
  int c1Data = (data[3] * 256) + data[2];
  double proximity = (data[5] * 256.0) + data[4];
  float CPL = (101.0) / 24.0;
  float luminance1 = (1 * c0Data - 2 * c1Data) / CPL;
  float luminance2 = (0.6 * c0Data - 1.00 * c1Data) / CPL;
  float luminance = 0.0;
  
  if((luminance1 > 0) && (luminance1 > luminance2))
  {
    luminance = luminance1;
  }
  else if((luminance2 > 0) && (luminance2 > luminance1))
  {
    luminance = luminance2;
  }
  
  // Output data to serial monitor
  Serial.print("Ambient Light luminance : ");
  Serial.print(luminance);
  Serial.println(" lux");
  Serial.print("Proximity of the device : ");
  Serial.println(proximity);
  delay(1000);
}
