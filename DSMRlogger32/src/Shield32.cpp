/*
***************************************************************************
**  Program  : Shield32, part of DSMRlogger32
**  Version  : v5.n
**
**  Copyright (c) 2024 Willem Aandewiel
**
**  TERMS OF USE: MIT License. See bottom of file.
***************************************************************************
*/

#include <esp_log.h>
#include "Shield32.h"

#define MAX_LOG_BUFFER_SIZE 512  

static const char *TAG = "Shield32";

//Shield32::Shield32() {}
// Constructor
Shield32::Shield32() {}

//--------------------------------------------------------------------------------------------
void Shield32::setup(int pinNr, int8_t inversedLogic, int onValue, int offValue, int16_t onHysteresis)
{
  esp_log_level_set("Shield32", ESP_LOG_INFO);
  Shield32::_pinNr          = pinNr;
  Shield32::_inversedLogic  = inversedLogic;
  Shield32::_onValue        = onValue;
  Shield32::_offValue       = offValue;
  Shield32::_onHysteresis   = onHysteresis;
  Shield32::_switchOnDelay  = millis() + (_onHysteresis * 1000);

  if (Shield32::_inversedLogic)
  {
    Shield32::_HIGH = LOW;
    Shield32::_LOW  = HIGH;
  }
  else
  {  
    Shield32::_HIGH = HIGH;
    Shield32::_LOW  = LOW;
  }

  ESP_LOGI(TAG, "====> setup(pinNr[%d], HIGH[%s], onValue[%d], offValue[%d], onHyseresis[%d])\r\n", Shield32::_pinNr
                                                                                        , (Shield32::_inversedLogic ? "LOW" : "HIGH")
                                                                                        , Shield32::_onValue, Shield32::_offValue
                                                                                        , onHysteresis);
  if (Shield32::_pinNr >= 0)
  {
    digitalWrite(Shield32::_pinNr, Shield32::_LOW);
    pinMode(Shield32::_pinNr, OUTPUT);
    digitalWrite(Shield32::_pinNr, Shield32::_LOW);
  }

} // setup()


//--------------------------------------------------------------------------------------------
void Shield32::loop(int actualValue)
{
  if (Shield32::_pinNr < 0) 
  {
    ESP_LOGI(TAG, "=====> No Shield pin set");
    return;
  }

  switch(Shield32::shieldState)
  {
    case 0: //-- initialize shield, set output LOW
          {
            digitalWrite(Shield32::_pinNr, Shield32::_LOW);
            Shield32::shieldState = 1;
            //-- fall through to next state (1)          
          }

    case 1: //-- wait for due _switchOnDelay
          {
            if (millis() > Shield32::_switchOnDelay)
                  Shield32::shieldState = 2;
            else  Shield32::shieldState = 1;
            break;
          }

    case 2: //-- if _onValue is reached, set output HIGH
          {
            if (digitalRead(Shield32::_pinNr) == Shield32::_HIGH) //-- allready "On"
            {
              Shield32::shieldState = 3;
              break;
            }
            if (actualValue >= Shield32::_onValue)
            {
              ESP_LOGI(TAG, "=====> [2] Switch HIGH");
              digitalWrite(Shield32::_pinNr, Shield32::_HIGH);
              Shield32::shieldState = 3;
            }
            Shield32::shieldState = 2;
            break;
          }

    case 3: //-- if _offValue is reached, set output LOW
          {
            if (digitalRead(Shield32::_pinNr) == Shield32::_LOW) //-- allready "Off"
            {
              Shield32::shieldState = 2;
              break;
            } 
            if (actualValue < Shield32::_offValue)
            {
              ESP_LOGI(TAG, "=====> [3] Switch LOW");
              digitalWrite(Shield32::_pinNr, Shield32::_LOW);
              Shield32::shieldState = 1;
              Shield32::_switchOnDelay = millis() + (Shield32::_onHysteresis *1000);  
            }
            break;
          }

    default:  Shield32::shieldState = 0;

  } // switch(Shield32::shieldState)

} //  loop()


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
***************************************************************************
*/
