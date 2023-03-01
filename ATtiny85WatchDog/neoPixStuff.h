/*
** part of ATtiny85 Watch Dog 
*/ 

//-- How many NeoPixels are attached to the Arduino?
#define _NEO_PIXELS_COUNT         2

//-- NeoPixel brightness, 0 (min) to 255 (max)
#define _NEO_BRIGHTNESS       50 

//-- Since this is for the static version of the library, we need to supply the 
//-- pixel array. This saves space by eliminating use of malloc() and free(), 
//-- and makes the RAM used for the frame buffer show up when the sketch is 
//-- compiled.
byte pixels[_NEO_PIXELS_COUNT * 3];

//-- When we setup the NeoPixel library, we tell it how many pixels, and which 
//-- pin to use to send signals.
//-- Note that for older NeoPixel strips you might need to change the third 
//-- parameter--see the strandtest example for more information on possible 
//-- values.

//-- Declare our NeoPixel strip object:
tinyNeoPixel neoPixel = tinyNeoPixel(_NEO_PIXELS_COUNT, _PIN_NEOPIXELS, NEO_GRB, pixels);
//-- Argument 1 = Number of pixels in NeoPixel strip
//-- Argument 2 = Arduino pin number (most are valid)
//-- Argument 3 = Pixel type flags, add together as needed:
//--   + NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//--   + NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//--   + NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//--   + NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)

enum neoPixColor {
    neoPixWhite
  , neoPixRed
  , neoPixGreen
  , neoPixBlue
  , neoPixFade
  , neoPixBlink
};


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
void neoPixHeartBeat(int neoPixNr, int16_t heartRate, neoPixColor color, int type=neoPixFade) 
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


//EOF//
