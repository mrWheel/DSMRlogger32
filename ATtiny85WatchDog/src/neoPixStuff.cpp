/*
** part of ATtiny85 Watch Dog 
*/ 

#include "neoPixStuff.h"

byte pixels[_NEO_PIXELS_COUNT * 3];

// Parameter 1 = number of pixels in neoPixel
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)
Adafruit_NeoPixel neoPixel = Adafruit_NeoPixel(_NEO_PIXELS_COUNT, _PIN_NEOPIXELS, NEO_GRB + NEO_KHZ800);


//----------------------------------------------------------------
void neoPixOff(int neoPixNr) 
{
  if (neoPixNr >= _NEO_PIXELS_COUNT) return;
  neoPixel.setPixelColor(neoPixNr, 0, 0, 0);
  neoPixel.show();
  
} //  neoPixOff()


//----------------------------------------------------------------
void neoPixOn(int neoPixNr, neoPixColor color) 
{
  if (neoPixNr >= _NEO_PIXELS_COUNT) return;

  switch(color)
  {
     //--  Set pixel's color (in RAM)
    case neoPixRed:
            neoPixel.setPixelColor(neoPixNr, 255, 0, 0);
            break;
    case neoPixGreen:
            neoPixel.setPixelColor(neoPixNr, 0, 255, 0);
            break;
    case neoPixBlue:
            neoPixel.setPixelColor(neoPixNr, 0, 0, 255);
            break;
    default:  //-- neoPixWhite
            neoPixel.setPixelColor(neoPixNr, 255, 255, 255);
  }
  //-- Update strip
  neoPixel.show();                      
  
} //  neoPixOn()


//----------------------------------------------------------------
void neoPixHeartBeat(int neoPixNr, int16_t heartRate, neoPixColor color, int type) //  =neoPixFade) 
{
  if (neoPixNr >= _NEO_PIXELS_COUNT) return;
  
  int16_t moduloHeartRate = (millis() % heartRate);
  int16_t brightness  = map(moduloHeartRate, 0, heartRate, -255, 255);
  
  if (type==neoPixFade)
        brightness  = abs(brightness);
  else
  {
        if (brightness < 50) brightness = 0;
        else                 brightness = 255;
  }
  
  switch(color)
  {
     //--  Set pixel's color (in RAM)
    case neoPixRed:
            neoPixel.setPixelColor(neoPixNr, brightness, 0, 0);
            break;
    case neoPixGreen:
            neoPixel.setPixelColor(neoPixNr, 0, brightness, 0);
            break;
    case neoPixBlue:
            neoPixel.setPixelColor(neoPixNr, 0, 0, brightness);
            break;
    default:  //-- neoPixWhite
            neoPixel.setPixelColor(neoPixNr, brightness, brightness,brightness);
  }
  neoPixel.show();                      //  Update strip to match

} //  neoPixHeartBeat()

