//***************************************************************************/	
//  Hardware: Grove - I2C Color Sensor
//  Arduino IDE: Arduino-1.6
//  
//  Refactored version of the library by FrankieChu - www.seeedstudio.com
//	
//***************************************************************************/

// This file derived from GroveColorSensor.cpp.
// This software is written by Seeed Studio
// and is licensed under The LGPL License V2.1. 

#include "amb_grove.h"

#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <cerrno>
#include <chrono>
#include <cmath>
#include <cstring>
#include <thread>

#include <linux/i2c-dev.h>

#define min(X, Y) (((X) < (Y)) ? (X) : (Y))
#define max(X, Y) (((X) < (Y)) ? (Y) : (X))

namespace amb {

using ::std::chrono::milliseconds;
using ::std::this_thread::sleep_for;

GroveColorSensor::GroveColorSensor() : i2c_device_(-1) {	
}

GroveColorSensor::~GroveColorSensor() {	
  Deinit();
}

int GroveColorSensor::Init() {	
  unsigned char buffer[2];
  int result;

  i2c_device_ = open("/dev/i2c-0", O_RDWR);
  if (i2c_device_ < 0)
    return errno;

  result = ioctl(i2c_device_, I2C_SLAVE, AMB_GROVE_COLOR_SENSOR_ADDR);
  if (result < 0)
    return -ENXIO;

  buffer[0] = AMB_GROVE_REG_TIMING;
  buffer[1] = AMB_GROVE_INTEG_MODE_FREE | AMB_GROVE_INTEG_PARAM_PULSE_COUNT1;
  result = write(i2c_device_, buffer, 2);
  if (result < 0)
    return -EIO;
  sleep_for(milliseconds(10));

  buffer[0] = AMB_GROVE_REG_INT_SOURCE;
  buffer[1] = AMB_GROVE_INT_SOURCE_CLEAR;
  result = write(i2c_device_, buffer, 2);
  if (result < 0)
    return -EIO;
  sleep_for(milliseconds(10));

  buffer[0] = AMB_GROVE_REG_INT;
  buffer[1] = AMB_GROVE_INTR_LEVEL | AMB_GROVE_INTR_PERSIST_EVERY;
  result = write(i2c_device_, buffer, 2);
  if (result < 0)
    return -EIO;
  sleep_for(milliseconds(10));

  buffer[0] = AMB_GROVE_REG_GAIN,
  buffer[1] = AMB_GROVE_GAIN_1 | AMB_GROVE_PRESCALER_1;
  result = write(i2c_device_, buffer, 2);
  if (result < 0)
    return -EIO;
  sleep_for(milliseconds(10));

  buffer[0] = AMB_GROVE_REG_CTL;
  buffer[1] = AMB_GROVE_CTL_DAT_INIITIATE;
  result = write(i2c_device_, buffer, 2);
  if (result < 0)
    return -EIO;
  sleep_for(milliseconds(10));

  return 0;
}

void GroveColorSensor::Deinit() {	
  close(i2c_device_);
}

int GroveColorSensor::clearInterrupt()
{
  unsigned char buffer[1];

  buffer[0] = AMB_GROVE_CLR_INT;
  int result = write(i2c_device_, buffer, 1);
  if (result < 0)
    return -EIO;

  return 0;
}

int GroveColorSensor::readRGB(int *red, int *green, int *blue)
{
  int result;
  int length_read;
  unsigned char data[8];

  data[0] = AMB_GROVE_REG_BLOCK_READ;
  result = write(i2c_device_, data, 1);
  if (result < 0)
    return -EIO;
  sleep_for(milliseconds(100));

  for (int idx = 0; idx < 8; ++idx)
    data[idx] = 0;
  length_read = read(i2c_device_, data, 8);
  if (length_read != 8)
    return -EIO;

  int greenLocal = static_cast<int>(data[1]) * 256 + static_cast<int>(data[0]);
  int redLocal   = static_cast<int>(data[3]) * 256 + static_cast<int>(data[2]);
  int blueLocal  = static_cast<int>(data[5]) * 256 + static_cast<int>(data[4]);

  double tmp;
  int maxColor;

  if (ledStatus == 1) {
    redLocal  = redLocal * 1.70;
    blueLocal = blueLocal * 1.35;

    maxColor = max(redLocal, greenLocal);
    maxColor = max(maxColor, blueLocal);

    if (maxColor > 255) {
      tmp     = 250.0 / maxColor;
      greenLocal *= tmp;
      redLocal   *= tmp;
      blueLocal  *= tmp;
    }
  }

  if (ledStatus == 0) {
    maxColor = max(redLocal, greenLocal);
    maxColor = max(maxColor, blueLocal);

    tmp     = 250.0 / maxColor;
    greenLocal *= tmp;
    redLocal   *= tmp;
    blueLocal  *= tmp;
  }

  int minColor = min(redLocal, greenLocal);
  minColor = min(maxColor, blueLocal);
  maxColor = max(redLocal, greenLocal);
  maxColor = max(maxColor, blueLocal);

  int greenTmp = greenLocal;
  int redTmp   = redLocal;
  int blueTmp  = blueLocal;

  if (redLocal < 0.8 * maxColor && redLocal >= 0.6 * maxColor)
    redLocal *= 0.4;
  else if (redLocal < 0.6 * maxColor)
    redLocal *= 0.2;

  if (greenLocal < 0.8 * maxColor && greenLocal >= 0.6 * maxColor) {
    greenLocal *= 0.4;
  }
  else if (greenLocal < 0.6 * maxColor) {
    if (maxColor == redTmp && greenTmp >= 2 * blueTmp &&
        greenTmp >= 0.2 * redTmp)
      greenLocal *= 5;
    greenLocal *= 0.2;
  }

  if (blueLocal < 0.8 * maxColor && blueLocal >= 0.6 * maxColor) {
    blueLocal *= 0.4;
  }
  else if (blueLocal < 0.6 * maxColor) {
    if (maxColor == redTmp && greenTmp >= 2 * blueTmp &&
        greenTmp >= 0.2 * redTmp)
      blueLocal *= 0.5;

    if (maxColor == redTmp && greenTmp <= blueTmp && blueTmp >= 0.2 * redTmp)
      blueLocal  *= 5;
    blueLocal *= 0.2;
  }

  minColor = min(redLocal, greenLocal);
  minColor = min(maxColor, blueLocal);

  if (maxColor == greenLocal &&
      redLocal >= 0.85 * maxColor &&
      minColor == blueLocal) {
    redLocal = maxColor;
    blueLocal *= 0.4;
  }

  *red   = redLocal;
  *green = greenLocal;
  *blue  = blueLocal;

  return 0;
}

};
