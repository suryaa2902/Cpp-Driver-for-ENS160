#include "ens160_i2c.h"

ENS160::ENS160(i2c_inst_t *i2c_device_bus, int i2c_device_address)
{
    this->i2c_address = i2c_device_address;
    this->i2cbus = i2c_device_bus;
}
bool ENS160::ping(int address)
{
    int ret;
    uint8_t rxdata;
    ret = i2c_read_blocking(this->i2cbus, address, &rxdata, 1, false);
    if (ret != PICO_ERROR_GENERIC)
        return true;
    return false;
}

int32_t ENS160::readRegisterRegion(uint8_t reg, uint8_t *data, uint8_t length)
{
    i2c_write_blocking(this->i2cbus, this->i2c_address, const_cast<uint8_t*>(&reg), 1, true);
	return i2c_read_blocking(this->i2cbus, this->i2c_address, data, length, false);
}

int32_t ENS160::writeRegisterRegion(uint8_t *data, uint8_t length)
{
    return i2c_write_blocking(this->i2cbus, this->i2c_address, const_cast<uint8_t*>(data), length, true);
}

int32_t ENS160::writeRegisterRegion(uint8_t reg, uint8_t data, uint8_t length)
{
    uint8_t buf[] = {reg, data};
    return i2c_write_blocking(this->i2cbus, this->i2c_address, buf, length, true);
}

//////////////////////////////////////////////////////////////////////////////
// getUniqueID()
// Gets the device's unique ID
uint16_t ENS160::getUniqueID()
{
    int32_t retVal;
	uint8_t tempVal[2] = {0}; 
	uint16_t id; 

    retVal = readRegisterRegion(SFE_ENS160_PART_ID, tempVal, 2);

	id = tempVal[0];
	id |= tempVal[1] << 8;

	if( retVal != 0 )
		return 0;

	return id; 
}

///////////////////////////////////////////////////////////////////////
// isConnected()
//  Parameter   Description
//  ---------   -----------------------------
//  retVal      true if device is connected, false if not connected

bool ENS160::isConnected()
{
	uint16_t uniqueID; 
	uniqueID = getUniqueID(); 
	if( uniqueID != ENS160_DEVICE_ID )
		return false;
	return true;
}

bool ENS160::init()
{
    if(!this->ping(this->i2c_address))
        return false;
    return this->isConnected();
}

//////////////////////////////////////////////////////////////////////////////
// setOperatingMode()
// Sets the operating mode: Deep Sleep (0x00), Idle (0x01), Standard (0x02), Reset (0xF0)
//
//  Parameter    Description
//  ---------    -----------------------------
//  val					 The desired operating mode to set. 
bool ENS160::setOperatingMode(uint8_t val)
{
	int32_t retVal;

	if( val > SFE_ENS160_RESET )
		return false;

	retVal = this->writeRegisterRegion(SFE_ENS160_OP_MODE, val, 1);

	if( retVal != 0 )
		return false;

	return true; 
}

//////////////////////////////////////////////////////////////////////////////
// getOperatingMode()
//
// Gets the current operating mode: Deep Sleep (0x00), Idle (0x01), Standard (0x02), Reset (0xF0)

int8_t ENS160::getOperatingMode()
{
	int32_t retVal;
	uint8_t tempVal; 

	retVal = readRegisterRegion(SFE_ENS160_OP_MODE, &tempVal, 1);

	if( retVal != 0 )
		return -1;

	return tempVal; 
}

//////////////////////////////////////////////////////////////////////////////
// configureInterrupt()
//
// Changes all of the settings within the interrupt configuration register.
//
//  Parameter    Description
//  ---------    -----------------------------
//  val					 The desired configuration settings.

bool ENS160::configureInterrupt(uint8_t val)
{
	int32_t retVal;

	retVal = writeRegisterRegion(SFE_ENS160_CONFIG, val, 1);

	if( retVal != 0 )
		return false;

	return true;
}


//////////////////////////////////////////////////////////////////////////////
// setInterrupt()
//
// Enables the interrupt.
//
//  Parameter    Description
//  ---------    -----------------------------
//  enable			 Turns on or off the interrupt. 

bool ENS160::enableInterrupt(bool enable)
{
	int32_t retVal;
	uint8_t tempVal; 

	retVal = readRegisterRegion(SFE_ENS160_CONFIG, &tempVal, 1);

	if( retVal != 0 )
		return false;
	
	tempVal = (tempVal | (uint8_t)enable); 

	retVal = writeRegisterRegion(SFE_ENS160_CONFIG, tempVal, 1);

	if( retVal != 0 )
		return false;

	return true;
}

