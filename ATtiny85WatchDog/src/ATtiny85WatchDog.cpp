/*
** ATtiny85 Watch Dog for an ESP8266/ESP32
** 
** Copyright 2022, 2023 Willem Aandewiel
** Version 3.1  23-03-2023
** 
** 
** PACKAGES: 
**   - framework-arduino-avr-attiny @ 1.5.2 
**   - tool-avrdude @ 1.60300.200527 (6.3.0) 
**   - toolchain-atmelavr @ 1.70300.191015 (7.3.0)
** 
** Use avrfuses.app to set fuses:
**      Extended  : 0xFF
**      High      : 0xDE (but 0xDF might also "work" --> brown-out detection disabled)
**      Low       : 0xE2
**
**    [Program] -> [Verify] -> [Read]
**
** -->> AVRfuses: Serial programdownloading (SPI) enabled
** -->> AVRfuses: Brown-out Detector trigger level: VCC=1.8 V
** -->> AVRfuses: Select Clock source: Int. RC Osc. 8MHz; 
**                  Start-up time PWRDWN/RESET: 6 CK/1 4CK +64 ms
** 
** Board              : "ATtiny25/45/85 (no bootloader)"
** Chip               : "ATtiny85"
** Clock Source       : "8MHz (internal)"
** Timer 1 Clock      : "CPU (CPU Frequency)"
** LTO(1.6.11+ only)  : "disabled"
** millis()/micros()  : "Enabled"                                      
** save EEPROM        : "EEPROM not retained"
** B.O.D. Level (Only set on bootload): "B.O.D. Enabled (1.8v)"
** 
**  Copyright (c) 2022, 2023 Willem Aandewiel
**
**  TERMS OF USE: MIT License. See bottom of file.
**
** ATMEL ATTINY85
**                        +--\/--+
**             RESET PB5 1|      |8 VCC
**    <--[ESP32_EN]  PB3 2|      |7 PB2  (INT0) <----- heartbeat
**       [RESET_ME]  PB4 3|      |6 PB1  [x] 
**                   GND 4|      |5 PB0  [NEO_LED] -> 
**                        +------+
**
**  Boot sequence:
**  ==============
**  State        | NEOPIXEL                | Remark
**  -------------+-------------------------+--------------------------------------
**  Power On     | BLUE                    | Fade On ~6 sec, fade Off 6 sec 
**               |                         | (duration: _STARTUP_TIME seconds) 
**               |                         | Next: "Normal Operation"
**  -------------+-------------------------+--------------------------------------
**  Normal       | GREEN HeartBeat         | Fade On ~5 sec, fade Off ~5 sec
**  Operation    |      while heartbeats   | 
**               |      received           | Next: "Normal Operation"
**               |                         |  
**               | if no Heartbeats        |  
**               |   for _FIRST_WARNING    |
**               |   seconds               | Next: "First Alarm"
**  -------------+-------------------------+--------------------------------------
**  First Alarm  | RED Heartbeat           | Fade On ~ 1.5sec, fade Off ~ 1.5 sec
**               |                         |  
**               | if Heartbeat received   | Next: "Normal Operation"                        |  
**               |                         |  
**               | if no Heartbeats        |  
**               |   for _LAST_WARNING     |
**               |   seconds               | Next: "Second Alarm"
**  -------------+-------------------------+--------------------------------------
**  Second Alarm | RED Heartbeat           | Fade On ~1 sec, fade Off ~1 sec
**               |                         |  
**               | if Heartbeat received   | Next: "Normal Operation"                        |  
**               |                         |  
**               | if no Heartbeats        |  
**               |   for _MAX_NO_HARTBEAT  |
**               |   seconds               | Next: "Last Change"
**  -------------+-------------------------+--------------------------------------
**  Last Change  | RED blink               | Blink On ~ 100mS, Off ~ 100mS
**               |                         | (duration _LAST_CHANGE seconds)
**               |                         |  
**               | if Heartbeat received   | Next: "Normal Operation"                        |  
**               |                         | 
**               |                         | Next: "Reset Master" state
**  -------------+-------------------------+--------------------------------------
**  Reset Master | WHITE ON                | loop indefinitely...
**               |                         | or Wait for RESET from Master
**  -------------+-------------------------+--------------------------------------
**  
*/

// https://github.com/GreyGnome/EnableInterrupt
#include <EnableInterrupt.h>

#define _PIN_NEOPIXELS        0       // GPIO-00 ==> DIL-6 ==> PB0
#define _PIN_DUMMY1           1       // GPIO-01 ==> DIL-5 ==> PB1
#define _PIN_HEARTBEAT        2       // GPIO-02 ==> DIL-7 ==> PB2  / INT0
#define _PIN_MASTER_EN        3       // GPIO-03 ==> DIL-2 ==> PB3
#define _PIN_RESET_MYSELF     4       // GPIO-04 ==> DIL-3 ==> PB4

#include <neoPixStuff.h>

#define _STARTUP_TIME     30000 //-- 30 seconden
#define _MAX_NO_HARTBEAT  90000 //-- 90 seconds
#define _LAST_WARNING     75000 //-- 15 seconds before reset
#define _FIRST_WARNING    60000 //-- 30 seconds before reset
#define _LAST_CHANGE       5000

volatile  bool receivedInterrupt = false;
uint8_t   feedsReceived = 0;
uint32_t  loopTimer, lastHartbeatTimer = 0;
uint32_t  waitTimer;

//----------------------------------------------------------------
void interruptSR(void) 
{
  receivedInterrupt = true;
    
}   // interruptSR()

