/*
***************************************************************************
**  Sketch uses 1114649 bytes (85%) of program storage space. 
**                                  Maximum is 1310720 bytes.
**  Global variables use 47332 bytes (14%) of dynamic memory, 
**  leaving 280348 bytes for local variables. 
**                                  Maximum is 327680 bytes.
***************************************************************************
**  Program  : DSMRlogger32 (restAPI)
*/
#define _FW_VERSION "v5.0.1 (21-12-2022)"
/*
**  Copyright (c) 2022, 2023 Willem Aandewiel
**
**  TERMS OF USE: MIT License. See bottom of file.
***************************************************************************
**
**  Tested with: ESP32 core 2.0.5 by Espressif Systems
**
**  Arduino-IDE settings for DSMR-logger Revision 5 (ESP32):
**
**    - Board             : "ESP32 Wrover Module" [ESP32-WROVER-E]
**    - Upload Speed      : "230400" (max. with FTDI programmer)
**    // Flash Size       : "4MB (32Mb)"
**    - Flash Mode        : "DIO" ("QIO" is too fast for some chips!!)
**    - Partition Scheme  : "Default 4MB with spiffs (1.2MB APP/1.5MB SPIFFS)"
**    - Core Debug Level  : "None" ??
**    // PSRAM            : "Enabled"
**    // Arduino Runs On  : "Core 1"
**    // Events Run On    : "Core 0"
**
**
**  Coding Style  ( http://astyle.sourceforge.net/astyle.html#_Quick_Start )
**   - Allman style (-A1)
**   - tab 2 spaces (-s2)
**   - Indent 'switch' blocks (-S)
**   - Indent preprocessor blocks (-xW)
**   - Indent multi-line preprocessor definitions ending with a backslash (-w)
**   - Indent C++ comments beginning in column one (-Y)
**   - Insert space padding after commas (-xg)
**   - Attach a pointer or reference operator (-k3)
**
**  use:  astyle -A1 -s2 -S -xW -w -Y -xg- k3 *.{ino|h}
**
**  remove <filename>.orig afterwards
**    
*/

/*
**  You can find more info in the following links (all in Dutch):
**   https://willem.aandewiel.nl/index.php/2020/02/28/restapis-zijn-hip-nieuwe-firmware-voor-de-dsmr-logger/
**   https://mrwheel-docs.gitbook.io/DSMRloggerAPI/
**   https://mrwheel.github.io/DSMRloggerWS/
*/
/******************** compiler options  ********************************************/
//  #define _SHOW_PASSWRDS             // well .. show the PSK key and MQTT password, what else?
//  #define _HAS_NO_SLIMMEMETER        // define for testing only!

//---- one of these
//#define _LITTLEFS
#define _SPIFFS

/******************** don't change anything below this comment **********************/

#include "esp_heap_caps.h"

#include "DSMRlogger32.h"

struct showValues
{
  template<typename Item>
  void apply(Item &i)
  {
    if (i.present())
    {
      DebugT(Item::name);
      Debug(F(": "));
      Debug(i.val());
      Debug(Item::unit());
    }
    Debugln();
  }
};

//-- prototype --
bool isInFieldsArray(const char *lookUp, int elemts);
void addToTable(const char *cName, float fValue);

//=======================================================================
template<typename Item>
Item &typecastValue(Item &i)
{
  return i;
}

//=======================================================================
float typecastValue(TimestampedFixedValue i)
{
  return strToFloat(String(i).c_str(), 3);
}

//=======================================================================
float typecastValue(FixedValue i)
{
  return i;
}

//=======================================================================
struct buildJsonV2ApiSm
{
  bool  skip = false;

  template<typename Item>
  void apply(Item &i)
  {
    skip = false;
    String Name = String(Item::name);
    if (!isInFieldsArray(Name.c_str(), fieldsElements))
    {
      skip = true;
    }
    if (!skip)
    {
      if (i.present())
      {
        addToTable(Name.c_str(), typecastValue(i.val()));
      }
    }
  }

};  // buildJsonV2ApiSm

//=======================================================================
struct addSmToActualStore
{
  bool  skip = false;