//////////////////////////////////////////////////////////////////////////////
// setInterruptPolarity()
//
// Changes the polarity of the interrupt: active high or active low. By default
// this value is set to zero or active low. 
//
//  Parameter    Description
//  ---------    -----------------------------
//  activeHigh   Changes active state of interrupt from high to low. 

bool ENS160::setInterruptPolarity(bool activeHigh)
{
	int32_t retVal;
	uint8_t tempVal; 

	retVal = readRegisterRegion(SFE_ENS160_CONFIG, &tempVal, 1);
	
	if( retVal != 0 )
		return false;

	tempVal = (tempVal | (activeHigh << 6)); 

	retVal = writeRegisterRegion(SFE_ENS160_CONFIG, tempVal, 1);

	if( retVal != 0 )
		return false;

	return true;
}

//////////////////////////////////////////////////////////////////////////////
// getInterruptPolarity()
//
// Retrieves the polarity of the physical interrupt. 

int8_t ENS160::getInterruptPolarity()
{
	int32_t retVal;
	uint8_t tempVal; 

	retVal = readRegisterRegion(SFE_ENS160_CONFIG, &tempVal, 1);
	
	if( retVal != 0 )
		return -1;

	tempVal &= 0x40;

	return (tempVal >> 6);
}

//////////////////////////////////////////////////////////////////////////////
// setInterruptDrive()
//
// Changes the pin drive of the interrupt: open drain (default) to push/pull
//
//  Parameter    Description
//  ---------    -----------------------------
//  pushPull     Changes the drive of the pin. 

bool ENS160::setInterruptDrive(bool pushPull)
{
	int32_t retVal;
	uint8_t tempVal; 

	retVal = readRegisterRegion(SFE_ENS160_CONFIG, &tempVal, 1);

	if( retVal != 0 )
		return false;
	
	tempVal = (tempVal | (uint8_t)(pushPull << 5)); 

	retVal = writeRegisterRegion(SFE_ENS160_CONFIG, tempVal, 1);

	if( retVal != 0 )
		return false;

	return true;
}


//////////////////////////////////////////////////////////////////////////////
// setDataInterrupt()
//
// Routes the data ready signal to the interrupt pin.
//
//  Parameter    Description
//  ---------    -----------------------------
//  enable			 Self-explanatory: enables or disables data ready on interrupt.

bool ENS160::setDataInterrupt(bool enable)
{
	int32_t retVal;
	uint8_t tempVal; 

	retVal = readRegisterRegion(SFE_ENS160_CONFIG, &tempVal, 1);

	if( retVal != 0 )
		return false;
	
	tempVal = (tempVal | (uint8_t)(enable << 1)); 

	retVal = writeRegisterRegion(SFE_ENS160_CONFIG, tempVal, 1);

	if( retVal != 0 )
		return false;

	return true; 
}


//////////////////////////////////////////////////////////////////////////////
// setGPRInterrupt()
//
// Routes the general purporse read register signal to the interrupt pin.
//
//  Parameter    Description
//  ---------    -----------------------------
//  enable			 Self-explanatory: enables or disables general purpos read interrupt.

bool ENS160::setGPRInterrupt(bool enable)
{
	int32_t retVal;
	uint8_t tempVal; 

	retVal = readRegisterRegion(SFE_ENS160_CONFIG, &tempVal, 1);

	if( retVal != 0 )
		return false;
	
	tempVal = (tempVal | (uint8_t)(enable << 3)); 
	
	retVal = writeRegisterRegion(SFE_ENS160_CONFIG, tempVal, 1);

	if( retVal != 0 )
		return false;

	return true; 
}


//////////////////////////////////////////////////////////////////////////////
// getAppVer()
//
// Retrieves the 24 bit application version of the device.

uint32_t ENS160::getAppVer()
{
	int32_t retVal;
	uint8_t tempVal[3] = {0};
	uint32_t version;

	retVal = readRegisterRegion(SFE_ENS160_GPR_READ4, tempVal, 3);

	if( retVal != 0 )
		return 0;

	version = tempVal[0];
	version |= tempVal[1] << 8;
	version |= tempVal[2] << 16;

	return version;
}

//////////////////////////////////////////////////////////////////////////////
// setTempCompensation()
//
// The ENS160 can use temperature data to help give more accurate sensor data. 
//
//  Parameter    Description
//  ---------    -----------------------------
//  kelvinConversion	 The given temperature in Kelvin 

