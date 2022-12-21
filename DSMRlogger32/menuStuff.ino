/*
***************************************************************************
**  Program  : menuStuff, part of DSMRlogger32
**  Version  : v5.n
**
**  Copyright (c) 2022, 2023 Willem Aandewiel
**
**  TERMS OF USE: MIT License. See bottom of file.
***************************************************************************
*/


//===========================================================================================
void displayHoursHist(bool Telnet=true)
{
  char dummy[DATA_RECLEN] = {0};
  
  readAllSlots(dummy, RNG_HOURS, HOURS_FILE, lastTlgrmTime);

} // displayHoursHist()


//===========================================================================================
void displayDaysHist(bool Telnet=true)
{
  char dummy[DATA_RECLEN] = {0};
  
  readAllSlots(dummy, RNG_DAYS, DAYS_FILE, lastTlgrmTime);

} // displayDaysHist()


//===========================================================================================
void displayMonthsHist(bool Telnet=true)
{
  char dummy[DATA_RECLEN] = {0};

  readAllSlots(dummy, RNG_MONTHS, MONTHS_FILE, lastTlgrmTime);

} // displayMonthsHist()


//===========================================================================================
void displayBoardInfo()
{
  Debugln(F("\r\n==================================================================\r"));
  Debug(F(" \r\n            (c) by [Willem Aandewiel"));

  Debug(F("]\r\n         Firmware Version ["));
  Debug( _FW_VERSION );
  Debug(F("]\r\n                 Compiled ["));
  Debug( __DATE__ );
  Debug( "  " );
  Debug( __TIME__ );
  Debug(F("]\r\n            compiled with [dsmr2.h"));
  Debug(F("]\r\n                 #defines "));
#ifdef _SHOW_PASSWRDS
  Debug(F("[SHOW_PASSWRDS]"));
#endif

  //-- https://github.com/espressif/arduino-esp32/blob/master/cores/esp32/Esp.h#L63
  
  Debug(F(" \r\n      Telegrams Processed ["));
  Debug( telegramCount );
  Debug(F("]\r\n              With Errors ["));
  Debug( telegramErrors );
  Debug(F("]\r\n                 FreeHeap ["));
  Debug( ESP.getFreeHeap() );
  Debug(F("]\r\n         Minimal FreeHeap ["));
  Debug( esp_get_minimum_free_heap_size() );
  Debug(F("]\r\n               Free PSRAM ["));
  Debug( ESP.getFreePsram() );
  Debug(F("]\r\n                max.Block ["));
  Debug( ESP.getMaxAllocHeap() );
  Debug(F("]\r\n                  Chip ID ["));
  Debug( ESP.getEfuseMac() );
  Debug(F("]\r\n               Chip Model ["));
  Debug( ESP.getChipModel() );
  //Debug(F("]\r\n           Core Version ["));
  //Debug( ESP.getSdkVersion() );
  Debug(F("]\r\n              SDK Version ["));
  Debug( ESP.getSdkVersion() );
  Debug(F("]\r\n           CPU Freq (MHz) ["));
  Debug( ESP.getCpuFreqMHz() );
  Debug(F("]\r\n      Sketch Size (bytes) ["));
  Debug( ESP.getSketchSize() );
  Debug(F("]\r\nFree Sketch Space (bytes) ["));
  Debug( ESP.getFreeSketchSpace() );
  Debug(F("]\r\n       PSRAM size (bytes) ["));
  Debug( ESP.getPsramSize() );
  Debug(F("]\r\n  Flash Chip Size (bytes) ["));
  Debug( ESP.getFlashChipSize() );
  //esp32 Debug(F("]\r\n   Chip Real Size (kB) ["));
  //esp32 Debug( ESP.getFlashChipRealSize() / 1024 );
#ifdef SPIFFS
  Debug(F("]\r\n         SPIFFS Size (kB) ["));
#else
  Debug(F("]\r\n       LittleFS Size (kB) ["));
#endif
  Debug( _FSYS.totalBytes() / 1024 );

  Debug(F("]\r\n   Flash Chip Speed (MHz) ["));
  Debug( ESP.getFlashChipSpeed() / 1000 / 1000 );
  FlashMode_t ideMode = ESP.getFlashChipMode();
  Debug(F("]\r\n          Flash Chip Mode ["));
  Debug( flashMode[ideMode] );

  Debugln(F("]\r"));

  Debugln(F("==================================================================\r"));
  Debug(F("]\r\n                     SSID ["));
  Debug( WiFi.SSID() );
#ifdef _SHOW_PASSWRDS
  Debug(F("]\r\n                  PSK key ["));
  Debug( WiFi.psk() );
#else
  Debug(F("]\r\n                  PSK key [**********"));
#endif
  Debug(F("]\r\n               IP Address ["));
  Debug( WiFi.localIP().toString() );
  Debug(F("]\r\n                 Hostname ["));
  Debug( sysSetting->Hostname );
  //esp32 Debug(F("]\r\n     Last reset reason ["));
  //esp32 Debug( ESP.getResetReason() );
  Debug(F("]\r\n        number of reboots ["));
  Debug( nrReboots );
  Debug(F("]\r\n                   upTime ["));
  Debug( upTime() );
  Debugln(F("]\r"));

  Debugln(F("==================================================================\r"));
  Debug(F(" \r\n              MQTT broker ["));
  Debug( sysSetting->MQTTbroker );
  Debug(F("]\r\n                MQTT User ["));
  Debug( sysSetting->MQTTuser );
#ifdef _SHOW_PASSWRDS
  Debug(F("]\r\n            MQTT PassWord ["));
  Debug( sysSetting->MQTTpasswd );
#else
  Debug(F("]\r\n            MQTT PassWord [**********"));
#endif
  Debug(F("]\r\n                Top Topic ["));
  Debug(sysSetting->MQTTtopTopic );
  Debug(F("]\r\n          Update Interval ["));
  Debug(sysSetting->MQTTinterval);
  Debugln(F("]\r"));
  Debugln(F("==================================================================\r\n\r"));

} // displayBoardInfo()