  template<typename Item>
  void apply(Item &i)
  {
    skip = false;
    String Name = String(Item::name);
    if (!isInFieldsArray(Name.c_str(), fieldsElements))
    {
      skip = true;
    }
    if (!skip)
    {
      if (i.present())
      {
        pushToActualStore(Name.c_str(), typecastValue(i.val()));
      }
    }
  }

};  // addSmToActualStore


//===========================================================================================
void displayStatus()
{
  if (sysSetting->OledType > 0)
  {
    switch(msgMode)
    {
      case 1:
        snprintf(gMsg, _GMSG_LEN, "Up:%-15.15s", upTime().c_str());
        break;
      case 2:
        snprintf(gMsg, _GMSG_LEN, "WiFi RSSI:%4d dBm", WiFi.RSSI());
        break;
      case 3:
        snprintf(gMsg, _GMSG_LEN, "Heap:%7d Bytes", ESP.getFreeHeap());
        break;
      case 4:
        if (WiFi.status() != WL_CONNECTED)
              snprintf(gMsg, _GMSG_LEN, "**** NO  WIFI ****");
        else  snprintf(gMsg, _GMSG_LEN, "IP %s", WiFi.localIP().toString().c_str());
        break;
      default:
        snprintf(gMsg, _GMSG_LEN, "Telgrms:%6d/%3d", telegramCount, telegramErrors);
        break;
    }

    oled_Print_Msg(3, gMsg, 0);
    //-- modular 5 = number of message displayed (hence it cycles thru the messages
    msgMode= (msgMode+1) % 5;
  }
} // displayStatus()


