#ifndef DEBUG_H
#define DEBUG_H

/*
***************************************************************************
**  Program  : Debug.h, part of DSMRlogger32
**  Version  : v2.0.1
**
**  Copyright (c) 2020 .. 2022 Willem Aandewiel
**  Met dank aan Erik
**
**  TERMS OF USE: MIT License. See bottom of file.
***************************************************************************
*/
//============ Includes ====================
#include "arduinoGlue.h"

char _bol[128];
void _debugBOL(const char *fn, int line)
{
  time(&now);
  snprintf(_bol, sizeof(_bol), "[%02d:%02d:%02d] %-20.20s(%4d): " \
           , localtime(&now)->tm_hour, localtime(&now)->tm_min, localtime(&now)->tm_sec \
           , fn, line);

  Serial.print (_bol);
  Serial.flush();       //esp32
  TelnetStream.print (_bol);
  TelnetStream.flush(); //esp32
}

#endif // DEBUG_H