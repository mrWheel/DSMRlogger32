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

//============ Added by Convertor ==========


/*---- start macro's ------------------------------------------------------------------*/

	//-- moved to arduinoGlue.h // #define Debug(...)      ({ Serial.print(__VA_ARGS__);         \
	//-- moved to arduinoGlue.h //                            TelnetStream.print(__VA_ARGS__);   \
	//-- moved to arduinoGlue.h //                            DebugFlush();                       \
	//-- moved to arduinoGlue.h //                         })
	//-- moved to arduinoGlue.h // #define Debugln(...)    ({ Serial.println(__VA_ARGS__);       \
	//-- moved to arduinoGlue.h //                            TelnetStream.println(__VA_ARGS__); \
	//-- moved to arduinoGlue.h //                            DebugFlush();                       \
	//-- moved to arduinoGlue.h //                         })
	//-- moved to arduinoGlue.h // #define Debugf(...)     ({ Serial.printf(__VA_ARGS__);        \
	//-- moved to arduinoGlue.h //                            TelnetStream.printf(__VA_ARGS__);  \
	//-- moved to arduinoGlue.h //                            DebugFlush();                       \
	//-- moved to arduinoGlue.h //                         })

	//-- moved to arduinoGlue.h // #define DebugFlush()    ({ Serial.flush(); \
	//-- moved to arduinoGlue.h //                            TelnetStream.flush(); \
	//-- moved to arduinoGlue.h //                         })

	//-- moved to arduinoGlue.h // #define DebugT(...)     ({ _debugBOL(__FUNCTION__, __LINE__);  \
	//-- moved to arduinoGlue.h //                            Debug(__VA_ARGS__);                 \
	//-- moved to arduinoGlue.h //                            DebugFlush();                       \
	//-- moved to arduinoGlue.h //                         })
	//-- moved to arduinoGlue.h // #define DebugTln(...)   ({ _debugBOL(__FUNCTION__, __LINE__);  \
	//-- moved to arduinoGlue.h //                            Debugln(__VA_ARGS__);        \
	//-- moved to arduinoGlue.h //                            DebugFlush();                       \
	//-- moved to arduinoGlue.h //                         })
	//-- moved to arduinoGlue.h // #define DebugTf(...)    ({ _debugBOL(__FUNCTION__, __LINE__);  \
	//-- moved to arduinoGlue.h //                            Debugf(__VA_ARGS__);                \
	//-- moved to arduinoGlue.h //                            DebugFlush();                       \
	//-- moved to arduinoGlue.h //                         })

/*---- einde macro's ------------------------------------------------------------------*/

//-- [TelnetStream@1.2.2] - https://github.com/jandrassy/TelnetStream
//-- needs #include <TelnetStream.h>

char _bol[128];
void _debugBOL(const char *fn, int line)
{

  snprintf(_bol, sizeof(_bol), "[%02d:%02d:%02d] %-20.20s(%4d): " \
           , localtime(&now)->tm_hour, localtime(&now)->tm_min, localtime(&now)->tm_sec \
           , fn, line);

  Serial.print (_bol);
  Serial.flush();       //esp32
  TelnetStream.print (_bol);
  TelnetStream.flush(); //esp32
}

#endif // DEBUG_H