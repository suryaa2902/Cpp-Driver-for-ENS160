#include "mbed.h" 
#include "rtos.h"
#include "uLCD_4DGL.h"
#include "PinDetect.h"
#include "ens160_i2c.h"

ENS160 myENS(p9, p10, ENS160_ADDRESS_HIGH);
uLCD_4DGL uLCD(p28,p27,p30); // serial tx, serial rx, reset pin;
PinDetect pb(p8);
Mutex mutex;

uint8_t volatile aqi;
uint32_t volatile co2,tvoc;
uint8_t volatile current_screen = 0;

void redrawBG()
{
    mutex.lock();
    uLCD.color(WHITE);
    uLCD.circle(64,64,64,WHITE);
    uLCD.filled_circle(64,64,60,BLUE);   
    mutex.unlock();
}

void getData(void const *args)
{
    while(1)
    {
        aqi = myENS.getAQI();
        co2 = myENS.getECO2();
        tvoc = myENS.getTVOC();
        Thread::wait(100);
    }
}

void allScreens()
{
    uLCD.text_width(1.75); //4X size text
    uLCD.text_height(1.75);
    uLCD.locate(6,4);
    uLCD.printf("AQI:");
    uLCD.printf("%d",aqi);
    uLCD.locate(4,7);
    uLCD.printf("C02:");
    uLCD.printf("%d", co2);
    uLCD.printf("ppm");
    uLCD.locate(4,10);
    uLCD.printf("TVOC:");
    uLCD.printf("%d",tvoc);
    uLCD.printf("ppb");
}

void AQI()
{
    uLCD.text_width(2); //4X size text
    uLCD.text_height(2);
    uLCD.locate(3,3);
    uLCD.printf("AQI\n");
    uLCD.locate(4,4);
    uLCD.printf("%d",aqi);
}

void CO2()
{
    uLCD.text_width(2); //4X size text
    uLCD.text_height(2);
    uLCD.locate(3,3);
    uLCD.printf("C02\n");
    uLCD.locate(1,4);
    uLCD.printf("%d", co2);
    uLCD.printf("ppm");
}

void TVOC()
{
    uLCD.text_width(2); //4X size text
    uLCD.text_height(2);
    uLCD.locate(3,3);
    uLCD.printf("TVOC\n");
    uLCD.locate(2,4);
    uLCD.printf("%d",tvoc);
    uLCD.printf("ppb");
}

void updateScreen()
{
    mutex.lock();
    switch (current_screen)
    {
        case 0:
            allScreens();
            break;
        case 1:
            AQI();
            break;
        case 2:
            TVOC();
            break;
        case 3:
            CO2();
            break;
    }
    mutex.unlock();
}

void pb_hit_callback()
{
    current_screen = current_screen + 1;
    if (current_screen > 3)
        current_screen = 0;
}

int main()
{
    pb.mode(PullUp);
    wait(.001);
    pb.attach_deasserted(&pb_hit_callback);
    pb.setSampleFrequency();
    Thread t1(getData);
    redrawBG();
    uint8_t oldScreen = current_screen;
    while(1)
    {
        if (oldScreen != current_screen)
        {
            oldScreen = current_screen;
            redrawBG();
        }
        updateScreen();
        Thread::wait(1000);
    }
}