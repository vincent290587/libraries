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
static void writeCommand(byte reg, byte value)
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
static void read8(byte reg, uint8_t *value)
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
static void read16(byte reg, uint16_t *value)
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
static void readS16(byte reg, int16_t *value)
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

void STC3100::computeCounter(float *counter)
{
	uint8_t tl=_stc_data.CounterLow, th=_stc_data.CounterHigh;
	uint32_t t;
	int val;
	
	t = th;
	t <<= 8;
	val = (t & 0xFF00) | tl;
	*counter = (float) val;
}

void STC3100::computeVoltage(float *voltage)
{

    uint8_t tl=_stc_data.VoltageLow, th=_stc_data.VoltageHigh;
	uint32_t t;
	int val;
	
	t = th;
	t <<= 8;
	val = (t & 0xFF00) | tl;
	*voltage = (float) val * 0.00244;
	
}

void STC3100::computeCharge(float *charge)
{

    uint8_t tl=_stc_data.ChargeLow, th=_stc_data.ChargeHigh;
	uint32_t t;
	int val;
	
	t = th;
	t <<= 8;
	val = (t & 0xFF00) | tl;
	*charge = ((float) val * 6.7 / _r_sens);
	
}

void STC3100::computeCurrent(float *current)
{
    uint8_t tl=_stc_data.CurrentLow, th=_stc_data.CurrentHigh;
	uint16_t t;
	float val;
	
	val = compute2Complement(th, tl);
	*current = (val * 11.77 / _r_sens);
	
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

void STC3100::computeTemp(float *temp)
{
    uint8_t tl=_stc_data.TemperatureLow, th=_stc_data.TemperatureHigh;
	uint32_t t;
	int val;
	
	t = th;
	t <<= 8;
	val = (t & 0xFF00) | tl;
	*temp = ((float) val * 0.125);
	
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
    @brief  Provides the sensor_t data for this sensor
*/
/**************************************************************************/
void STC3100::getSensor(sensor_t *sensor)
{
  /* Clear the sensor_t object */
  memset(sensor, 0, sizeof(sensor_t));

  /* Insert the sensor name in the fixed length char array */
  strncpy (sensor->name, "STC3100", sizeof(sensor->name) - 1);
  sensor->name[sizeof(sensor->name)- 1] = 0;
  sensor->version     = _deviceID;
  sensor->sensor_id   = _sensorID;
  //sensor->type        = SENSOR_TYPE_VOLTAGE;
}

/**************************************************************************/
/*!
    @brief  Reads the sensor and returns the data as a sensors_event_t
*/
/**************************************************************************/
bool STC3100::getBatteryData(tBatteryData *batt)
{
  readChip();
  
  memset(batt, 0, sizeof(tBatteryData));

  computeVoltage (&batt->Voltage);
  computeCharge  (&batt->Charge);
  computeCurrent (&batt->Current);
  computeTemp    (&batt->Temperature);
  
  return true;
}
