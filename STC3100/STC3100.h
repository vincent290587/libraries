/***************************************************************************
  This is a library for the STC3100 pressure sensor


  These displays use I2C to communicate, 2 pins are required to interface.

 ***************************************************************************/
#ifndef __STC3100_H__
#define __STC3100_H__

#if (ARDUINO >= 100)
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif

#include <Adafruit_Sensor.h>

#ifdef __AVR_ATtiny85__
 #include "TinyWireM.h"
 #define Wire TinyWireM
#else
 #include <Wire.h>
#endif

/*=========================================================================
    I2C ADDRESS/BITS
    -----------------------------------------------------------------------*/
    #define STC3100_ADDRESS                (0x70)
	//#define STC3100_ADDRESS                0b01110000
/*=========================================================================*/

/*=========================================================================
    REGISTERS
    -----------------------------------------------------------------------*/

#define REG_MODE                0
#define REG_CONTROL             1
#define REG_CHARGE_LOW          2
#define REG_CHARGE_HIGH         3
#define REG_COUNTER_LOW         4
#define REG_COUNTER_HIGH        5
#define REG_CURRENT_LOW         6
#define REG_CURRENT_HIGH        7
#define REG_VOLTAGE_LOW         8
#define REG_VOLTAGE_HIGH        9
#define REG_TEMPERATURE_LOW     10
#define REG_TEMPERATURE_HIGH    11

/*=========================================================================*/

/*=========================================================================
    MODE SETTINGS
    -----------------------------------------------------------------------*/
    typedef enum
    {
      STC3100_MODE_ULTRALOWPOWER          = 0,
      STC3100_MODE_STANDARD               = 1,
      STC3100_MODE_HIGHRES                = 2,
      STC3100_MODE_ULTRAHIGHRES           = 3
    } stc3100_mode_t;

typedef union
{
    /**Used for simpler readings of the chip.*/
    uint8_t array[10];
    struct
    {
        uint8_t ChargeLow;
        /**Gas gauge charge data, bits 8-15*/
        uint8_t ChargeHigh;
        /**Number of conversions, bits 0-7*/
        uint8_t CounterLow;
        /**Number of conversions, bits 8-15*/
        uint8_t CounterHigh;
        /**Battery current value, bits 0-7*/
        uint8_t CurrentLow;
        /**Battery current value, bits 8-15*/
        uint8_t CurrentHigh;
        /**Battery voltage value, bits 0-7*/
        uint8_t VoltageLow;
        /**Battery voltage value, bits 8-15*/
        uint8_t VoltageHigh;
        /**Temperature value, bits 0-7*/
        uint8_t TemperatureLow;
        /**Temperature value, bits 8-15*/
        uint8_t TemperatureHigh;
    };
} tSTC31000Data;
	
	
typedef struct
{
    /**Value of the voltage in mV.*/
    float Voltage;
    /**Value of the current in mA*/
    float Current;
    /**Value of the temperature in ºC*/
    float Temperature;
    /**Value of the current charge of the battery in mAh*/
    float Charge;
} tBatteryData;
	
	/*=========================================================================*/




class STC3100
{
  public:
    STC3100(int32_t sensorID = -1);
  
    bool  begin(uint32_t r_sens, stc3100_mode_t mode = STC3100_MODE_ULTRAHIGHRES);
	
	void  getSensor(sensor_t*);
	
	float compute2Complement(uint8_t msb, uint8_t lsb);
	
    bool  getBatteryData(tBatteryData*);
	
  private:
    int32_t _sensorID;
	uint8_t _deviceID, _stc3100Mode;
	int32_t _r_sens;
	tSTC31000Data _stc_data;
	
	void  readChip();
	void  computeVoltage (float*);
	void  computeCharge  (float*);
	void  computeCurrent (float*);
	void  computeCounter(float*);
	void  computeTemp(float*);
};

#endif
