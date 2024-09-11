/*
***************************************************************************
**  Program  : networkStuff, part of DSMRlogger32
**  Version  : v5.n
**
**  Copyright (c) 2020 .. 2023 Willem Aandewiel
**
**  TERMS OF USE: MIT License. See bottom of file.
***************************************************************************
*/
#include "networkStuff.h"
/****
//------ [WiFi@1.2.7 WiFi@2.0.0]
//#include <WiFi.h>                                 		//-- moved to arduinoGlue.h
//------ [WebServer@2.0.0]      
//#include <WebServer.h>                            		//-- moved to arduinoGlue.h
//------ [ESPmDNS@2.0.0]
//#include <ESPmDNS.h>                              		//-- moved to arduinoGlue.h

//#include <WiFiUdp.h>                              		//-- moved to arduinoGlue.h

//------ [HTTPUpdateServer@2.0.0]
#include "HTTPUpdateServer.h"  // part of ESP32 Core https://github.com/ESP32/Arduino

//------ [WiFiManager@2.0.10-beta] ([DNSServer@2.0.0])
//#include <WiFiManager.h>                          		//-- moved to arduinoGlue.h
// included in main program: #include <TelnetStream.h>       // Version 0.0.1 - https://github.com/jandrassy/TelnetStream
//------ [FS@2.0.0]
//#include <FS.h>                                   		//-- moved to arduinoGlue.h


WebServer        httpServer (80);
HTTPUpdateServer httpUpdater(true);
****/

//static      FSInfo SPIFFSinfo;
//bool        filesysMounted;
bool        isConnected = false;

//-- gets called when WiFiManager enters configuration mode
//===========================================================================================
void configModeCallback (WiFiManager *myWiFiManager)
{
  resetWatchdog(); //-- buy some time ..
  neoPixOn(0, neoPixWhite);
  neoPixOff(1);
  
  DebugTln(F("Entered config mode\r"));
  DebugTln(WiFi.softAPIP());
  //if you used auto generated SSID, print it
  DebugTln(myWiFiManager->getConfigPortalSSID());
  if (devSetting->OledType > 0)
  {
    oled_Clear();
    oled_Print_Msg(0, ">>DSMR-logger32<<", 0);
    oled_Print_Msg(1, "AP mode active", 0);
    oled_Print_Msg(2, "Connect to:", 0);
    oled_Print_Msg(3, myWiFiManager->getConfigPortalSSID(), 0);
  }

} // configModeCallback()


//===========================================================================================
void startWiFi(const char *hostname, int timeOut, bool eraseCredentials)
{
  WiFi.mode(WIFI_STA);

  WiFiManager manageWiFi;
  uint32_t lTime = millis();
  String thisAP = String(hostname) + "-" + WiFi.macAddress();

  DebugTf("startWiFi ...[%s]\r\n",  thisAP.c_str());

  if (eraseCredentials) manageWiFi.resetSettings();

  manageWiFi.setDebugOutput(true);

  //--- set callback that gets called when connecting to previous WiFi fails, and enters Access Point mode
  manageWiFi.setAPCallback(configModeCallback);

  //--- sets timeout until configuration portal gets turned off
  //--- useful to make it all retry or go to sleep in seconds
  //manageWiFi.setTimeout(240);  // 4 minuten
  manageWiFi.setTimeout(timeOut);  // in seconden ...

  //--- fetches ssid and pass and tries to connect
  //--- if it does not connect it starts an access point with the specified name
  //--- here  "DSMR-WS-<MAC>"
  //--- and goes into a blocking loop awaiting configuration
  if (!manageWiFi.autoConnect(thisAP.c_str()))
  {
    DebugTln(F("failed to connect and hit timeout"));
    if (devSetting->OledType > 0)
    {
      oled_Clear();
      oled_Print_Msg(0, ">>DSMR-logger32<<", 0);
      oled_Print_Msg(1, "Failed to connect", 0);
      oled_Print_Msg(2, "and hit TimeOut", 0);
      oled_Print_Msg(3, "**** NO WIFI ****", 0);
    }

    DebugTf(" took [%d] milli-seconds ==> ERROR!\r\n", (millis() - lTime));
    neoPixOn(0, neoPixRed);
  }
  else
  {
    DebugTf("Connected with IP-address [%s]\r\n\r\n", WiFi.localIP().toString().c_str());
  }
  if (devSetting->OledType > 0)
  {
    oled_Clear();
  }
  
  strlcpy(myWiFi.SSID,     manageWiFi.getWiFiSSID().c_str(), _MY_SSID_LEN);
  strlcpy(myWiFi.password, manageWiFi.getWiFiPass().c_str(), _MY_PASSWD_LEN);

  if (WiFi.softAPdisconnect(true))
        DebugTln("WiFi Access Point disconnected and closed");
  else  DebugTln("Hm.. could not disconnect WiFi Access Point! (maybe there was none?)\r\n");
  DebugFlush();
  
  DebugTf("startWiFi() took [%d] milli-seconds => OK!\r\n", (millis() - lTime));

  myWiFi.ipGateway = WiFi.gatewayIP();

  neoPixOn(0, neoPixBlue);

  pulseHeart(true);
  
} // startWiFi()


//===========================================================================================
void startTelnet()
{
  TelnetStream.begin();
  Debugln("\r\n");
  DebugTln("Telnet server started ..\r\n");
  TelnetStream.flush();

} // startTelnet()


//=======================================================================
void startMDNS(const char *Hostname)
{
  if (lostWiFiConnection) return;
  
  MDNS.end(); //-- end service
  DebugTf("[1] mDNS setup as [%s.local]\r\n", Hostname);
  if (MDNS.begin(Hostname))               // Start the mDNS responder for Hostname.local
  {
    DebugTf("[2] mDNS responder started as [%s.local]\r\n", Hostname);
    writeToSysLog("mDNS responder started as [%s.local]", Hostname);
  }
  else
  {
    DebugTln("[3] Error setting up MDNS responder!\r\n");
    writeToSysLog("Error setting up MDNS responder!");
  }
  MDNS.addService("http", "tcp", 80);

} // startMDNS()

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