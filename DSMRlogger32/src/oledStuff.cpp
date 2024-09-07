/*
***************************************************************************
**  Program  : oledStuff, part of DSMRlogger32
**  Version  : v5.n
**
**  Copyright (c) 2020 .. 2023 Willem Aandewiel
**
**  TERMS OF USE: MIT License. See bottom of file.
***************************************************************************
*/
#include "oledStuff.h"

//------ [SSD1306Ascii@1.3.0]
//#include <SSD1306Ascii.h>                         		//-- moved to arduinoGlue.h
//------ [Wire@2.0.0]
//#include <SSD1306AsciiWire.h>                     		//-- moved to arduinoGlue.h

// 0X3C+SA0 - 0x3C or 0x3D
	//-- moved to arduinoGlue.h // #define I2C_ADDRESS 0x3C

// Define proper RST_PIN if required.
	//-- moved to arduinoGlue.h // #define RST_PIN -1

SSD1306AsciiWire oled;

void oled_Print_Msg(uint8_t, String, uint16_t);

//static bool     buttonState = LOW;
//static uint8_t  msgMode = 0;
//static bool     boolDisplay = true;
//static uint8_t  settingOledType = 1;  // 0=none, 1=SSD1306, 2=SH1106
//static uint16_t settingOledSleep;
//static uint8_t  settingOledFlip;

uint8_t     lineHeight, charHeight;

//===========================================================================================
void checkFlashButton()
{
  //-- if the display timer is turned off, then don't check flashbutton
  if (devSetting->OledSleep == 0) return;  

  //--check if the displaytimer is due...
  if ( (devSetting->OledSleep > 0) && boolDisplay && DUE(oledSleepTimer) )
  {
    DebugTln("Switching display off..");
    oled.clear();
    boolDisplay = false;
  }
  //-- check the button and turn it on.
  if (digitalRead(_FLASH_BUTTON) == LOW && buttonState == LOW)
  {
    DebugTln(F("Pressed the FlashButton!"));
    buttonState = HIGH;
  }
  else if (digitalRead(_FLASH_BUTTON) == HIGH && buttonState == HIGH)
  {
    buttonState = LOW;
    boolDisplay = !boolDisplay;
    if (boolDisplay)
    {
      DebugTln(F("Switching display on.."));
    }
    else
    {
      DebugTln(F("Switching display off.."));
    }
    oled.clear();
    oled_Print_Msg(0, ">>DSMR-logger32<<", 0);
    oled_Print_Msg(2, "Wacht ...", 5);
    msgMode = 0; //reset the display loop
    RESTART_TIMER(oledSleepTimer);
  }

} // checkFlashButton()


//===========================================================================================
void oled_Init()
{
  Wire.begin();
  if (devSetting->OledType == 2)
        oled.begin(&SH1106_128x64, I2C_ADDRESS);
  else  oled.begin(&Adafruit128x64, I2C_ADDRESS);

  oled.setFont(X11fixed7x14B);  // this gives us 4 rows by 18 chars
  charHeight  = oled.fontHeight();
  lineHeight  = oled.displayHeight() / 4;
  DebugTf("OLED is [%3dx%3d], charHeight[%d], lineHeight[%d], nrLines[%d]\r\n", oled.displayWidth()
          , oled.displayHeight()
          , charHeight, lineHeight, 4);
  boolDisplay = true;
  if (devSetting->OledFlip)  oled.displayRemap(true);
  RESTART_TIMER(oledSleepTimer);

}   // oled_Init()

//===========================================================================================
void oled_Clear()
{
  oled.clear();

}   // oled_Clear


//===========================================================================================
DECLARE_TIMER_MS(timer, 0);
void oled_Print_Msg(uint8_t line, String message, uint16_t wait)
{
  if (!boolDisplay) return;

  message += "                    ";
  oled.setCursor(0, ((line * lineHeight)/8));
  oled.print(message.c_str());

  if (wait>0)
  {
    CHANGE_INTERVAL_MS(timer, wait);
    RESTART_TIMER(timer);
    while (!DUE(timer))
    {
      delay(1);
    }
  }

}   // oled_Print_Msg()


/***************************************************************************
*
* Permission is hereby granted, free of charge, to any person obtaining a
* copy of this software and associated documentation files (the
* "Software"), to deal in the Software without restriction, including
* without limitation the rights to use, copy, modify, merge, publish,
* distribute, sublicense, and/or sell copies of the Software, and to permit
* persons to whom the Software is furnished to do so, subject to the
* following conditions:
*
* The above copyright notice and this permission notice shall be included
* in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
* OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT
* OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR
* THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*
***************************************************************************/