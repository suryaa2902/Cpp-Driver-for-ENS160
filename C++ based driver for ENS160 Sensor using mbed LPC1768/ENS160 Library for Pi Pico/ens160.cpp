#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "ens160_i2c.h"

int main()
{
    stdio_init_all();
    // This example will use I2C0 on the default SDA and SCL pins (4, 5 on a Pico)
    i2c_init(i2c_default, 400 * 1000);
    gpio_set_function(PICO_DEFAULT_I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(PICO_DEFAULT_I2C_SDA_PIN);
    gpio_pull_up(PICO_DEFAULT_I2C_SCL_PIN);
    // Make the I2C pins available to picotool
    bi_decl(bi_2pins_with_func(PICO_DEFAULT_I2C_SDA_PIN, PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C));
    printf("Initialisation complete!\n");
    ENS160 myENS(i2c_default, ENS160_ADDRESS_HIGH);

    bool printedCompensation = false; 
    int ensStatus; 

    if (!myENS.init())
    {
        printf("Hello, ENS160! Reading raw data from registers...\n");
        while(1);
    }
    if( myENS.setOperatingMode(SFE_ENS160_RESET) )
        printf("Ready.\n");
    sleep_ms(100);
    myENS.setOperatingMode(SFE_ENS160_IDLE);
    sleep_ms(500);
    myENS.setOperatingMode(SFE_ENS160_STANDARD);
    ensStatus = myENS.getFlags();
    printf("Gas Sensor Status Flag: ");
    printf("%d\n", ensStatus);
    while (1)
    {
        if( myENS.checkDataStatus() )
        {
            if( printedCompensation == false)
            {
                printf("---------------------------\n");
                printf("Compensation Temperature: ");
                printf("%f\n", myENS.getTempCelsius());
                printf("---------------------------");
                printf("Compensation Relative Humidity: ");
                printf("%f\n", myENS.getRH());
                printf("---------------------------\n");
                printedCompensation = true;
                sleep_ms(500);
            }

            printf("Air Quality Index (1-5) : ");
            printf("%d\n", myENS.getAQI());

            printf("Total Volatile Organic Compounds: ");
            printf("%d", myENS.getTVOC());
            printf("ppb\n");

            printf("CO2 concentration: ");
            printf("%d", myENS.getECO2());
            printf("ppm\n");

        }
        sleep_ms(100);
    }
    return 0;
}