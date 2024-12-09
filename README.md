# Developing a C++ based driver for ENS160 Sensor compatible with mbed LPC1768

**Team Members: Poorvi Bhaskar, Shreyaa Senthilnathan, Surya Senthilkumar Shanthi, Venkata Siva Krishna Madala**

**Georgia Institute of Technology**

Watch our video presentation and demo:

Presentation (5min): https://github.com/krishnamvs/ECE-4180-Final-Project-Fall-22/blob/main/ECE4180FinalPresentation.pptx

Demonstration (5min): https://www.youtube.com/watch?v=4Gfi7DoumOI


## Table of Content
* [Project Idea](#project-idea)
* [Parts List](#parts-list)
* [Schematic and Connection Guide](#schematic-and-connection-guide)
* [Source Code](#source-code)
* [Future Work](#future-work)

## Project Idea

In our lab experiments we used a lot of Libraries from the mbed community. We wanted contribute back to them and hone our C++ skills. 

We narrowed down on the SparkFun Qwiic ENS160 Air Quality Sensor as it was very recently launched and would be significantly challenging to write a C++ based I2C driver for mbed board.

We also planned on extending our driver to Raspberry Pi 4 Model B and Raspberry Pi Pico. We worked on the Pi Pico driver but faced multiple issues trying to debug it.

## Parts List

Electronics:
* Mbed LPC1768, https://www.sparkfun.com/products/9564
* LCD Display: uLCD-144G2, https://www.sparkfun.com/products/11377
* Pushbutton: https://www.sparkfun.com/products/97
* ENS160 Sensor: https://www.sparkfun.com/products/20844 
* Jumper Wires(M/M): https://www.sparkfun.com/products/124

## Schematic and Connection Guide

#### Device Setup

Here is a few picture of our setup we used for the presentation demo.

<img width="384" alt="Setup" src="https://user-images.githubusercontent.com/29601249/206524220-d715f1f4-a34a-4a29-80b6-bd622c163a46.png">

#### Schematic

![Schematic](https://user-images.githubusercontent.com/29601249/206524154-da763efb-baf2-48f2-a6d6-9be25cbd800f.png)


#### Connections
| mbed        |  Push Button |   ENS160  |    uLCD   | 
|-------------|--------------|-----------|-----------|
|             |              |           |           |
| 5V = Vdd    |              | VCC       | VCC       |
| Gnd         |  -           | GND       | GND       |
| p8          |  +           |           |           |
| p9          |              | SDA       |           |
| p10         |              | SCL       |           |
| p27         |              |           | RX        | 
| p28         |              |           | TX        |
| p30         |              |           | RES       |

## Source Code
All the source code is published in the mbed repository.

Link to Final Project Code -> https://os.mbed.com/users/krishnamvs/code/ECE4180FinalProjectFall22/

Link to ENS160 Library -> https://os.mbed.com/users/krishnamvs/code/ENS160_Library/

## Future Work

We developed a C++ based driver for ENS160 for the Raspberry Pi Pico. But due to issues with printf's on TinyUSB in the Pi Pico C++ SDK 1.4.0 we were unable to fully test it. As the Pi Pico C++ SDK matures, we hope that we can verify the driver we have developed.

