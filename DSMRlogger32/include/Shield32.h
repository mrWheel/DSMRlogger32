#ifndef SHIELD32_H
#define SHIELD32_H
/*
***************************************************************************
**  Program  : Shield32.h, part of DSMRlogger32
**  Version  : v5.n
**
**  Copyright (c) 2024 Willem Aandewiel
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
        void setup(int pinNr, int8_t inversedLogic, int onValue, int offValue, uint32_t onDelay, uint32_t offDelay);
        void loop(int actualValue);
        void flipSwitch();
        bool getRelayState();

    private:
        static int customTelnetVprintf(const char* fmt, va_list args);
        
        int       _pinNr;
        int8_t    _inversedLogic;
        int       _onValue;
        int       _offValue;
        uint32_t  _onDelay;
        uint32_t  _offDelay;
        int32_t   _switchDelay = 0;
        int8_t    shieldState = 0;
        uint8_t   _HIGH = 1;
        uint8_t   _LOW  = 0;
        bool      _mustFlip = false;
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