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

// Constructor
Shield32::Shield32() {}

//--------------------------------------------------------------------------------------------
void Shield32::setup(int pinNr, int8_t inversedLogic, int activeStart, int activeStop, int onValue, int offValue, uint32_t onDelay, uint32_t offDelay)
{
  esp_log_level_set("Shield32", ESP_LOG_INFO);
  Shield32::_pinNr          = pinNr;
  Shield32::_inversedLogic  = inversedLogic;
  Shield32::_activeStart    = activeStart;
  Shield32::_activeStop     = activeStop;
  Shield32::_onValue        = onValue;
  Shield32::_offValue       = offValue;
  Shield32::_onDelay        = onDelay;
  Shield32::_offDelay       = offDelay;
  if (Shield32::_onDelay > (30*1000)) 
  {
    //-- wait 30 seconds before activating the Shield
    Shield32::_switchDelay  = millis() + (30 *1000);  
  }
  else
  {
    //-- wait onDelay seconds before activating the Shield
    Shield32::_switchDelay  = millis() + (Shield32::_onDelay *1000);  
  }

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

  ESP_LOGI(TAG, "====> setup(pinNr[%d], HIGH[%s], onValue[%d], offValue[%d], onDelay[%d], offDelay[%d])\r\n", Shield32::_pinNr
                                                                                        , (Shield32::_inversedLogic ? "LOW" : "HIGH")
                                                                                        , Shield32::_onValue, Shield32::_offValue
                                                                                        , Shield32::_onDelay
                                                                                        , Shield32::_offDelay);
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

  if (Shield32::_mustFlip)
  {
      ESP_LOGI(TAG, "=====> flipSwitch()");
      Shield32::_mustFlip = false;

      if (digitalRead(_pinNr) == Shield32::_LOW)
      {
        digitalWrite(Shield32::_pinNr, Shield32::_HIGH);
        //-- set _switchDelay in ms
        Shield32::_switchDelay = millis() + (Shield32::_offDelay *1000);  
        Shield32::shieldState = 3;
      }
      else
      {
        digitalWrite(Shield32::_pinNr, Shield32::_LOW);
        //-- set _switchDelay in ms
        Shield32::_switchDelay = millis() + (Shield32::_onDelay *1000);  
        Shield32::shieldState = 1;
      }
  }

  //-- States:
  //-- 0 - initialize shield, set output LOW
  //-- 1 - wait for due _switchDelay
  //-- 2 - if _onValue is reached, set output HIGH
  //-- 3 - wait for due _switchDelay
  //-- 4 - if _offValue is reached, set output LOW
  //-- 5 - wait for due _switchDelay

  switch(Shield32::shieldState)
  {
    case 0: //-- initialize shield, set output LOW
          {
            digitalWrite(Shield32::_pinNr, Shield32::_LOW);
            Shield32::shieldState = 1;
            //-- fall through to next state (1)          
          }

    case 1: //-- wait for due _switchDelay
          {
            if (millis() < (Shield32::_switchDelay))
            {
              ESP_LOGI(TAG, "=====> [1] Wait[%d] seconds till due On Delay ...", (Shield32::_switchDelay - millis()) /1000);
              Shield32::shieldState = 1;
              break;
            }
            else
            {
              ESP_LOGI(TAG, "=====> [1] Done waiting!");
              Shield32::shieldState = 2;
            }
          }

    case 2: //-- if _onValue is reached, set output HIGH
          {
            ESP_LOGI(TAG, "=====> [2] Check if act[%d] > on[%d]", actualValue, Shield32::_onValue);
            if (digitalRead(Shield32::_pinNr) == Shield32::_HIGH) //-- allready "On"
            {
              ESP_LOGI(TAG, "=====> [2] Switch already HIGH");
              Shield32::shieldState = 3;
              break;
            }
            if (actualValue >= Shield32::_onValue)
            {
              ESP_LOGI(TAG, "=====> [2] Set Switch to HIGH (set switch off delay to [%d] seconds)", Shield32::_offDelay);
              digitalWrite(Shield32::_pinNr, Shield32::_HIGH);
              //-- set _switchDelay in ms
              Shield32::_switchDelay = millis() + (Shield32::_offDelay *1000);  
              Shield32::shieldState = 3;
              break;
            }
            Shield32::shieldState = 2;
            break;
          }

    case 3: //-- wait for due _switchDelay
          {
            if (millis() < (Shield32::_switchDelay))
            {
              ESP_LOGI(TAG, "=====> [3] Wait[%d] seconds till due Off Delay ...", (Shield32::_switchDelay - millis()) / 1000);
              Shield32::shieldState = 3;
              break;
            }
            else
            {
              ESP_LOGI(TAG, "=====> [3] Done waiting!");
              Shield32::shieldState = 4;
            }
          }

    case 4: //-- if _offValue is reached, set output LOW
          {
            ESP_LOGI(TAG, "=====> [4] Check if act[%d] < off[%d]", actualValue, Shield32::_offValue);
            if (digitalRead(Shield32::_pinNr) == Shield32::_LOW) //-- allready "Off"
            {
              ESP_LOGI(TAG, "=====> [4] Switch already LOW");
              Shield32::shieldState = 1;
              break;
            } 
            if (actualValue < Shield32::_offValue)
            {
              ESP_LOGI(TAG, "=====> [3] Set Switch to LOW (set switch on delay to [%d] seconds)", Shield32::_onDelay);
              digitalWrite(Shield32::_pinNr, Shield32::_LOW);
              //-- set _switchDelay in ms
              Shield32::_switchDelay = millis() + (Shield32::_onDelay *1000);  
              Shield32::shieldState = 1;
              break;
            }
            Shield32::shieldState = 4;
            break;
          }

    default:  Shield32::shieldState = 0;

  } // switch(Shield32::shieldState)

} //  loop()

//--------------------------------------------------------------------------------------------
bool Shield32::isActive(int thisTimeMinutes)
{
    // Case 1: start == eind, altijd actief
    if (_activeStart == _activeStop) 
    {
        return true;
    }
    // Case 2: start <= eind, eenvoudig interval op dezelfde dag
    if (_activeStart <= _activeStop) 
    {
        // Het actieve interval is tussen start en eind
        return (bool)(thisTimeMinutes >= _activeStart && thisTimeMinutes < _activeStop);
    } 
    // Case 3: start > eind, het interval gaat over middernacht heen
    else 
    {
        // Het actieve interval is van start tot middernacht, OF van middernacht tot eind
        return (bool)(thisTimeMinutes >= _activeStart || thisTimeMinutes < _activeStop);
    }
} //  isActive()

//--------------------------------------------------------------------------------------------
bool Shield32::getRelayState()
{
  if (digitalRead(Shield32::_pinNr) == Shield32::_HIGH)
        return 1;
  else  return 0;

} // getRelayState()

//--------------------------------------------------------------------------------------------
void Shield32::setRelayState(bool state)
{
  if (state)
        digitalWrite(Shield32::_pinNr, Shield32::_HIGH);
  else  digitalWrite(Shield32::_pinNr, Shield32::_LOW);

} // setRelayState()


//--------------------------------------------------------------------------------------------
void Shield32::flipSwitch()
{
  Shield32::_mustFlip = true;

} // flipSwitch()


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
