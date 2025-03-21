#include "ens160_i2c.h"

//Initializes the I2C object with given SDA/SCL pins and sets the device address.
ENS160::ENS160(PinName sda, PinName scl, uint8_t i2c_device_address)
:i2c(sda, scl)
{
    this->i2c_address = i2c_device_address;
}

//Writes the register address, reads one byte, stores it in data, and returns status.
int32_t ENS160::readRegisterRegion(uint8_t reg, char *data)
{
    uint8_t retVal;
    char temp[1] = {reg};
    char tempVal[1] = {0xFF};
    this->i2c.write(this->i2c_address, temp, 1);
    wait(0.01);
    retVal = this->i2c.read(this->i2c_address, tempVal, 1);
    wait(0.01);
    data[0] = tempVal[0];
    if (retVal != 0)
    	return -1;
    return 0;
}

//Writes the register, reads a specified number of bytes into a temporary buffer, copies to data, and returns status.
int32_t ENS160::readRegisterRegion(uint8_t reg, char *data, uint8_t length)
{
    int i;
    int32_t retVal;
    char temp_dest[length];
    for (i=0; i < length; i++)
    {
        temp_dest[i] = 0xFF;
    }
    char temp[1] = {reg};
    this->i2c.write(this->i2c_address, temp, 1);
    wait(0.01);
    retVal = this->i2c.read(this->i2c_address, temp_dest, length);
    wait(0.01);
    for (i=0; i < length; i++)
    {
        data[i] = temp_dest[i];
    }
    if (retVal !=0)
    	return -1;
    return 0;
}

//Writes an array of bytes to the sensor and returns status.
int32_t ENS160::writeRegisterRegion(char *data, uint8_t length)
{
    int32_t retVal;
    retVal = this->i2c.write(this->i2c_address, data, length);
    wait(0.01);
    if(retVal !=0)
    	return -1;
	return 0;
}

//Writes a register address followed by one data byte and returns status.
int32_t ENS160::writeRegisterRegion(uint8_t reg, char data)
{
    int32_t retVal;
    char temp_data[2] = {reg, data};
    retVal = this->i2c.write(this->i2c_address, temp_data, 2);
    wait(0.01);
    if (retVal != 0)
    	return -1;
	return 0;
}

//////////////////////////////////////////////////////////////////////////////
//getUniqueID()
//Gets the device's unique ID

