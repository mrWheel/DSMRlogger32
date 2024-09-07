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
#include "menuStuff.h"


//===========================================================================================
void displayHoursHist(bool Telnet)
{
  char dummy[DATA_RECLEN] = {0};
  
  readAllSlots(dummy, RNG_HOURS, HOURS_FILE, lastTlgrmTime);

} // displayHoursHist()


//===========================================================================================
void displayDaysHist(bool Telnet)
{
  char dummy[DATA_RECLEN] = {0};
  
  readAllSlots(dummy, RNG_DAYS, DAYS_FILE, lastTlgrmTime);

} // displayDaysHist()


//===========================================================================================
void displayMonthsHist(bool Telnet)
{
  char dummy[DATA_RECLEN] = {0};

  readAllSlots(dummy, RNG_MONTHS, MONTHS_FILE, lastTlgrmTime);

} // displayMonthsHist()


//===========================================================================================
void displayBoardInfo()
{
  Debugln("\r\n==================================================================\r");
  Debug(" \r\n         Copyright (c) by [Willem Aandewiel");

  Debug("]\r\n                 Hostname [");
  Debug( devSetting->Hostname );
  Debug("]\r\n         Firmware Version [");
  Debug( _FW_VERSION );
  Debug("]\r\n                 Compiled [");
  Debug( __DATE__ );
  Debug( "  " );
  Debug( __TIME__ );
  Debug("]\r\n            compiled with [dsmr2.h]");
#ifdef _SHOW_PASSWRDS
  Debug(" \r\n                 #defines [SHOW_PASSWRDS]");
#endif

  //-- https://github.com/espressif/arduino-esp32/blob/master/cores/esp32/Esp.h#L63
  
  Debug(" \r\n      Telegrams Processed [");
  Debug( telegramCount );
  Debug("]\r\n              With Errors [");
  Debug( telegramErrors );
  Debug("]\r\n                 FreeHeap [");
  Debug( ESP.getFreeHeap() );
  Debug("]\r\n         Minimal FreeHeap [");
  Debug( esp_get_minimum_free_heap_size() );
  Debug("]\r\n               Free PSRAM [");
  Debug( ESP.getFreePsram() );
  Debug("]\r\n                max.Block [");
  Debug( ESP.getMaxAllocHeap() );
  Debug("]\r\n                  Chip ID [");
  Debug( ESP.getEfuseMac() );
  Debug("]\r\n               Chip Model [");
  Debug( ESP.getChipModel() );
  //Debug("]\r\n           Core Version [");
  //Debug( ESP.getSdkVersion() );
  Debug("]\r\n              SDK Version [");
  Debug( ESP.getSdkVersion() );
  Debug("]\r\n           CPU Freq (MHz) [");
  Debug( ESP.getCpuFreqMHz() );
  Debug("]\r\n      Sketch Size (bytes) [");
  Debug( ESP.getSketchSize() );
  Debug("]\r\nFree Sketch Space (bytes) [");
  Debug( ESP.getFreeSketchSpace() );
  Debug("]\r\n       PSRAM size (bytes) [");
  Debug( ESP.getPsramSize() );
  Debug("]\r\n  Flash Chip Size (bytes) [");
  Debug( ESP.getFlashChipSize() );
  //esp32 Debug("]\r\n   Chip Real Size (kB) [");
  //esp32 Debug( ESP.getFlashChipRealSize() / 1024 );
#ifdef _SPIFFS
  Debug("]\r\n         SPIFFS Size (kB) [");
#else
  Debug("]\r\n       LittleFS Size (kB) [");
#endif
  Debug( _FSYS.totalBytes() / 1024 );

  Debug("]\r\n   Flash Chip Speed (MHz) [");
  Debug( ESP.getFlashChipSpeed() / 1000 / 1000 );
  FlashMode_t ideMode = ESP.getFlashChipMode();
  Debug("]\r\n          Flash Chip Mode [");
  Debug( flashMode[ideMode] );

  Debugln("]\r");

  Debugln("==================================================================\r");
  Debug(" \r\n                     SSID [");
  if (runAPmode)  Debug(devSetting->Hostname);
  else            Debug( WiFi.SSID() );
#ifdef _SHOW_PASSWRDS
  Debug("]\r\n                  PSK key [");
  Debug( WiFi.psk() );
#else
  Debug("]\r\n                  PSK key [**********");
#endif
  Debug("]\r\n               IP Address [");
  if (runAPmode)  Debug(WiFi.softAPIP());
  else            Debug( WiFi.localIP() );
  Debug("]\r\n                 Hostname [");
  Debug( devSetting->Hostname );
  //esp32 Debug("]\r\n     Last reset reason [");
  //esp32 Debug( ESP.getResetReason() );
  Debug("]\r\n        number of reboots [");
  Debug( nrReboots );
  Debug("]\r\n                   upTime [");
  Debug( upTime() );
  Debugln("]\r");

  Debugln("==================================================================\r");
  Debug(" \r\n              MQTT broker [");
  Debug( devSetting->MQTTbroker );
  Debug("]\r\n                MQTT User [");
  Debug( devSetting->MQTTuser );
#ifdef _SHOW_PASSWRDS
  Debug("]\r\n            MQTT PassWord [");
  Debug( devSetting->MQTTpasswd );
#else
  Debug("]\r\n            MQTT PassWord [**********");
#endif
  Debug("]\r\n                Top Topic [");
  Debug(devSetting->MQTTtopTopic );
  Debug("]\r\n          Update Interval [");
  Debug(devSetting->MQTTinterval);
  Debugln("]\r");
  Debugln("==================================================================\r\n\r");

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
        
#if defined(_HAS_NO_SLIMMEMETER)
      case 'C':
        forceBuildRingFiles = true;
        runMode = SInit;
        break;
#endif
        
      case 'd':
      case 'D':
        displayDaysHist(true);
        break;
        
      case 'E':
        eraseFile();
        break;

      case 'f': 
      case 'F': writeToSysLog("Force Watchdog reset by user");
                resetWatchdog();
                break;

      case 'h':
      case 'H':
        displayHoursHist(true);
        break;
        
      case 'i':
      case 'I':
        blinkNeoPixels(10, 1000);
        break;
        
      case 'l':
      case 'L':
        listFilesystem();
        break;

      case 'm':
      case 'M':
        displayMonthsHist(true);
        break;
        
      case 'p':
      case 'P':
        showRaw = !showRaw;
        showRawCount = 0;
        break;
        
      case 'R':
        slotErrors      = 0;
        nrReboots       = 0;
        //telegramCount = 0;
        telegramErrors  = 0;
        writeLastStatus();
        DebugT("Reboot in 3 seconds ... \r\n");
        DebugFlush();
        writeToSysLog("User requested ReBoot!");
        delay(3000);
        DebugTln("now Rebooting. \r");
        DebugFlush();
        ESP.restart();
        break;

      case 's':
      case 'S':
        readSmSettings(true);
        readDevSettings(true);
        break;

      case 'U':
        writeDataToRingFiles(lastTlgrmTime);
        break;
        
      case 'v':
      case 'V':
        if (Verbose2)
        {
          Debugln("Verbose is OFF\r");
          Verbose1 = false;
          Verbose2 = false;
        }
        else if (Verbose1)
        {
          Debugln("Verbose Level 2 is ON\r");
          Verbose2 = true;
        }
        else
        {
          Debugln("Verbose Level 1 is ON\r");
          Verbose1 = true;
          Verbose2 = false;
        }
        break;

      case 'W':
        Debugf("\r\nConnect to AP [%s] and go to ip address shown in the AP-name\r\n", devSetting->Hostname);
        delay(1000);
        WiFi.disconnect(true);  // deletes credentials ?!
        writeToSysLog("Erase WiFi credentials! and restart..");
        startWiFi(devSetting->Hostname, 240, true);
        delay(2000);
        ESP.restart();
        delay(2000);
        break;
        
      case 'y': skipHeartbeats = false;
                writeToSysLog("User: Restart sending heart Beats..");
                break;
      case 'Y': skipHeartbeats = !skipHeartbeats;
                if (skipHeartbeats)
                      writeToSysLog("User: Stopped sending heart Beats..");
                else  writeToSysLog("User: Restart sending heart Beats..");
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
        Debugln("\r\nCommands are (*X only uppercase):\r\n");
        Debugln("   B - Board Info\r");
#if defined(_HAS_NO_SLIMMEMETER)
        Debugln("  *C - Create build RING files\r");
#endif
        Debugln("  *E - erase file from Filesystem\r");
        Debugln("   F - force Reboot Watchdog\r");
        Debugln("   I - Identify by blinking NeoPixels\r");
        Debugln("   L - List Filesystem\r");
        if (showRaw)
        {
          Debugln("   P - Start Parsing again\r");
        }
        else
        {
          Debugln("   P - No Parsing (show RAW data from Smart Meter)\r");
          showRawCount = 0;
        }
        Debugln("  *R - Reboot\r");
        Debugln("   S - Show Settings\r");
        Debugln("  *U - Update Filesystem (save Data-files)\r");
        if (Verbose1 & Verbose2)  Debugln("   V - Toggle Verbose Off\r");
        else if (Verbose1)        Debugln("   V - Toggle Verbose 2\r");
        else                      Debugln("   V - Toggle Verbose 1\r");
        Debugln("  *W - Force Re-Config WiFi\r");
        if (skipHeartbeats)
              Debugln("   Y - Start zending heartbeat pulses\r");
        else  Debugln("  *Y - Stop zending heartbeat pulses\r");
        Debugln("  *Z - Zero Counters and sysLog\r\n");
        
        Debugln("   D - Display Day table from Filesystem\r");
        Debugln("   H - Display Hour table from Filesystem\r");
        Debugln("   M - Display Month table from Filesystem\r");
        Debugln();

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