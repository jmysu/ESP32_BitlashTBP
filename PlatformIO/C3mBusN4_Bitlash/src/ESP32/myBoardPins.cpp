/**
 * @file myBoardoardPins.cpp
 * @author jimmy.su
 * @brief   board pin define by platformio.ini build-flags
 * @version 0.1
 * @date 2023-06-03
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include <Arduino.h>
#include "_BitlashScript.h"

Ticker tickBlinkLED1;
Ticker tickBlinkLED20;
Ticker tickBlinkLED21;

Ticker tickOffLED1;
Ticker tickOffLED20;
Ticker tickOffLED21;

int8_t _pinNeopixel=-1;
int8_t _pinBuzzer  =-1; 
String _BoardName  ="";

void setupBoardPins()
{
    _BoardName  = String(BOARDNAME);

    #if (PIN_C3mBusN4)
     _pinBuzzer  = 2; 
     pinMode(_pinBuzzer, OUTPUT);
     log_i("Buzzer@%02d", _pinBuzzer);
     _pinNeopixel= 3;
     pinMode(_pinNeopixel, OUTPUT);
     log_i("Neopixel@%02d", _pinNeopixel);
     neopixelRGB(15,0,15); //pink
     neopixelSetupBlink(1000); //1 sencond

     pinMode( 1, OUTPUT);
     pinMode(20, OUTPUT);
     pinMode(21, OUTPUT);
     
     digitalWrite( 1, 1); //Off     IO1 Red
     digitalWrite(20, 0); //On      Rx Red
     digitalWrite(21, 1); //Off     Tx Green
     
    #endif
}

 
void toggleLED1() 
{
    digitalWrite(1, !digitalRead(1));
    //log_w("LED1:%d", digitalRead(1));
}
void offLED1() {digitalWrite(1,1);}
void blinkLED1(uint8_t msOnTime)
{
    //Sync neopixel color
    //uint8_t cUMS= (_EspNowLeaderUms/1000)%6;
    //neopixelRotateColors(cUMS);
    //log_w("LED1: %d", cUMS);
    _cNeopixelColors = 0; //Reset neopixel to RED
    neopixelBlink();
    //Blink every 3 seconds
    //if (cUMS==0){
        pinMode(1, OUTPUT);
        digitalWrite(1, 0); //Turn On IO1
        tickOffLED1.once_ms(msOnTime, offLED1); //Wait 250ms and off
    //    }
    //
}

void offLED20() {digitalWrite(20,1);}
void blinkLED20(uint8_t msOnTime)
{
    pinMode(20, OUTPUT);
    digitalWrite(20, 0); //TurnOn IO20
    tickOffLED20.once_ms(msOnTime, offLED20);
}

void offLED21() {digitalWrite(21,1);}
void blinkLED21(uint8_t msOnTime)
{
    //Reset/Adjust sync time
    //struct timeval tv;
    //gettimeofday(&tv, NULL); //retrieve original tv
    //tv.tv_usec= 0;           //reset usec
    //settimeofday(&tv, NULL); //set 
    //----------------------------------
    //Log synced time in HH:MM:SS.ZZZ
    //log_w("%02d:%02d:%02d.%03lu",hour(), minute(), second(), tv.tv_usec/1000L);
    _cNeopixelColors = 0; //Reset neopixel to RED
    neopixelBlink();
    //Sync neopixel color
    //uint8_t cUMS= (_EspNowLeaderUms/1000)%6;
    //neopixelRotateColors(cUMS);
    //log_w("LED21: %d", cUMS);

    //if (cUMS==0) {
        pinMode(21, OUTPUT);
        digitalWrite(21, 0); //Turn On IO21
        tickOffLED21.once_ms(msOnTime, offLED21); //Wait 100ms and off
    //    }


}