bool ENS160::setTempCompensation(float tempKelvin)
{
	int32_t retVal;
	uint8_t tempVal[3] = {0};
	uint16_t kelvinConversion = tempKelvin; 

	kelvinConversion = kelvinConversion * 64; // convert value - fixed equation pg. 29 of datasheet
    tempVal[0] = SFE_ENS160_TEMP_IN;
	tempVal[1] = (kelvinConversion & 0x00FF);
	tempVal[2] = (kelvinConversion & 0xFF00) >> 8;

	retVal = writeRegisterRegion(tempVal, 2);

	if( retVal != 0 )
		return false;

	return true; 
}


//////////////////////////////////////////////////////////////////////////////
// setTempCompensationCelsius()
//
// The ENS160 can use temperature data to help give more accurate sensor data. 
//
//  Parameter    Description
//  ---------    -----------------------------
//  tempCelsius	 The given temperature in Celsius 

bool ENS160::setTempCompensationCelsius(float tempCelsius)
{
	float kelvinConversion = tempCelsius + 273.15; 

	if( setTempCompensation(kelvinConversion) )
			return true;

	return false; 
}


//////////////////////////////////////////////////////////////////////////////
// setRHCompensation()
//
// The ENS160 can use relative Humidiy data to help give more accurate sensor data. 
//
//  Parameter    Description
//  ---------    -----------------------------
//  humidity	   The given relative humidity. 

bool ENS160::setRHCompensation(uint16_t humidity)
{
	int32_t retVal;
	uint8_t tempVal[3] = {0};

	humidity = humidity * 512; // convert value - fixed equation pg. 29 in datasheet. 
    tempVal[0] = SFE_ENS160_RH_IN;
	tempVal[1] = (humidity & 0x00FF);
	tempVal[2] = (humidity & 0xFF00) >> 8;

	retVal = writeRegisterRegion(tempVal, 2);

	if( retVal != 0 )
		return false;

	return true; 
}

//////////////////////////////////////////////////////////////////////////////
// setRHCompensationFloat()
//
// The ENS160 can use relative Humidiy data to help give more accurate sensor data. 
//
//  Parameter    Description
//  ---------    -----------------------------
//  humidity	   The given relative humidity. 

bool ENS160::setRHCompensationFloat(float humidity)
{
	uint16_t humidityConversion = (uint16_t)humidity;

	if( setRHCompensation(humidityConversion) )
		return false;

	return true; 
}

//////////////////////////////////////////////////////////////////////////////
// checkDataStatus()
//
// This checks the if the NEWDAT bit is high indicating that new data is ready to be read. 
// The bit is cleared when data has been read from their registers. 

bool ENS160::checkDataStatus()
{
	int32_t retVal;
	uint8_t tempVal; 

	retVal = readRegisterRegion(SFE_ENS160_DEVICE_STATUS, &tempVal, 1);

	if( retVal != 0 )
		return false; 

	tempVal &= 0x02; 

	if( tempVal == 0x02 )
		return true;

	return false;
}


//////////////////////////////////////////////////////////////////////////////
// checkGPRStatus()
//
// This checks the if the NEWGPR bit is high indicating that there is data in the
// general purpose read registers. The bit is cleared the relevant registers have been
// read. 

bool ENS160::checkGPRStatus()
{
	int32_t retVal;
	uint8_t tempVal; 

	retVal = readRegisterRegion(SFE_ENS160_DEVICE_STATUS, &tempVal, 1);

	if( retVal != 0 )
		return false; 

	tempVal &= 0x01;

	if( tempVal == 0x01 )
		return true;

	return false;
}


//////////////////////////////////////////////////////////////////////////////
// getFlags()
//
// This checks the status "flags" of the device (0-3).

uint8_t ENS160::getFlags()
{
	int32_t retVal;
	uint8_t tempVal;

	retVal = readRegisterRegion(SFE_ENS160_DEVICE_STATUS, &tempVal, 1);

	if( retVal != 0 )
		return 0xFF; // Change to general error

	tempVal = (tempVal & 0x0C) >> 2; 

	switch( tempVal )
	{
		case 0: // Normal operation
			return 0;
			break;
		case 1: // Warm-up phase
			return 1;
			break;
		case 2: // Initial Start-Up Phase
			return 2;
			break;
		case 3: // Invalid Output
			return 3;
			break;
		default:
			return 0xFF;
	}
}



//////////////////////////////////////////////////////////////////////////////
// checkOperationStatus()
//
// Checks the bit that indicates if an operation mode is running i.e. the device is not off. 

bool ENS160::checkOperationStatus()
{
	int32_t retVal;
	uint8_t tempVal;

	retVal = readRegisterRegion(SFE_ENS160_DEVICE_STATUS, &tempVal, 1);

	if( retVal != 0 )
		return false; 

	tempVal &= 0x80;

	if( tempVal == 0x80 )
		return true;

	return false;
}


