/*
***************************************************************************
**  Program  : shield, part of DSMRlogger32
**  Version  : v5.n
**
**  Copyright (c) 204 Willem Aandewiel
**
**  TERMS OF USE: MIT License. See bottom of file.
***************************************************************************
*/
#include "Shield32.h"


Shield32::Shield32() {}

//--------------------------------------------------------------------------------------------
void Shield32::setup(int pinNr, int8_t monitorFase, int onValue, int offValue, int16_t onHysteresis)
{
  _pinNr = pinNr;
  _monitorFase = monitorFase;
  _onValue = onValue;
  _offValue = offValue;
  _onHysteresis = onHysteresis;
  Serial.printf("Shield32::setup(pinNr[%d], monitorFase[%d], onValue[%d], offValue[%d], onHyseresis[%d])\r\n", _pinNr
                                                                                                             , _monitorFase
                                                                                                             , _onValue, _offValue
                                                                                                             , onHysteresis);
  pinMode(_pinNr, OUTPUT);

} // setup()

//--------------------------------------------------------------------------------------------
void Shield32::loop(int actualValue)
{
  Serial.printf("==============>>  Fase[%d], OnValue[%d], OffValue[%d], onHysteresys[%d]\r\n", _monitorFase, _onValue, _offValue, _onHysteresis);
  _switchOnDelay = millis();
  if (actualValue >= _onValue)
  {
    if ((millis() - _switchOnDelay) > (_onHysteresis*1000))
    {
      if (digitalRead(_pinNr))
            Serial.printf("==============>>  Shield32::already(ON) with power[%d]\r\n", actualValue);
      else  Serial.printf("==============>>  Shield32::switch(ON) with power[%d]\r\n", actualValue);
      _switchOnDelay = millis();  
      digitalWrite(_pinNr, HIGH);
    }
    else Serial.printf("==============>>  Shield32::hysteresis[%d]\r\n", (millis() - _switchOnDelay) );
  }
  if (actualValue < _offValue)
  {
    if (digitalRead(_pinNr)) 
    {
          //-- reset _switchOnDelay only if state was HIGH!
          _switchOnDelay = millis();
          Serial.printf("==============>>  Shield32::switch(OFF)  with power[%d]\r\n", actualValue);
    }
    else  Serial.printf("==============>>  Shield32::already(OFF)  with power[%d]\r\n", actualValue);
    digitalWrite(_pinNr, LOW);
  }

} // loop()


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