//===========================================================================================
void setup()
{
  Serial.begin(115200); 
  while(!Serial) { delay(10); }

  upTimeStart = millis() / 1000;
  
  // for now 115200. Look at end of setup()
  SMserial.begin (115200, SERIAL_8N1, SMRX, SMTX);

  Debugln("\n\n*******************************************************");
  Debugf("* Booting....[%s] \r\n", String(_FW_VERSION).c_str());
  Debugln("*******************************************************\r\n");
  DebugFlush();

  heap_caps_malloc_extmem_enable(_PSRAM_LIMIT);

  //-- claim PSRAM 
  setupPsram();
  
  pinMode(LED_BUILTIN,    OUTPUT);
  pinMode(_FLASH_BUTTON,  INPUT);
  pinMode(_PIN_HEARTBEAT, OUTPUT);
  pinMode(_PIN_WD_RESET,  OUTPUT);
  
  //-- Hold WatchDog
  DebugTln("Reset Watchdog ..");
  resetWatchdog();  
  
  neoPixels.begin();
  neoPixels.show();
  neoPixels.setBrightness(125);  

  DebugTln("blink Neo Pixels ..");
  blinkNeoPixels(5, 750);
  neoPixOn(0, neoPixRed);
  neoPixOn(1, neoPixRed);

  pulseHeart(true);
  neoPixOn(0, neoPixRed);
  
  //------ initialize File System --------------------------
  setupFileSystem();

  //------ initialize sysSetting logger --------------------
  setupSysLogger();

  readSysSettings(true);
//  sysSetting->NoHourSlots  = readRingHistoryDepth(HOURS_FILE,  RNG_HOURS);
//  sysSetting->NoDaySlots   = readRingHistoryDepth(DAYS_FILE,   RNG_DAYS);
//  sysSetting->NoMonthSlots = readRingHistoryDepth(MONTHS_FILE, RNG_MONTHS);
//  writeSysSettings(true);

  //------ read status file for last Timestamp --------------------
  strlcpy(lastTlgrmTime.Timestamp, "040302010101X", _TIMESTAMP_LEN);
  //==========================================================//
  // writeLastStatus();  // only for firsttime initialization //
  //==========================================================//
  readLastStatus(); // place it in lastTlgrmTime.Timestamp
  
  DebugTf("last used time [%s]\r\n", lastTlgrmTime.Timestamp);
  
  //-- make sure both prev and last are initialized
  saveTimestamp(lastTlgrmTime.Timestamp);
  saveTimestamp(lastTlgrmTime.Timestamp);
  tzEurope.setTime(  lastTlgrmTime.Hour
                   , lastTlgrmTime.Minute
                   , lastTlgrmTime.Second
                   , lastTlgrmTime.Day
                   , lastTlgrmTime.Month
                   , lastTlgrmTime.Year);
  DebugTf("===> lastTimestamp[%s]-> nrReboots[%u] - Errors[%u]\r\n\n"
                                      , lastTlgrmTime.Timestamp
                                      , nrReboots++
                                      , slotErrors);
  
  readSmSettings(true);
  
  if (sysSetting->OledType > 0)
  {
    oled_Init();
    oled_Clear();  // clear the screen so we can paint the menu.
    oled_Print_Msg(0, "<DSMRlogger32>", 0);
    int8_t sPos = String(_FW_VERSION).indexOf(' ');
    snprintf(gMsg,  _GMSG_LEN, "(c)2022, 2023 [%s]", String(_FW_VERSION).substring(0, sPos).c_str());
    oled_Print_Msg(1, gMsg, 0);
    oled_Print_Msg(2, " Willem Aandewiel", 0);
    oled_Print_Msg(3, " >> Have fun!! <<", 1000);
    yield();
  }
  else     // don't blink if oled-screen attatched
  {
    for(int I=0; I<2; I++)
    {
      digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
      delay(250);
    }
  }
  digitalWrite(LED_BUILTIN, LED_OFF);  // HIGH is OFF

  if (filesysMounted)
  {
    if (sysSetting->OledType > 0)
    {
      oled_Print_Msg(0, "<DSMRlogger32>", 0);
      oled_Print_Msg(3, "Filesystm mounted", 1500);
    }
  }
  else
  {
    if (sysSetting->OledType > 0)
    {
      oled_Print_Msg(0, "<DSMRlogger32>", 0);
      oled_Print_Msg(3, "MOUNT FS FAILED!", 2000);
    }
  }

  while(!psramFound())
  {
    DebugTln("*******************************************");
    DebugTln("**   This module does not have PSRAM!!   **");
    DebugTln("** Firmware will not run on this module! **");
    DebugTln("*******************************************\r\n");
    //-- send heartbeat for 2 minutes
    uint32_t waitTimer = millis();
    while((millis()-waitTimer) < 120000) 
    { 
      pulseHeart();
      delay(10000);
    }
    DebugTln("No more heartbeats from now!\r\n");
    //-- now wait for Watchdog to reset this module
    while(1) { delay(1000); }
  }
  
  snprintf(sysSetting->Hostname, sizeof(sysSetting->Hostname), "%s", _DEFAULT_HOSTNAME);

  //-- Press [Reset] -> "External System"
  //-- Software reset -> "Vbat power on reset"
  lastResetReason(lastReset, _GMSG_LEN, (int)esp_reset_reason());
  DebugTf("Last Reset Reason [%s]\r\n", lastReset);

  oled_Init();
  
  if (sysSetting->DailyReboot)
  {
    ; //if (strcmp(lastReset, "Vbat power on reset") == 0) telegramCount = 0;
  }

  //=============start Networkstuff==================================
  if (sysSetting->OledType > 0)
  {
    if (sysSetting->OledFlip)  oled_Init();  // only if true restart(init) oled screen
    oled_Clear();                       // clear the screen
    oled_Print_Msg(0, "<DSMRlogger32>", 0);
    oled_Print_Msg(1, "Verbinden met WiFi", 500);
  }
  neoPixOn(0, neoPixRed);
  digitalWrite(LED_BUILTIN, LED_ON);
  startWiFi(sysSetting->Hostname, 240, false);  // timeout 4 minuten
  myWiFi.ipWiFi    = WiFi.localIP();
  myWiFi.ipGateway = WiFi.gatewayIP();
  neoPixOn(0, neoPixBlue);
  
  WiFi.onEvent(WiFiEvent);

  //--- setup randomseed the right way (??)
  //--- Read more: http://www.esp32learning.com/code/esp32-true-random-number-generator-example.php
  esp_random();

  if (sysSetting->OledType > 0)
  {
    oled_Print_Msg(0, "<DSMRlogger32>", 0);
    oled_Print_Msg(1, WiFi.SSID(), 0);
    snprintf(gMsg,  _GMSG_LEN, "IP %s", WiFi.localIP().toString().c_str());
    oled_Print_Msg(2, gMsg, 1500);
  }
  
  startTelnet();
  if (sysSetting->OledType > 0)
  {
    oled_Print_Msg(0, "<DSMRlogger32>", 0);
    oled_Print_Msg(3, "telnet (poort 23)", 2500);
  }
  
  digitalWrite(LED_BUILTIN, LED_OFF);

  Debugln();
  DebugT ("Connected to " );
  Debugln (myWiFi.SSID);
  DebugT ("IP address: " );
  Debugln (myWiFi.ipWiFi);
  DebugT ("IP gateway: " );
  Debugln (myWiFi.ipGateway);
  Debugln();

  for (int L=0; L < 4; L++)
  {
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    delay(200);
  }
  digitalWrite(LED_BUILTIN, LED_OFF);

  startMDNS(sysSetting->Hostname);
  if (sysSetting->OledType > 0)
  {
    oled_Print_Msg(3, "mDNS gestart", 1500);
  }

  //=============end Networkstuff======================================

  //================ startNTP =========================================
  if (sysSetting->OledType > 0)
  {
    oled_Print_Msg(3, "setup NTP server", 100);
  }

  DebugT("Wait for NTP sync ...");
  while (!waitForSync(2)) { Debug('.'); delay(500); }
  Debugln("Done!");
  updateNTP();
  DebugT("UTC: ");
  Debugln(UTC.dateTime());
  
  tzEurope.setLocation("Europe/Amsterdam");
  DebugTf("Amsterdam time: ");
  Debugln(tzEurope.dateTime());
  DebugT("Timezone: "); Debugln(getTimezoneName());

  DebugTln(F("NTP server set!\r\n\r"));
  DebugTf("NTP Date/Time: %02d-%02d-%04d %02d:%02d:%02d\r\n", tzEurope.day()
                                             , tzEurope.month()
                                             , tzEurope.year()
                                             , tzEurope.hour()
                                             , tzEurope.minute()
                                             , tzEurope.second());

  if (sysSetting->OledType > 0)
  {
    oled_Print_Msg(0, "<DSMRlogger32>", 0);
    oled_Print_Msg(3, "NTP gestart", 1500);
  }
  //32 prevNtpHour = hour();

  //================ end NTP =========================================

  writeToSysLog(lastReset);                         
  
  Serial.print("\nGebruik 'telnet ");
  Serial.print (WiFi.localIP());
  Serial.println("' voor verdere debugging\r\n");

  //=============now test if FS is correct populated!============
  filesysNotPopulated = setupIsFsPopulated();

  if (sysSetting->OledType > 0)
  {
    snprintf(gMsg,  _GMSG_LEN, "DT: %02d%02d%02d%02d0101x", thisYear
                                            , thisMonth, thisDay, thisHour);
    oled_Print_Msg(0, "<DSMRlogger32>", 0);
    oled_Print_Msg(3, gMsg, 1500);
  }

  //================ Start MQTT  ======================================

  connectMQTT();
  if (sysSetting->OledType > 0)
  {
    oled_Print_Msg(0, "<DSMRlogger32>", 0);
    oled_Print_Msg(3, "MQTT server set!", 1500);
  }

  //================ Start HTTP Server ================================

  if (!filesysNotPopulated)
  {
    DebugTln(F("Filesystem correct populated -> normal operation!\r"));
    if (sysSetting->OledType > 0)
    {
      oled_Print_Msg(0, "<DSMRlogger32>", 0);
      oled_Print_Msg(1, "OK, FS correct", 0);
      oled_Print_Msg(2, "Verder met normale", 0);
      oled_Print_Msg(3, "Verwerking ;-)", 2500);
    }
    if (hasAlternativeIndex)
    {
      httpServer.serveStatic("/",                 _FSYS, sysSetting->IndexPage);
      httpServer.serveStatic("/index",            _FSYS, sysSetting->IndexPage);
      httpServer.serveStatic("/index.html",       _FSYS, sysSetting->IndexPage);
      httpServer.serveStatic("/DSMRindex.html",   _FSYS, sysSetting->IndexPage);
    }
    else
    {
      httpServer.serveStatic("/",                 _FSYS, "/DSMRindex.html");
      httpServer.serveStatic("/DSMRindex.html",   _FSYS, "/DSMRindex.html");
      httpServer.serveStatic("/index",            _FSYS, "/DSMRindex.html");
      httpServer.serveStatic("/index.html",       _FSYS, "/DSMRindex.html");
      httpServer.serveStatic("/DSMRindex.css",    _FSYS, "/DSMRindex.css");
      httpServer.serveStatic("/DSMRindex.js",     _FSYS, "/DSMRindex.js");
      httpServer.serveStatic("/DSMRgraphics.js",  _FSYS, "/DSMRgraphics.js");
    }
  }
  else
  {
    DebugTln(F("Oeps! not all files found on FS -> Start FSmanager!\r"));
    filesysNotPopulated = true;
    if (sysSetting->OledType > 0)
    {
      oled_Print_Msg(0, "!OEPS! niet alle", 0);
      oled_Print_Msg(1, "files op FS", 0);
      oled_Print_Msg(2, "gevonden! (fout!)", 0);
      oled_Print_Msg(3, "Start FSmanager", 2000);
    }
  }

  setupFSmanager();

  httpServer.on("/api", HTTP_GET, processAPI);
  // all other api calls are catched in FSmanager onNotFounD!

  httpServer.begin();
  DebugTln( "HTTP server gestart\r" );
  if (sysSetting->OledType > 0)                                  //HAS_OLED
  {
    //HAS_OLED
    oled_Clear();                                           //HAS_OLED
    oled_Print_Msg(0, "<DSMRlogger32>", 0);              //HAS_OLED
    oled_Print_Msg(2, "HTTP server ..", 0);                 //HAS_OLED
    oled_Print_Msg(3, "gestart (poort 80)", 0);             //HAS_OLED
  }                  

  httpUpdater.setup(&httpServer);

  //================ Start HTTP Server ================================

  DebugTf("Startup complete! lastTlgrmTime[%s]\r\n", lastTlgrmTime.Timestamp);
  writeToSysLog("Startup complete! lastTlgrmTime[%s]", lastTlgrmTime.Timestamp);

  if (sysSetting->OledType > 0)
  {
    oled_Print_Msg(0, "<DSMRlogger32>", 0);
    oled_Print_Msg(1, "Startup complete", 0);
    oled_Print_Msg(2, "Wait for first", 0);
    oled_Print_Msg(3, "telegram .....", 500);
  }

  //================ Start Slimme Meter ===============================

  DebugTln(F("Enable slimmeMeter..\r"));

#if !defined( _HAS_NO_SLIMMEMETER )
  DebugTln("Setup serial port for Smart Meter reading");
  if (setting->PreDSMR40 == 0)
  {
    DebugTf("SMserial is set to 115200 baud / 8N1 (RX=%d, TX=%d)\r\n", SMRX, SMTX);
    DebugFlush();
    SMserial.end();
    delay(100);
    SMserial.begin (115200, SERIAL_8N1, SMRX, SMTX);
    slimmeMeter.doChecksum(true);
  }
  else
  {
    //PRE40
    DebugTf("SMserial is set to 9600 baud / 7E1 (RX=%d, TX=%d)\r\n", SMRX, SMTX);
    DebugFlush();
    SMserial.end();
    delay(100);
    SMserial.begin(9600, SERIAL_7E1, SMRX, SMTX);
    slimmeMeter.doChecksum(false);
  }

#endif // HAS_NO_SLIMME_METER

  neoPixOn(1, neoPixGreenLow);
  
  updatedRINGfiles = false;
  
  delay(100);
  slimmeMeter.enable(true);

} // setup()