//////////////////////////////////////////////////////////////////////////////
// getOperationError()
//
// Checks the bit that indicates if an invalid operating mode has been selected. 

bool ENS160::getOperationError()
{
	int32_t retVal;
	uint8_t tempVal; 

	retVal = readRegisterRegion(SFE_ENS160_DEVICE_STATUS, &tempVal, 1);

	if( retVal != 0 )
		return false; 

	tempVal &= 0x40;

	if( tempVal == 0x40 )
		return true;

	return false;
}



//////////////////////////////////////////////////////////////////////////////
// getAQI()
//
// This reports the calculated Air Quality Index according to UBA which is a value between 1-5. 
// The AQI-UBA is a guideline developed by the German Federal Environmental Agency and is widely 
// referenced and adopted by many countries and organizations. 
//
// 1 - Excellent, 2 - Good, 3 - Moderate, 4 - Poor, 5 - Unhealthy. 

uint8_t ENS160::getAQI()
{
	int32_t retVal;
	uint8_t tempVal; 

	retVal = readRegisterRegion(SFE_ENS160_DATA_AQI, &tempVal, 1);

	if( retVal != 0 )
		return 0;
	
	tempVal = (tempVal & 0x03);

	return tempVal;
}

//////////////////////////////////////////////////////////////////////////////
// getTVOC()
//
// This reports the Total Volatile Organic Compounds in ppb (parts per billion)

uint16_t ENS160::getTVOC()
{
	int32_t retVal;
	uint16_t tvoc; 
	uint8_t tempVal[2] = {0}; 

	retVal = readRegisterRegion(SFE_ENS160_DATA_TVOC, tempVal, 2);

	if( retVal != 0 )
		return 0;
	
	tvoc = tempVal[0];
	tvoc |= tempVal[1] << 8;

	return tvoc;
}



//////////////////////////////////////////////////////////////////////////////
// getETOH()
//
// This reports the ehtanol concentration in ppb (parts per billion). According to 
// the datasheet this is a "virtual mirror" of the ethanol-calibrated TVOC register, 
// which is why they share the same register. 

uint16_t ENS160::getETOH()
{
	int32_t retVal;
	uint16_t ethanol; 
	uint8_t tempVal[2] = {0}; 

	retVal = readRegisterRegion(SFE_ENS160_DATA_ETOH, tempVal, 2);

	if( retVal != 0 )
		return 0;
	
	ethanol = tempVal[0];
	ethanol |= tempVal[1] << 8;

	return ethanol;
}


//////////////////////////////////////////////////////////////////////////////
// getECO2()
//
// This reports the CO2 concentration in ppm (parts per million) based on the detected VOCs and hydrogen. 

uint16_t ENS160::getECO2()
{
	int32_t retVal;
	uint16_t eco; 
	uint8_t tempVal[2] = {0}; 

	retVal = readRegisterRegion(SFE_ENS160_DATA_ECO2, tempVal, 2);

	if( retVal != 0 )
		return 0;
	
	eco = tempVal[0];
	eco |= tempVal[1]  << 8;

	return eco;
}


//////////////////////////////////////////////////////////////////////////////
// getTempKelvin()
//
// This reports the temperature compensation value given to the sensor in Kelvin.

float ENS160::getTempKelvin()
{
	int32_t retVal;
	float temperature; 
	int16_t tempConversion; 
	uint8_t tempVal[2] = {0}; 

	retVal = readRegisterRegion(SFE_ENS160_DATA_T, tempVal, 2);

	if( retVal != 0 )
		return 0;
	
	tempConversion = tempVal[0];
	tempConversion |= (tempVal[1] << 8);
	temperature = (float)tempConversion; 

	temperature = temperature/64; // Formula as described on pg. 32 of datasheet.

	return temperature;
}


//////////////////////////////////////////////////////////////////////////////
// getTempCelsius()
//
// This reports the temperature compensation value given to the sensor in Celsius.

float ENS160::getTempCelsius()
{
	float temperature; 

	temperature = getTempKelvin();

	return (temperature - 273.15);
}


//////////////////////////////////////////////////////////////////////////////
// getRH()
//
// This reports the relative humidity compensation value given to the sensor.

float ENS160::getRH()
{
	int32_t retVal;
	uint16_t rh; 
	uint8_t tempVal[2] = {0}; 

	retVal = readRegisterRegion(SFE_ENS160_DATA_RH, tempVal, 2);

	if( retVal != 0 )
		return 0;
	
	rh = tempVal[0];
	rh |= tempVal[1] << 8;

	rh = rh/512; // Formula as described on pg. 33 of datasheet.

	return rh;
}