//Reads two bytes from the PART_ID register and combines them into a 16‑bit unique ID.
uint16_t ENS160::getUniqueID()
{
    int32_t retVal;
	char tempVal[2] = {0}; 
	uint16_t id; 

    retVal = this->readRegisterRegion(SFE_ENS160_PART_ID, tempVal, 2);

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

//Compares the read unique ID with the expected device ID to verify connection.
bool ENS160::isConnected()
{
	uint16_t uniqueID; 
	uniqueID = getUniqueID(); 
	if( uniqueID != ENS160_DEVICE_ID )
		return false;
	return true;
}

//Initializes the sensor by checking its connection.
bool ENS160::init()
{
    return this->isConnected();
}

//////////////////////////////////////////////////////////////////////////////
// setOperatingMode()
// Sets the operating mode: Deep Sleep (0x00), Idle (0x01), Standard (0x02), Reset (0xF0)
//
//  Parameter    Description
//  ---------    -----------------------------
//  val					 The desired operating mode to set. 

//Validates and sets the sensor’s operating mode by writing to its register.
bool ENS160::setOperatingMode(uint8_t val)
{
	int32_t retVal;

	if( val > SFE_ENS160_RESET )
		return false;

	retVal = this->writeRegisterRegion(SFE_ENS160_OP_MODE, val);

	if( retVal != 0 )
		return false;

	return true; 
}

//////////////////////////////////////////////////////////////////////////////
// getOperatingMode()
//
// Gets the current operating mode: Deep Sleep (0x00), Idle (0x01), Standard (0x02), Reset (0xF0)

//Reads and returns the current operating mode or an error code.
int8_t ENS160::getOperatingMode()
{
	int32_t retVal;
	char tempVal[1] = {0x00}; 

	retVal = this->readRegisterRegion(SFE_ENS160_OP_MODE, tempVal);

	if( retVal != 0 )
		return -1;

	return tempVal[0]; 
}

//////////////////////////////////////////////////////////////////////////////
// configureInterrupt()
//
// Changes all of the settings within the interrupt configuration register.
//
//  Parameter    Description
//  ---------    -----------------------------
//  val					 The desired configuration settings.

//Writes the given value to the interrupt configuration register.
bool ENS160::configureInterrupt(uint8_t val)
{
	int32_t retVal;

	retVal = this->writeRegisterRegion(SFE_ENS160_CONFIG, val);

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

//Reads the config register, sets the interrupt enable bit, and writes it back.
bool ENS160::enableInterrupt(bool enable)
{
	int32_t retVal;
	char tempVal[1] = {0x00}; 

	retVal = this->readRegisterRegion(SFE_ENS160_CONFIG, tempVal);

	if( retVal != 0 )
		return false;
	
	tempVal[0] = (tempVal[0] | (uint8_t)enable); 

	retVal = this->writeRegisterRegion(SFE_ENS160_CONFIG, tempVal[0]);

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

//Sets the interrupt polarity by shifting the activeHigh value into bit 6.
bool ENS160::setInterruptPolarity(bool activeHigh)
{
	int32_t retVal;
	char tempVal[1] = {0x00}; 

	retVal = this->readRegisterRegion(SFE_ENS160_CONFIG, tempVal);
	
	if( retVal != 0 )
		return false;

	tempVal[0] = (tempVal[0] | (activeHigh << 6)); 

	retVal = this->writeRegisterRegion(SFE_ENS160_CONFIG, tempVal[0]);

	if( retVal != 0 )
		return false;

	return true;
}

//////////////////////////////////////////////////////////////////////////////
// getInterruptPolarity()
//
// Retrieves the polarity of the physical interrupt. 

//Reads the config register, masks bit 6, and returns the interrupt polarity.
int8_t ENS160::getInterruptPolarity()
{
	int32_t retVal;
	char tempVal[1] = {0x00}; 

	retVal = this->readRegisterRegion(SFE_ENS160_CONFIG, tempVal);
	
	if( retVal != 0 )
		return -1;

	tempVal[0] &= 0x40;

	return (tempVal[0] >> 6);
}

//////////////////////////////////////////////////////////////////////////////
// setInterruptDrive()
//
// Changes the pin drive of the interrupt: open drain (default) to push/pull
//
//  Parameter    Description
//  ---------    -----------------------------
//  pushPull     Changes the drive of the pin. 

//Sets the interrupt drive type by shifting pushPull into bit 5.
bool ENS160::setInterruptDrive(bool pushPull)
{
	int32_t retVal;
	char tempVal[1] = {0x00}; 

	retVal = this->readRegisterRegion(SFE_ENS160_CONFIG, tempVal);

	if( retVal != 0 )
		return false;
	
	tempVal[0] = (tempVal[0] | (uint8_t)(pushPull << 5)); 

	retVal = this->writeRegisterRegion(SFE_ENS160_CONFIG, tempVal[0]);

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

//Routes the data ready signal to the interrupt by setting the appropriate bit.
bool ENS160::setDataInterrupt(bool enable)
{
	int32_t retVal;
	char tempVal[1] = {0x00}; 

	retVal = this->readRegisterRegion(SFE_ENS160_CONFIG, tempVal);

	if( retVal != 0 )
		return false;
	
	tempVal[0] = (tempVal[0] | (uint8_t)(enable << 1)); 

	retVal = this->writeRegisterRegion(SFE_ENS160_CONFIG, tempVal[0]);

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

//Sets the general-purpose register interrupt by shifting enable into bit 3.
bool ENS160::setGPRInterrupt(bool enable)
{
	int32_t retVal;
	char tempVal[1] = {0x00}; 

	retVal = this->readRegisterRegion(SFE_ENS160_CONFIG, tempVal);

	if( retVal != 0 )
		return false;
	
	tempVal[0] = (tempVal[0] | (uint8_t)(enable << 3)); 
	
	retVal = this->writeRegisterRegion(SFE_ENS160_CONFIG, tempVal[0]);

	if( retVal != 0 )
		return false;

	return true; 
}


//////////////////////////////////////////////////////////////////////////////
// getAppVer()
//
// Retrieves the 24 bit application version of the device.

//Reads 3 bytes from the GPR registers to construct a 24‑bit firmware version.
uint32_t ENS160::getAppVer()
{
	int32_t retVal;
	char tempVal[3] = {0};
	uint32_t version;

	retVal = this->readRegisterRegion(SFE_ENS160_GPR_READ4, tempVal, 3);

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

//Converts a Kelvin temperature to sensor format and writes it to the temperature input register.
bool ENS160::setTempCompensation(float tempKelvin)
{
	int32_t retVal;
	char tempVal[3] = {0};
	uint16_t kelvinConversion = tempKelvin; 

	kelvinConversion = kelvinConversion * 64; // convert value - fixed equation pg. 29 of datasheet
    tempVal[0] = SFE_ENS160_TEMP_IN;
	tempVal[1] = (kelvinConversion & 0x00FF);
	tempVal[2] = (kelvinConversion & 0xFF00) >> 8;

	retVal = this->writeRegisterRegion(tempVal, 3);

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

//Converts Celsius to Kelvin then sets temperature compensation.
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

//Converts relative humidity to sensor format and writes it to the humidity input register.
bool ENS160::setRHCompensation(uint16_t humidity)
{
	int32_t retVal;
	char tempVal[3] = {0};

	humidity = humidity * 512; // convert value - fixed equation pg. 29 in datasheet. 
    tempVal[0] = SFE_ENS160_RH_IN;
	tempVal[1] = (humidity & 0x00FF);
	tempVal[2] = (humidity & 0xFF00) >> 8;

	retVal = this->writeRegisterRegion(tempVal, 3);

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

//Converts float humidity to integer and calls setRHCompensation.
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

//Checks the NEWDAT bit in the device status register to determine if new data is ready.
bool ENS160::checkDataStatus()
{
	int32_t retVal;
	char tempVal[1] = {0x00}; 

	retVal = this->readRegisterRegion(SFE_ENS160_DEVICE_STATUS, tempVal);

	if( retVal != 0 )
		return false; 

	tempVal[0] &= 0x02; 

	if( tempVal[0] == 0x02 )
		return true;

	return false;
}


//////////////////////////////////////////////////////////////////////////////
// checkGPRStatus()
//
// This checks the if the NEWGPR bit is high indicating that there is data in the
// general purpose read registers. The bit is cleared the relevant registers have been
// read. 

//Checks the NEWGPR bit to see if general-purpose register data is available.
bool ENS160::checkGPRStatus()
{
	int32_t retVal;
	char tempVal[1] = {0x00}; 

	retVal = this->readRegisterRegion(SFE_ENS160_DEVICE_STATUS, tempVal);

	if( retVal != 0 )
		return false; 

	tempVal[0] &= 0x01;

	if( tempVal[0] == 0x01 )
		return true;

	return false;
}


//////////////////////////////////////////////////////////////////////////////
// getFlags()
//
// This checks the status "flags" of the device (0-3).

//Extracts and returns device status flags from the status register.
uint8_t ENS160::getFlags()
{
	int32_t retVal;
	char tempVal[1] = {0x00};

	retVal = this->readRegisterRegion(SFE_ENS160_DEVICE_STATUS, tempVal);

	if( retVal != 0 )
		return 0xFF; // Change to general error

	tempVal[0] = (tempVal[0] & 0x0C) >> 2; 

	switch( tempVal[0] )
	{
		case 0: // Normal operation
			return 0;
		case 1: // Warm-up phase
			return 1;
		case 2: // Initial Start-Up Phase
			return 2;
		case 3: // Invalid Output
			return 3;
		default:
			return 0xFF;
	}
}



//////////////////////////////////////////////////////////////////////////////
// checkOperationStatus()
//
// Checks the bit that indicates if an operation mode is running i.e. the device is not off. 

//Checks bit 7 of the status register to verify if an operation mode is active.
bool ENS160::checkOperationStatus()
{
	int32_t retVal;
	char tempVal[1] = {0x00};

	retVal = this->readRegisterRegion(SFE_ENS160_DEVICE_STATUS, tempVal);

	if( retVal != 0 )
		return false; 

	tempVal[0] &= 0x80;

	if( tempVal[0] == 0x80 )
		return true;

	return false;
}


//////////////////////////////////////////////////////////////////////////////
// getOperationError()
//
// Checks the bit that indicates if an invalid operating mode has been selected. 

//Checks bit 6 of the status register to determine if an operation error is present.
bool ENS160::getOperationError()
{
	int32_t retVal;
	char tempVal[1] = {0x00}; 

	retVal = this->readRegisterRegion(SFE_ENS160_DEVICE_STATUS, tempVal);

	if( retVal != 0 )
		return false; 

	tempVal[0] &= 0x40;

	if( tempVal[0] == 0x40 )
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

//Reads and masks the AQI register to return the AQI value.
uint8_t ENS160::getAQI()
{
	int32_t retVal;
	char tempVal[1] = {0x00}; 

	retVal = this->readRegisterRegion(SFE_ENS160_DATA_AQI, tempVal);

	if( retVal != 0 )
		return 0;
	
	tempVal[0] = (tempVal[0] & 0x03);

	return tempVal[0];
}

//////////////////////////////////////////////////////////////////////////////
// getTVOC()
//
// This reports the Total Volatile Organic Compounds in ppb (parts per billion)

//Reads two bytes of TVOC data and combines them into a 16‑bit value.
uint16_t ENS160::getTVOC()
{
	int32_t retVal;
	uint16_t tvoc; 
	char tempVal[2] = {0}; 

	retVal = this->readRegisterRegion(SFE_ENS160_DATA_TVOC, tempVal, 2);

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

//Reads ethanol data (mirroring TVOC) and returns it as a 16‑bit value.
uint16_t ENS160::getETOH()
{
	int32_t retVal;
	uint16_t ethanol; 
	char tempVal[2] = {0}; 

	retVal = this->readRegisterRegion(SFE_ENS160_DATA_ETOH, tempVal, 2);

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

//Reads CO2 data from the sensor and returns it as a 16‑bit value.
uint16_t ENS160::getECO2()
{
	int32_t retVal;
	uint16_t eco; 
	char tempVal[2] = {0}; 

	retVal = this->readRegisterRegion(SFE_ENS160_DATA_ECO2, tempVal, 2);

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

//Reads raw temperature data, converts it using the sensor’s formula, and returns Kelvin.
float ENS160::getTempKelvin()
{
	int32_t retVal;
	float temperature; 
	int16_t tempConversion; 
	char tempVal[2] = {0}; 

	retVal = this->readRegisterRegion(SFE_ENS160_DATA_T, tempVal, 2);

	if( retVal != 0 )
		return -1.0;
	
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

//Converts the Kelvin temperature to Celsius.
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

//Reads two bytes of relative humidity data, applies conversion, and returns the RH value.
float ENS160::getRH()
{
	int32_t retVal;
	uint16_t rh; 
	char tempVal[2] = {0}; 

	retVal = this->readRegisterRegion(SFE_ENS160_DATA_RH, tempVal, 2);

	if( retVal != 0 )
		return -1.0;
	
	rh = tempVal[0];
	rh |= tempVal[1] << 8;

	rh = rh/512; // Formula as described on pg. 33 of datasheet.

	return rh;
}