//===[ no-blocking delay with running background tasks in ms ]============================
DECLARE_TIMER_MS(timer_delay_ms, 1);
void delayms(unsigned long delay_ms)
{
  CHANGE_INTERVAL_MS(timer_delay_ms, delay_ms);
  RESTART_TIMER(timer_delay_ms);
  while (!DUE(timer_delay_ms))
  {
    doSystemTasks();
  }

} // delayms()


//==[ Do Telegram Processing ]===============================================================
void doTaskTelegram()
{
  if (DUE(nextTelegram))
  {
    if (Verbose1) 
      DebugTln("doTaskTelegram");
#if defined(_HAS_NO_SLIMMEMETER)
    handleTestdata();
#else
    //-- enable DTR to read a telegram from the Slimme Meter
    slimmeMeter.enable(true);
    slimmeMeter.loop();
    handleSlimmemeter();
#endif
  }
  
} //  doTaskTelegram()


//===[ Do System tasks ]=============================================================
void doSystemTasks()
{
  pulseHeart();
  
#ifndef _HAS_NO_SLIMMEMETER
  slimmeMeter.loop();
#endif
  MQTTclient.loop();
  httpServer.handleClient();

  wait4KeyInput();
  if (sysSetting->OledType > 0)
  {
    checkFlashButton();
  }

  if ((millis() - glowTimer1) > _GLOW_TIME)
  {
    neoPixOn(1, neoPixGreenLow);
  }

  events(); //-- update ezTime every 30? minutes
  yield();

} // doSystemTasks()