//----------------------------------------------------------------
bool handleInterrupt()
{
  if (receivedInterrupt)
  {
    receivedInterrupt = false;
    lastHartbeatTimer = millis();
    neoPixOn(0, neoPixWhite);
    delay(20);
    return true;
  }
  return false;
  
} //  handleInterrupt()


//----------------------------------------------------------------
void setup() 
{
    //-- INITIALIZE NeoPixel neoPixel object (REQUIRED)
    //neoPixel.begin();    
    //-- with tinyNeoPixel_Static, you need to set pinMode yourself. This means 
    //-- you can eliminate pinMode() and replace with direct port writes to 
    //-- save a couple hundred bytes in sketch size (note that this
    //-- savings is only present when you eliminate *all* references to pinMode).
    //-- leds.begin() not needed on tinyNeoPixel_Static     
    pinMode(_PIN_NEOPIXELS, OUTPUT);
    //--Turn OFF all pixels ASAP  
    //neoPixel.clear();
    //neoPixel.show();            
    //neoPixel.setBrightness(_NEO_BRIGHTNESS);
    neoPixel.begin();
    neoPixel.setBrightness(_NEO_BRIGHTNESS);
    neoPixel.show(); // Initialize all pixels to 'off'

    pinMode(_PIN_HEARTBEAT, INPUT); 
    
    digitalWrite(_PIN_RESET_MYSELF, HIGH);
    pinMode(_PIN_RESET_MYSELF,  OUTPUT); 
    digitalWrite(_PIN_RESET_MYSELF, HIGH);

    enableInterrupt(_PIN_HEARTBEAT, interruptSR, CHANGE);

    waitTimer = millis();
    while((millis() - waitTimer) < _STARTUP_TIME)
    {
      neoPixHeartBeat(0, 12000, neoPixBlue, neoPixFade);
    }
    neoPixOff(0);

    receivedInterrupt = false;
    feedsReceived     = 0;
    lastHartbeatTimer = millis();

} // setup()


//----------------------------------------------------------------
void loop() 
{
  
  handleInterrupt();
  loopTimer = (millis() - lastHartbeatTimer);

  if (loopTimer > 50)  neoPixOff(1);

  //---- watch out! Watchdog kicking in soon!
  if ((loopTimer > _LAST_WARNING) && (loopTimer < _MAX_NO_HARTBEAT))
  {
    neoPixHeartBeat(0, 500, neoPixRed); 
  }
  
  //---- Alarm! Watchdog no feed!
  else if ((loopTimer > _FIRST_WARNING) && (loopTimer < _MAX_NO_HARTBEAT))
  {
    neoPixHeartBeat(0, 2000, neoPixRed);
  }

  //---- OK! Watchdog had feed, normal operation!
  else if (loopTimer < _MAX_NO_HARTBEAT)
  {
    neoPixHeartBeat(0, 10000, neoPixGreen);
    if (handleInterrupt()) return;
  }
  //-- no heartbeats for too long!
  //-- initiate reset sequence ..
  else if (loopTimer > _MAX_NO_HARTBEAT)
  {
    waitTimer = millis();
    while((millis() - waitTimer) < _LAST_CHANGE)
    {
      neoPixHeartBeat(0, 200, neoPixRed, neoPixBlink);
      if (handleInterrupt()) return;
    }
    disableInterrupt(_PIN_HEARTBEAT);
    neoPixOn(0, neoPixWhite);
    receivedInterrupt = false;
    digitalWrite(_PIN_MASTER_EN,  HIGH);
    //-- reset ESP32 ------------------
    pinMode(_PIN_MASTER_EN, OUTPUT);
    digitalWrite(_PIN_MASTER_EN, HIGH);
    delay(500);
    digitalWrite(_PIN_MASTER_EN,  LOW);
    delay(2000);  
    digitalWrite(_PIN_MASTER_EN, HIGH);

    pinMode(_PIN_MASTER_EN, INPUT);
    //-- now give ESP time (60 seconds) to startup!
    waitTimer = millis();
    while((millis() - waitTimer) < (_STARTUP_TIME / 10))
    {
      neoPixOn(0, neoPixRed);
      delay(100);
      neoPixOff(0);
      delay(100);
    }
    neoPixOn(0, neoPixWhite);
    //-- reset myself in case the Master does not do 
    //-- this (Connect DIL-1 (Reset) to DIL-3 (PB4)!
    pinMode(_PIN_RESET_MYSELF, OUTPUT);
    digitalWrite(_PIN_RESET_MYSELF, LOW);
    delay(100);
    digitalWrite(_PIN_RESET_MYSELF, HIGH);
    delay(100);
    digitalWrite(_PIN_RESET_MYSELF, LOW);
    delay(100);
    digitalWrite(_PIN_RESET_MYSELF, HIGH);
    setup();

  } // > _MAX_NO_HARTBEAT
  
} // loop()



/***************************************************************************
**
** Permission is hereby granted, free of charge, to any person obtaining a
** copy of this software and associated documentation files (the
** "Software"), to deal in the Software without restriction, including
** without limitation the rights to use, copy, modify, merge, publish,
** distribute, sublicense, and/or sell copies of the Software, and to permit
** persons to whom the Software is furnished to do so, subject to the
** following conditions:
**
** The above copyright notice and this permission notice shall be included
** in all copies or substantial portions of the Software.
**
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
** OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
** MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
** IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
** CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT
** OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR
** THE USE OR OTHER DEALINGS IN THE SOFTWARE.
**
****************************************************************************
*/
