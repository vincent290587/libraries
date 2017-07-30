/***************************************************************************
  This is a library for the STC3100 pressure sensor

  These displays use I2C to communicate, 2 pins are required to interface.

 ***************************************************************************/
#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif

#include <Wire.h>

#include <math.h>
#include <limits.h>

#include "STC3100.h"


/***************************************************************************
 PRIVATE FUNCTIONS
 ***************************************************************************/

/**************************************************************************/
/*!
    @brief  Writes an 8 bit value over I2C
*/
/**************************************************************************/
void STC3100::writeCommand(byte reg, byte value)
{
  Wire.beginTransmission((uint8_t)STC3100_ADDRESS);
  #if ARDUINO >= 100
    Wire.write((uint8_t)reg);
    Wire.write((uint8_t)value);
  #else
    Wire.send(reg);
    Wire.send(value);
  #endif
  Wire.endTransmission();
}

/**************************************************************************/
/*!
    @brief  Reads an 8 bit value over I2C
*/
/**************************************************************************/
void STC3100::read8(byte reg, uint8_t *value)
{
  Wire.beginTransmission((uint8_t)STC3100_ADDRESS);
  #if ARDUINO >= 100
    Wire.write((uint8_t)reg);
  #else
    Wire.send(reg);
  #endif
  Wire.endTransmission();
  Wire.requestFrom((uint8_t)STC3100_ADDRESS, (byte)1);
  #if ARDUINO >= 100
    *value = Wire.read();
  #else
    *value = Wire.receive();
  #endif  
  Wire.endTransmission();
}

/**************************************************************************/
/*!
    @brief  Reads a 16 bit value over I2C
*/
/**************************************************************************/
void STC3100::read16(byte reg, uint16_t *value)
{
  uint16_t data;
  Wire.beginTransmission((uint8_t)STC3100_ADDRESS);
  #if ARDUINO >= 100
    Wire.write((uint8_t)reg);
  #else
    Wire.send(reg);
  #endif
  Wire.endTransmission();
  Wire.requestFrom((uint8_t)STC3100_ADDRESS, (byte)2);
  #if ARDUINO >= 100
    while (Wire.available() < 2)
    ;
    data = ((uint16_t) Wire.read()) << 8;
    data |= Wire.read();
	*value = data;
  #else
    *value = (Wire.receive() << 8) | Wire.receive();
  #endif  
  Wire.endTransmission();
}

/**************************************************************************/
/*!
    @brief  Reads a signed 16 bit value over I2C
*/
/**************************************************************************/
void STC3100::readS16(byte reg, int16_t *value)
{
  uint16_t i;
  read16(reg, &i);
  *value = (int16_t)i;
}


void STC3100::readChip()
{
  uint8_t rec_bytes = 0;
  Wire.beginTransmission((uint8_t)STC3100_ADDRESS);
  Wire.write(REG_CHARGE_LOW);
  Wire.endTransmission();
  Wire.requestFrom((uint8_t)STC3100_ADDRESS, (byte)10);
  
  while (rec_bytes < 10) {
	  while (Wire.available() < 1);
	  _stc_data.array[rec_bytes++] = Wire.read();
  }
  
  Wire.endTransmission();
}

void STC3100::computeCounter()
{
	uint8_t tl=_stc_data.CounterLow, th=_stc_data.CounterHigh;
	uint32_t t;
	int val;
	
	t = th;
	t <<= 8;
	val = (t & 0xFF00) | tl;
	_counter = (float) val;
}

void STC3100::computeVoltage()
{

    uint8_t tl=_stc_data.VoltageLow, th=_stc_data.VoltageHigh;
	uint32_t t;
	int val;
	
	t = th;
	t <<= 8;
	val = (t & 0xFF00) | tl;

	_voltage = (float)  val * 0.00244;
	
}

void STC3100::computeCharge()
{

    uint8_t tl=_stc_data.ChargeLow, th=_stc_data.ChargeHigh;
	float val;
	
	val = compute2Complement(th, tl);
	_charge = (val * 6.7 / _r_sens);
	
}

void STC3100::computeCurrent()
{
    uint8_t tl=_stc_data.CurrentLow, th=_stc_data.CurrentHigh;
	float val;
	
	val = compute2Complement(th, tl);
	_current = (val * 11.77 / _r_sens);
	
}

float STC3100::compute2Complement(uint8_t msb, uint8_t lsb) {
	float ret;
	uint16_t t;
	uint16_t val;
	uint8_t tl=lsb, th=msb;
	
	if (th & 0b00100000) {
	  t = th << 8;
	  val = (t & 0xFF00) | (tl & 0x00FF);
	  val -= 1;
	  val = ~(val | 0b1110000000000000);
	  ret = (float)-val;
	} else {
	  t = (th & 0xFF) << 8;
	  val = (t & 0xFF00) | (tl & 0x00FF);
	  ret = (float)val;
	}
	
	return val;
}

void STC3100::computeTemp()
{
    uint8_t tl=_stc_data.TemperatureLow, th=_stc_data.TemperatureHigh;
	uint32_t t;
	int val;
	
	t = th;
	t <<= 8;
	val = (t & 0xFF00) | tl;
	
        _temperature = ((float) val * 0.125);
	
}


float STC3100::getCorrectedVoltage(float int_res) {

	float res = _voltage + int_res * _current / 1000.;
	return res;

}

/***************************************************************************
 CONSTRUCTOR
 ***************************************************************************/
 
STC3100::STC3100(int32_t sensorID) {
  _sensorID = sensorID;
}

/***************************************************************************
 PUBLIC FUNCTIONS
 ***************************************************************************/
 
/**************************************************************************/
/*!
    @brief  Setups the HW
*/
/**************************************************************************/
bool STC3100::begin(uint32_t r_sens, stc3100_mode_t mode)
{
  // Enable I2C
  Wire.begin();
  delay(100);
  // reset
  //writeCommand(REG_MODE, 0b01000000);
  //writeCommand(REG_MODE, 0b00010000);
  
  _r_sens = r_sens;
  
  writeCommand(REG_CONTROL, 0x02);
  writeCommand(REG_MODE, 0x10);
  
  read8(24, &_deviceID);

  /* Set the mode indicator */
  _stc3100Mode = mode;
    
  return true;
}




/**************************************************************************/
/*!
    @brief  Reads the sensor and returns the data as a sensors_event_t
*/
/**************************************************************************/
bool STC3100::refresh()
{
  readChip();
  
  computeVoltage ();
  computeCharge  ();
  computeCurrent ();
  computeTemp    ();
  
  return true;
}