//========================================================================================
void loop ()
{
  //--- do the tasks that has to be done
  //--- as often as possible
  doSystemTasks();
  doTaskTelegram();

  loopCount++;

//  //--- update upTime counter
//  if (DUE(updateSeconds))
//  {
//    upTimeSeconds++;
//  }

  //--- if an OLED screen attached, display the status
  if (sysSetting->OledType > 0)
  {
    if (DUE(updateDisplay))
    {
      displayStatus();
    }
  }

  //--- if connection lost, try to reconnect to WiFi
  if ( lostWiFiConnection )
  {
    neoPixOn(0, neoPixRed);
    if (firstConnectionLost)
    {
      writeToSysLog("Reset Watchdog ...");
      firstConnectionLost = false;
      resetWatchdog();
    }

    glowTimer0 = millis() + 2000;
    if ((lostWiFiCount % 10) == 0)
    {
      DebugTf("Reconnect wifi with [%s]...\r\n", sysSetting->Hostname);
      writeToSysLog("Reconnect wifi with [%s]...", sysSetting->Hostname);
    }
    WiFi.disconnect();
    //WiFi.reconnect();
    delay(1000);
    WiFi.begin(myWiFi.SSID, myWiFi.password);
    delay(1000);
    if (WiFi.status() != WL_CONNECTED)
    {
      if ((lostWiFiCount % 10) == 0)
      {
        DebugTln("Wifi still not connected!");
        writeToSysLog("Wifi still not connected!");
      }
    }
    else
    {
      snprintf(gMsg,  _GMSG_LEN, "IP:[%s], Gateway:[%s]"
                                      , WiFi.localIP().toString().c_str()
                                      , WiFi.gatewayIP().toString().c_str());
      DebugTf("%s", gMsg);
      writeToSysLog("%s", gMsg);
    }
  } //  lostWiFiConnection
  else
  {
    //-- set NEO 0 to Green
    if ((millis() - glowTimer0) > _GLOW_TIME)
    {
      neoPixOn(0, neoPixGreenLow);
    }
  }
  
#ifndef _HAS_NO_SLIMMEMETER
  //-- hier moet nog even over worden nagedacht
  //-- via een setting in- of uit-schakelen
  if (sysSetting->DailyReboot && (hour() == 4) && (minute() == 5))
  {
    slotErrors      = 0;
    nrReboots       = 0;
    writeLastStatus();
    //--  skip to next minute (6)
    writeToSysLog("Daily Reboot! after 60 seconds");
    delay(60000);
    ESP.restart();
    delay(3000);
  }
#endif

  yield();

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
***************************************************************************/
