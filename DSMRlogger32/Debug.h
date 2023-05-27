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

/*---- start macro's ------------------------------------------------------------------*/

#define Debug(...)      ({ Serial.print(__VA_ARGS__);         \
                           TelnetStream.print(__VA_ARGS__);   \
                           DebugFlush();                       \
                        })
#define Debugln(...)    ({ Serial.println(__VA_ARGS__);       \
                           TelnetStream.println(__VA_ARGS__); \
                           DebugFlush();                       \
                        })
#define Debugf(...)     ({ Serial.printf(__VA_ARGS__);        \
                           TelnetStream.printf(__VA_ARGS__);  \
                           DebugFlush();                       \
                        })

#define DebugFlush()    ({ Serial.flush(); \
                           TelnetStream.flush(); \
                        })


#define DebugT(...)     ({ _debugBOL(__FUNCTION__, __LINE__);  \
                           Debug(__VA_ARGS__);                 \
                           DebugFlush();                       \
                        })
#define DebugTln(...)   ({ _debugBOL(__FUNCTION__, __LINE__);  \
                           Debugln(__VA_ARGS__);        \
                           DebugFlush();                       \
                        })
#define DebugTf(...)    ({ _debugBOL(__FUNCTION__, __LINE__);  \
                           Debugf(__VA_ARGS__);                \
                           DebugFlush();                       \
                        })

/*---- einde macro's ------------------------------------------------------------------*/

//-- [TelnetStream@1.2.2] - https://github.com/jandrassy/TelnetStream
//-- needs #include <TelnetStream.h>
char _bol[128];
void _debugBOL(const char *fn, int line)
{

  snprintf(_bol, sizeof(_bol), "[%02d:%02d:%02d] %-20.20s(%4d): " \
           , tzEurope.hour(), tzEurope.minute(), tzEurope.second() \
           , fn, line);

  Serial.print (_bol);
  Serial.flush();       //esp32
  TelnetStream.print (_bol);
  TelnetStream.flush(); //esp32
}
