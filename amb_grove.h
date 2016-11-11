//***************************************************************************/	
//  Hardware: Grove - I2C Color Sensor
//  Arduino IDE: Arduino-1.6
//  
//  Refactored version of the library by FrankieChu - www.seeedstudio.com
//	
//***************************************************************************/

// This file derived from Registers.h and GroveColorSensor.h.
// This software is written by Seeed Studio
// and is licensed under The LGPL License V2.1. 

#pragma once

// the I2C address for the color sensor 
#define AMB_GROVE_COLOR_SENSOR_ADDR          (0x39) 
#define AMB_GROVE_REG_CTL                    (0x80)
#define AMB_GROVE_REG_TIMING                 (0x81)
#define AMB_GROVE_REG_INT                    (0x82)
#define AMB_GROVE_REG_INT_SOURCE             (0x83)
#define AMB_GROVE_REG_ID                     (0x84)
#define AMB_GROVE_REG_GAIN                   (0x87)
#define AMB_GROVE_REG_LOW_THRESH_LOW_BYTE    (0x88)
#define AMB_GROVE_REG_LOW_THRESH_HIGH_BYTE   (0x89)
#define AMB_GROVE_REG_HIGH_THRESH_LOW_BYTE   (0x8A)
#define AMB_GROVE_REG_HIGH_THRESH_HIGH_BYTE  (0x8B)
#define AMB_GROVE_REG_BLOCK_READ             (0xD0) // REG_BLOCK_READ and
#define AMB_GROVE_REG_GREEN_LOW              (0xD0) // REG_GREEN_LOW are the same
#define AMB_GROVE_REG_GREEN_HIGH             (0xD1)
#define AMB_GROVE_REG_RED_LOW                (0xD2)
#define AMB_GROVE_REG_RED_HIGH               (0xD3)
#define AMB_GROVE_REG_BLUE_LOW               (0xD4)
#define AMB_GROVE_REG_BLUE_HIGH              (0xD5)
#define AMB_GROVE_REG_CLEAR_LOW              (0xD6)
#define AMB_GROVE_REG_CLEAR_HIGH             (0xD7)
#define AMB_GROVE_CTL_DAT_INIITIATE          (0x03)
#define AMB_GROVE_CLR_INT                    (0xE0)

// timing register
#define AMB_GROVE_SYNC_EDGE                 (0x40)
#define AMB_GROVE_INTEG_MODE_FREE           (0x00)
#define AMB_GROVE_INTEG_MODE_MANUAL         (0x10)
#define AMB_GROVE_INTEG_MODE_SYN_SINGLE     (0x20)
#define AMB_GROVE_INTEG_MODE_SYN_MULTI      (0x30)
#define AMB_GROVE_INTEG_PARAM_PULSE_COUNT1  (0x00)
#define AMB_GROVE_INTEG_PARAM_PULSE_COUNT2  (0x01)
#define AMB_GROVE_INTEG_PARAM_PULSE_COUNT4  (0x02)
#define AMB_GROVE_INTEG_PARAM_PULSE_COUNT8  (0x03)

// interrupt control register 
#define AMB_GROVE_INTR_STOP            (40)
#define AMB_GROVE_INTR_DISABLE         (0x00)
#define AMB_GROVE_INTR_LEVEL           (0x10)
#define AMB_GROVE_INTR_PERSIST_EVERY   (0x00)
#define AMB_GROVE_INTR_PERSIST_SINGLE  (0x01)

// interrupt souce register
#define AMB_GROVE_INT_SOURCE_GREEN  (0x00)
#define AMB_GROVE_INT_SOURCE_RED    (0x01)
#define AMB_GROVE_INT_SOURCE_BLUE   (0x10)
#define AMB_GROVE_INT_SOURCE_CLEAR  (0x03)

// gain register
#define AMB_GROVE_GAIN_1        (0x00)
#define AMB_GROVE_GAIN_4        (0x10)
#define AMB_GROVE_GAIN_16       (0x20)
#define AMB_GROVE_GAIN_64       (0x30)
#define AMB_GROVE_PRESCALER_1   (0x00)
#define AMB_GROVE_PRESCALER_2   (0x01)
#define AMB_GROVE_PRESCALER_4   (0x02)
#define AMB_GROVE_PRESCALER_8   (0x03)
#define AMB_GROVE_PRESCALER_16  (0x04)
#define AMB_GROVE_PRESCALER_32  (0x05)
#define AMB_GROVE_PRESCALER_64  (0x06)

namespace amb {

class GroveColorSensor
{
 public:
  int ledStatus;

  GroveColorSensor();
  ~GroveColorSensor();

  int Init();
  void Deinit();

  int readRGB(int* red, int* green, int* blue);
  int clearInterrupt();

 private:	 
  int i2c_device_;

  GroveColorSensor(const GroveColorSensor&) = delete;
  GroveColorSensor(GroveColorSensor&&) = delete;
  GroveColorSensor& operator=(const GroveColorSensor&) = delete;
  GroveColorSensor& operator=(const GroveColorSensor&&) = delete;
};

};
