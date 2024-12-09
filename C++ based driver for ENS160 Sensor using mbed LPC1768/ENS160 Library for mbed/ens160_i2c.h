#include "ens160_i2c_regs.h"

#define ENS160_ADDRESS_LOW 0x52
#define ENS160_ADDRESS_HIGH 0x53

#define ENS160_DEVICE_ID 0x0160

class ENS160 {
    public:
        uint8_t i2c_address;
        I2C i2c;
        ENS160(PinName sda, PinName scl, uint8_t i2c_device_address);

        ///////////////////////////////////////////////////////////////////////
        // init()
        // Called to init the system. Connects to the device and sets it up for 
        // operation

        bool init();

        ///////////////////////////////////////////////////////////////////////
        // isConnected()
        //  Parameter   Description
        //  ---------   -----------------------------
        //  retval      true if device is connected, false if not connected

        bool isConnected(); // Checks if sensor ack's the I2C request
                                                    
        //////////////////////////////////////////////////////////////////////////////////
        // writeRegisterRegion()
        //  Parameter    Description
        //  ---------    -----------------------------
        //  reg          register to write to
        //  data         Array to store data in
        //  length       Length of the data being written in bytes 
        //  retval       -1 = error, 0 = success

        int32_t writeRegisterRegion(char *data, uint8_t length);
        int32_t writeRegisterRegion(uint8_t reg, char data);

        //////////////////////////////////////////////////////////////////////////////////
        // readRegisterRegion()
        //  Parameter    Description
        //  ---------    -----------------------------
        //  reg          register to read from
        //  data         Array to store data in
        //  length       Length of the data to read in bytes
        //  retval       -1 = error, 0 = success
        int32_t readRegisterRegion(uint8_t reg, char *data);
        int32_t readRegisterRegion(uint8_t reg, char *data, uint8_t length);

        //////////////////////////////////////////////////////////////////////////////////
        // General Operation
        bool setOperatingMode(uint8_t);
        int8_t getOperatingMode();
        uint32_t getAppVer();
        uint16_t getUniqueID();

        //////////////////////////////////////////////////////////////////////////////////
        // Interrupts
        bool configureInterrupt(uint8_t);
        bool enableInterrupt(bool enable = true);
        bool setInterruptPolarity(bool activeHigh = true);
        int8_t getInterruptPolarity();
        bool setInterruptDrive(bool pushPull = true);
        bool setDataInterrupt(bool enable = true);
        bool setGPRInterrupt(bool);

        //////////////////////////////////////////////////////////////////////////////////
        // Temperature and Humidity compensation
        bool setTempCompensation(float);
        float getTempCompensation();
        bool setTempCompensationCelsius(float);
        float getTempCompensationCelsius();
        bool setRHCompensation(uint16_t);
        bool setRHCompensationFloat(float);
            
        //////////////////////////////////////////////////////////////////////////////////
        bool checkDataStatus();
        bool checkGPRStatus();
        uint8_t getFlags();
        bool checkOperationStatus();
        bool getOperationError();

        //////////////////////////////////////////////////////////////////////////////////
        // Data registers
        uint8_t getAQI();
        uint16_t getTVOC();
        uint16_t getETOH();
        uint16_t getECO2();
        float getTempKelvin();
        float getTempCelsius();
        float getRH();
};
