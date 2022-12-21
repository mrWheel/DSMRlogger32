

//----------------------------------------------------------------
// Fill strip pixels one after another with a color. Strip is NOT cleared
// first; anything there will be covered pixel by pixel. Pass in color
// (as a single 'packed' 32-bit value, which you can get by calling
// neoPixel.Color(red, green, blue) as shown in the loop() function above),
// and a delay time (in milliseconds) between pixels.
void colorWipe(uint32_t color, int wait) 
{
  for(int i=0; i<neoPixel.numPixels(); i++)  
  {                                       // For each pixel in strip...
    neoPixel.setPixelColor(i, color);     //  Set pixel's color (in RAM)
    neoPixel.show();                      //  Update strip to match
    delay(wait);                          //  Pause for a moment
  }
} //  colorWipe()


//----------------------------------------------------------------
void whiteOverRainbow(int whiteSpeed, int whiteLength) 
{
  if(whiteLength >= neoPixel.numPixels()) whiteLength = neoPixel.numPixels() - 1;

  int      head          = whiteLength - 1;
  int      tail          = 0;
  int      loops         = 3;
  int      loopNum       = 0;
  uint32_t lastTime      = millis();
  uint32_t firstPixelHue = 0;

  for(;;)  // Repeat forever (or until a 'break' or 'return')
  {
    for(int i=0; i<neoPixel.numPixels(); i++)
    {  // For each pixel in strip...
      if(((i >= tail) && (i <= head)) ||      //  If between head & tail...
         ((tail > head) && ((i >= tail) || (i <= head)))) 
      {
        neoPixel.setPixelColor(i, neoPixel.Color(0, 0, 0, 255)); // Set white
      } 
      else 
      {                                             // else set rainbow
        int pixelHue = firstPixelHue + (i * 65536L / neoPixel.numPixels());
        neoPixel.setPixelColor(i, neoPixel.gamma32(neoPixel.ColorHSV(pixelHue)));
      }
    }

    neoPixel.show(); // Update strip with new contents
    //-- There's no delay here, it just runs full-tilt until the timer and
    //-- counter combination below runs out.

    firstPixelHue += 40; // Advance just a little along the color wheel

    //-- Time to update head/tail?
    if((millis() - lastTime) > whiteSpeed)
    {                                     
      //-- Advance head, wrap around
      if(++head >= neoPixel.numPixels())       
      {      
        head = 0;
        if(++loopNum >= loops) return;
      }
      //-- Advance tail, wrap around
      if(++tail >= neoPixel.numPixels()) 
      { 
        tail = 0;
      }
      //-- Save time of last movement
      lastTime = millis();                   
    }
  }
} //  whiteOverRainbow()


//----------------------------------------------------------------
void pulseWhite(uint8_t wait) 
{
  //-- Ramp up from 0 to 255
  for(int j=0; j<256; j++) 
  {
    //-- Fill entire strip with white at gamma-corrected brightness level 'j':
    neoPixel.fill(neoPixel.Color(0, 0, 0, neoPixel.gamma8(j)));
    neoPixel.show();
    delay(wait);
  }

  //-- Ramp down from 255 to 0
  for(int j=255; j>=0; j--) 
  { 
    neoPixel.fill(neoPixel.Color(0, 0, 0, neoPixel.gamma8(j)));
    neoPixel.show();
    delay(wait);
  }

} //  pulseWhite()


//----------------------------------------------------------------
void rainbowFade2White(int wait, int rainbowLoops, int whiteLoops) 
{
  int fadeVal=0, fadeMax=100;

  // Hue of first pixel runs 'rainbowLoops' complete loops through the color
  // wheel. Color wheel has a range of 65536 but it's OK if we roll over, so
  // just count from 0 to rainbowLoops*65536, using steps of 256 so we
  // advance around the wheel at a decent clip.
  for(uint32_t firstPixelHue = 0; 
                firstPixelHue < rainbowLoops*65536;
                firstPixelHue += 256) 
  {
    //-- For each pixel in strip...
    for(int i=0; i<neoPixel.numPixels(); i++) 
    { 
      // Offset pixel hue by an amount to make one full revolution of the
      // color wheel (range of 65536) along the length of the strip
      // (neoPixel.numPixels() steps):
      uint32_t pixelHue = firstPixelHue + (i * 65536L / neoPixel.numPixels());

      // neoPixel.ColorHSV() can take 1 or 3 arguments: a hue (0 to 65535) or
      // optionally add saturation and value (brightness) (each 0 to 255).
      // Here we're using just the three-argument variant, though the
      // second value (saturation) is a constant 255.
      neoPixel.setPixelColor(i, neoPixel.gamma32(neoPixel.ColorHSV(pixelHue, 255
                                                      ,255 * fadeVal / fadeMax)));
    }

    neoPixel.show();
    delay(wait);
    //-- First loop,
    if(firstPixelHue < 65536) 
    {                              
      //-- fade in
      if (fadeVal < fadeMax) fadeVal++;                       
    }    //-- Last loop,
    else if(firstPixelHue >= ((rainbowLoops-1) * 65536)) 
    {
      //-- fade out
      if(fadeVal > 0) fadeVal--;                             
    } 
    else 
    {
      //-- Interim loop, make sure fade is at max
      fadeVal = fadeMax; 
    }
  }

  for(int k=0; k<whiteLoops; k++) 
  {
    //-- Ramp up 0 to 255
    for(int j=0; j<256; j++) 
    { 
      //-- Fill entire strip with white at gamma-corrected brightness level 'j':
      neoPixel.fill(neoPixel.Color(0, 0, 0, neoPixel.gamma8(j)));
      neoPixel.show();
    }
    delay(1000); // Pause 1 second
    //-- Ramp down 255 to 0
    for(int j=255; j>=0; j--) 
    {
      neoPixel.fill(neoPixel.Color(0, 0, 0, neoPixel.gamma8(j)));
      neoPixel.show();
    }
  }

  delay(500); // Pause 1/2 second
  
} //  rainbowFade2White()

/*eof*/
