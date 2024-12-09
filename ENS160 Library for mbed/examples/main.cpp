#include "mbed.h"
#include "ens160_i2c.h"
 
ENS160 myENS(p9, p10, ENS160_ADDRESS_HIGH);
Serial pc(USBTX, USBRX);
 
bool printedCompensation = false; 
int ensStatus;
 
int main()
{
    if (!myENS.init())
    {
        pc.printf("Hello, ENS160! Reading raw data from registers...\n");
        while(1);
    }
    if( myENS.setOperatingMode(SFE_ENS160_RESET) )
        pc.printf("Ready.\n");
    wait(0.1);
    myENS.setOperatingMode(SFE_ENS160_IDLE);
    wait(0.5);
    myENS.setOperatingMode(SFE_ENS160_STANDARD);
    ensStatus = myENS.getFlags();
    pc.printf("Gas Sensor Status Flag: ");
    pc.printf("%d\n", ensStatus);
    while (1)
    {
        if( myENS.checkDataStatus() )
        {
            if( printedCompensation == false)
            {
                pc.printf("---------------------------\n");
                pc.printf("Compensation Temperature: ");
                pc.printf("%f\n", myENS.getTempCelsius());
                pc.printf("---------------------------");
                pc.printf("Compensation Relative Humidity: ");
                pc.printf("%f\n", myENS.getRH());
                pc.printf("---------------------------\n");
                printedCompensation = true;
                wait(0.5);
            }
 
            pc.printf("Air Quality Index (1-5) : ");
            pc.printf("%d\n", myENS.getAQI());
 
            pc.printf("Total Volatile Organic Compounds: ");
            pc.printf("%d", myENS.getTVOC());
            pc.printf("ppb\n");
 
            pc.printf("CO2 concentration: ");
            pc.printf("%d", myENS.getECO2());
            pc.printf("ppm\n");
 
        }
        wait(0.1);
        }
}