//===========================================================================================
void handleKeyInput(char inChar)
{
    yield();

    switch(inChar)
    {
      case 'b':
      case 'B':
        displayBoardInfo();
        break;
      case 'l':
      case 'L':
        readSmSettings(true);
        break;
      case 'd':
      case 'D':
        displayDaysHist(true);
        break;
      case 'E':
        eraseFile();
        break;
#if defined(_HAS_NO_SLIMMEMETER)
      case 'F':
        forceBuildRingFiles = true;
        runMode = SInit;
        break;
#endif
      case 'h':
      case 'H':
        displayHoursHist(true);
        break;

      case 'm':
      case 'M':
        displayMonthsHist(true);
        break;

      case 'U':
        writeDataToRingFiles(lastTlgrmTime);
        break;

      case 'W':
        Debugf("\r\nConnect to AP [%s] and go to ip address shown in the AP-name\r\n", sysSetting->Hostname);
        delay(1000);
        WiFi.disconnect(true);  // deletes credentials ?!
        writeToSysLog("Erase WiFi credentials! and restart..");
        startWiFi(sysSetting->Hostname, 240, true);
        delay(2000);
        ESP.restart();
        delay(2000);
        break;
        
      case 'i':
      case 'I':
        blinkNeoPixels(10, 1000);
        break;
        
      case 'p':
      case 'P':
        showRaw = !showRaw;
        //esp32 if (showRaw)  digitalWrite(DTR_ENABLE, HIGH);
        //esp32 else          digitalWrite(DTR_ENABLE, LOW);
        showRawCount = 0;
        break;
        
      case 'R':
        slotErrors      = 0;
        nrReboots       = 0;
        //telegramCount = 0;
        telegramErrors  = 0;
        writeLastStatus();
        DebugT(F("Reboot in 3 seconds ... \r\n"));
        DebugFlush();
        writeToSysLog("User requested ReBoot!");
        delay(3000);
        DebugTln(F("now Rebooting. \r"));
        DebugFlush();
        ESP.restart();
        break;
        
      case 's':
      case 'S':
        listFilesystem();
        break;
        
      case 'v':
      case 'V':
        if (Verbose2)
        {
          Debugln(F("Verbose is OFF\r"));
          Verbose1 = false;
          Verbose2 = false;
        }
        else if (Verbose1)
        {
          Debugln(F("Verbose Level 2 is ON\r"));
          Verbose2 = true;
        }
        else
        {
          Debugln(F("Verbose Level 1 is ON\r"));
          Verbose1 = true;
          Verbose2 = false;
        }
        break;
      case 'y': skipHeartbeats = false;
                break;
      case 'Y': skipHeartbeats = !skipHeartbeats;
                break;
      case 'Z':
        slotErrors      = 0;
        nrReboots       = 0;
        telegramCount   = 0;
        telegramErrors  = 0;
        writeLastStatus();
        sysLog.begin(_SYSLOG_LINES, _SYSLOG_LINE_LEN, true);
        writeToSysLog("reset Counters and sysLog by user");                         
        break;
        
      default:
        Debugln(F("\r\nCommands are:\r\n"));
        Debugln(F("   B - Board Info\r"));
        Debugln(F("  *E - erase file from Filesystem\r"));
        Debugln(F("   L - list Settings\r"));
        Debugln(F("   D - Display Day table from Filesystem\r"));
        Debugln(F("   H - Display Hour table from Filesystem\r"));
        Debugln(F("   M - Display Month table from Filesystem\r"));
#if defined(_HAS_NO_SLIMMEMETER)
        Debugln(F("  *F - Force build RING files\r"));
#endif
        Debugf ("   I - Identify by blinking LED on GPIO[%02d]\r\n", LED_BUILTIN);
        if (showRaw)
        {
          Debugln(F("   P - Start Parsing again\r"));
        }
        else
        {
          Debugln(F("   P - No Parsing (show RAW data from Smart Meter)\r"));
          showRawCount = 0;
        }
        Debugln(F("  *W - Force Re-Config WiFi\r"));
        Debugln(F("  *R - Reboot\r"));
        Debugln(F("   S - File info on Filesystem\r"));
        Debugln(F("  *U - Update Filesystem (save Data-files)\r"));
        Debugln(F("  *Y - Stop zending heartbeat pulses\r"));
        Debugln(F("  *Z - Zero Counters and sysLog\r\n"));
        if (Verbose1 & Verbose2)  Debugln(F("   V - Toggle Verbose Off\r"));
        else if (Verbose1)        Debugln(F("   V - Toggle Verbose 2\r"));
        else                      Debugln(F("   V - Toggle Verbose 1\r"));

    } // switch()

} // handleKeyInput()


//===========================================================================================
void wait4KeyInput()
{
  char    inChar;

  if (Serial.available() > 0) 
  { 
    inChar = (char)Serial.read();
    while (Serial.available() > 0)
    {
      yield();
      (char)Serial.read();
    }
    while (TelnetStream.available() > 0)
    {
      yield();
      (char)TelnetStream.read();
    }
    handleKeyInput(inChar);
  }
  if (TelnetStream.available() > 0) 
  { 
    inChar = (char)TelnetStream.read();
    while (TelnetStream.available() > 0)
    {
      yield();
      (char)TelnetStream.read();
    }
    while (Serial.available() > 0)
    {
      yield();
      (char)Serial.read();
    }
    handleKeyInput(inChar);
  }
  
} //  wait4KeyInput()


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
