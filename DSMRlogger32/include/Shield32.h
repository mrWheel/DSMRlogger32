#ifndef SHIELD32_H
#define SHIELD32_H
/*
***************************************************************************
**  Program  : shield32.h, part of DSMRlogger32
**  Version  : v2.0.1
**
**  Copyright (c) 204 Willem Aandewiel
**
**  TERMS OF USE: MIT License. See bottom of file.
***************************************************************************
*/
//============ Includes ====================
#include "Arduino.h"

class Shield32 
{
    public:
        Shield32();
        void setup(int pinNr, int8_t monitorFase, int onValue, int offValue, int16_t onHysteresis);
        void loop(int actualValue);

    private:
        int       _pinNr;
        int8_t    _monitorFase;
        int       _onValue;
        int       _offValue;
        uint32_t  _onHysteresis;
        uint32_t  _switchOnDelay = 0;
};

#endif // SHIELD32_H

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