/*
** part of ATtiny85 Watch Dog 
*/ 
#ifndef NEOPIXSTUFF_H
#define NEOPICSTUFF_H

#include <Arduino.h>

//-- How many NeoPixels are attached to the Arduino?
#define _NEO_PIXELS_COUNT      2

//-- NeoPixel brightness, 0 (min) to 255 (max)
#define _NEO_BRIGHTNESS       50 

//-- Since this is for the static version of the library, we need to supply the 
//-- pixel array. This saves space by eliminating use of malloc() and free(), 
//-- and makes the RAM used for the frame buffer show up when the sketch is 
//-- compiled.
extern byte pixels[_NEO_PIXELS_COUNT * 3];

//-- When we setup the NeoPixel library, we tell it how many pixels, and which 
//-- pin to use to send signals.
#define _PIN_NEOPIXELS        0       // GPIO-00 ==> DIL-6 ==> PB0

//-- Note that for older NeoPixel strips you might need to change the third 
//-- parameter

//-- Declare our NeoPixel strip object:
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif


// IMPORTANT: To reduce NeoPixel burnout risk, add 1000 uF capacitor across
// pixel power leads, add 300 - 500 Ohm resistor on first pixel's data input
// and minimize distance between Arduino and first pixel.  Avoid connecting
// on a live circuit...if you must, connect GND first.

enum neoPixColor 
{
    neoPixWhite
  , neoPixRed
  , neoPixGreen
  , neoPixBlue
  , neoPixFade
  , neoPixBlink
};

extern Adafruit_NeoPixel neoPixel;

void neoPixOff(int neoPixNr);
void neoPixOn(int neoPixNr, neoPixColor color);
void neoPixHeartBeat(int neoPixNr, int16_t heartRate, neoPixColor color, int type=neoPixFade);

#endif // NEOPIXSTUFF_H

