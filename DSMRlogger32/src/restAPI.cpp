/*
***************************************************************************
**  Program  : restAPI, part of DSMRlogger32
**  Version  : v5.n
**
**  Copyright (c) 2020 .. 2024 Willem Aandewiel
**
**  TERMS OF USE: MIT License. See bottom of file.
***************************************************************************
*/
#include "restAPI.h"


//=======================================================================
void processAPI()
{
  char fName[40] = "";
  char URI[50]   = "";
  String words[10];

  strlcpy( URI, httpServer.uri().c_str(), sizeof(URI) );

  if (httpServer.method() == HTTP_GET)
        DebugTf("from[%s] URI[%s] method[GET] \r\n"
                  , httpServer.client().remoteIP().toString().c_str()
                  , URI);
  else  DebugTf("from[%s] URI[%s] method[PUT] \r\n"
                  , httpServer.client().remoteIP().toString().c_str()
                  , URI);

  if (ESP.getFreeHeap() < 8500) // to prevent firmware from crashing!
  {
    DebugTf("==> Bailout due to low heap (%d bytes))\r\n", ESP.getFreeHeap() );
    writeToSysLog("from[%s][%s] Bailout low heap (%d bytes)"
                        , httpServer.client().remoteIP().toString().c_str()
                        , URI
                        , ESP.getFreeHeap() );
    httpServer.send(500, "text/plain", "500: internal server error (low heap)\r\n");
    return;
  }

  int8_t wc = splitString(URI, '/', words, 10);

  if (Verbose2)
  {
    DebugT(">>");
    for (int w=0; w<wc; w++)
    {
      Debugf("word[%d] => [%s], ", w, words[w].c_str());
    }
    Debugln(" ");
  }

  if (words[1] != "api" || words[2] != "v2")
  {
    //-- I will only serve v2 version
    sendApiNotFound(URI);
    return;
  }

  if (words[3] == "sm")
  {
    processApiV2Sm(words[4].c_str(), words[5].c_str());
    return;
  } //-- api/v2/sm ...

  //-- api/v2/dev ..
  if (words[3] == "dev")
  {
    processApiV2Dev(URI, words[4].c_str(), words[5].c_str(), words[6].c_str());
    return;
  }

  //-- api/v2/shield ..
  if (words[3] == "shield")
  {
    processApiV2Shield(URI, words[4].c_str(), words[5].c_str(), words[6].c_str());
    return;
  }

  //-- api/v2/hist ..
  if (words[3] == "hist")
  {
    processApiV2Hist(URI, words[4].c_str(), words[5].c_str(), words[6].c_str());
    return;
  }
  //-- unknown api call
  sendApiNotFound(URI);

} // processAPI()


//====================================================
void processApiV2Sm(const char* apiId, const char* oneField)
{
  if (strcmp(apiId, "actual") == 0)
  {
    //--- new api. 
    onlyIfPresent = true;
    copyToFieldsArray(actualArray, actualElements);
    sendJsonV2smApi(apiId);
    return;
  }
  //-- api/v2/sm
  if (strcmp(apiId, "fields") == 0)
  {
    //--- new api. 
    onlyIfPresent = false;
    memset(fieldsArray, 0, sizeof(fieldsArray));
    if (strlen(oneField) > 1)
    {
      DebugTf("apiID[%s], oneField[%s]\r\n", apiId, oneField);
      strlcpy(fieldsArray[0], "timestamp", 34);
      strlcpy(fieldsArray[1], oneField, 34);
      fieldsElements = 2;
    }
    sendJsonV2smApi(apiId);
    return;
  }
  if (strcmp(apiId, "info") == 0)
  {
    onlyIfPresent = true;
    copyToFieldsArray(infoArray, infoElements);
    sendJsonV2smApi(apiId);
    return;
  }

  if (strcmp(apiId, "settings") == 0)
  {
    //--- new api. 
    if (httpServer.method() == HTTP_PUT || httpServer.method() == HTTP_POST)
    {
      //------------------------------------------------------------
      // json string: {"name":"Interval","value":9}
      // json string: {"name":"TelegramInterval","value":123.45}
      // json string: {"name":"mBus1Type","value":"3"}
      //------------------------------------------------------------
      DebugTln(httpServer.arg(0));
      //-- Allocate the JsonDocument
      SpiRamJsonDocument  doc(3000);
      DeserializationError err = deserializeJson(doc, httpServer.arg(0).c_str());
      serializeJson(doc, jsonBuff, _JSONBUFF_LEN);
      //-dbg-Debugln(jsonBuff);
      char field[30]     = {0};
      char newValue[101] = {0};
      strlcpy(field,    doc["name"]  | "UNKNOWN",  sizeof(field));
      strlcpy(newValue, doc["value"] | "0",        sizeof(newValue));
      updateSmSettings(field, newValue);
      httpServer.send(200, "application/json", httpServer.arg(0));
      writeToSysLog("DSMReditor: Slimme Meter Field[%s] changed to [%s]", field, newValue);
    }
    else
    {
      sendSMsettings();
    }
    return;
  }

  if (strcmp(apiId, "telegram") == 0)
  {
    int16_t thisCRC=CRC16(0x0000, (unsigned char *) tlgrmRaw, strlen(tlgrmRaw));
    char crcChar[10] = {};
    snprintf(crcChar, sizeof(crcChar), "!%04x\r\n", (0xffff & thisCRC));
    strlcat(tlgrmRaw, crcChar, _TLGRM_LEN);
    if (Verbose1) 
      DebugTf("Telegram (%d chars):\r\n%s", strlen(tlgrmRaw), tlgrmRaw);
    httpServer.send(200, "application/plain", tlgrmRaw);
    return;
  }

} //  processApiV2Sm()


//====================================================
void processApiV2Dev(const char *URI, const char *apiId, const char *word5, const char *word6)
{
  DebugTf("apiId[%s], word5[%s], word6[%s]\r\n", apiId, word5, word6);
  if (strcmp(apiId, "info") == 0)
  {
    sendDeviceInfo();
    return;
  }
  
  if (strcmp(apiId, "time") == 0)
  {
    sendDeviceTime();
    return;
  }
  
  if (strcmp(apiId, "syslog") == 0)
  {
    char lLine[_SYSLOG_LINE_LEN] = {};

    DebugTln("Start processing sysLog...");
    sysLog.setOutput(&Serial, 115200);
    memset(jsonBuff, 0, _JSONBUFF_LEN);
    sysLog.startReading();
    while( sysLog.readPreviousLine(lLine, sizeof(lLine)) ) 
    {
      strlcat(jsonBuff, lLine, _JSONBUFF_LEN);
      strlcat(jsonBuff, "\r\n", _JSONBUFF_LEN);
    }
    httpServer.send(200, "application/plain", jsonBuff);
    return;
  }
  if (strcmp(apiId, "erase") == 0)
  {
    char    lLine[_SYSLOG_LINE_LEN] = {};

    DebugTln("request Erase sysLog and statistics ...");
    sysLog.begin(_SYSLOG_LINES, _SYSLOG_LINE_LEN, true);
    writeToSysLog("Erase sysLog and statistics by user");
    strlcpy(jsonBuff, "{\"status\":\"OK\"}", _JSONBUFF_LEN);
    httpServer.send(200, "application/plain", jsonBuff);
    slotErrors      = 0;
    nrReboots       = 0;
    telegramCount   = 0;
    telegramErrors  = 0;
    writeLastStatus();
    return;
  }

  if (strcmp(apiId, "relays") == 0)
  {
    DebugTln("Handle /api/v2/dev/relays..");
    if (httpServer.method() == HTTP_PUT || httpServer.method() == HTTP_POST)
    {
      SpiRamJsonDocument  doc(2000);
      DeserializationError err = deserializeJson(doc, httpServer.arg(0).c_str());
      serializeJson(doc, jsonBuff, _JSONBUFF_LEN);
      if (doc.containsKey("relay_state0")) 
      {
        relay0.setRelayState(doc["relay_state0"].as<int>());
      }
      if (doc.containsKey("relay_state1")) 
      {
        relay1.setRelayState(doc["relay_state1"].as<int>());
      }
      httpServer.send(200, "application/json", httpServer.arg(0));
      return;
    }
  }

  if (strcmp(apiId, "settings") == 0)
  {
    DebugTln("Handle /api/v2/dev/settings..");
    if (httpServer.method() == HTTP_PUT || httpServer.method() == HTTP_POST)
    {
      //------------------------------------------------------------
      // json string: {"name":"mqtt_broker","value":"192.168.1.2"}
      // json string: {"name":"mqtt_interval","value":12}
      // json string: {"name":"hostname","value":"abc"}
      //------------------------------------------------------------
      DebugTln(httpServer.arg(0));
      //-- Allocate the JsonDocument
      SpiRamJsonDocument  doc(3000);
      DeserializationError err = deserializeJson(doc, httpServer.arg(0).c_str());
      serializeJson(doc, jsonBuff, _JSONBUFF_LEN);
      //-dbg-Debugln(jsonBuff);
      char field[30]     = {0};
      char newValue[101] = {0};
      strlcpy(field,    doc["name"]  | "UNKNOWN",  sizeof(field));
      strlcpy(newValue, doc["value"] | "0",        sizeof(newValue));
      updateDevSettings(field, newValue);
      writeToSysLog("DSMReditor: System Field[%s] changed to [%s]", field, newValue);
      memset(field,    0, sizeof(field));
      memset(newValue, 0, sizeof(newValue));
      httpServer.send(200, "application/json", httpServer.arg(0));
    }
    else
    {
      sendDevSettings();
    }
    return;
  }

  if (strcmp(apiId, "shield") == 0)
  {
    DebugTln("Handle /api/v2/dev/shield..");
    if (httpServer.method() == HTTP_PUT || httpServer.method() == HTTP_POST)
    {
      //------------------------------------------------------------
      // json string: {"name":"mqtt_broker","value":"192.168.1.2"}
      // json string: {"name":"mqtt_interval","value":12}
      // json string: {"name":"hostname","value":"abc"}
      //------------------------------------------------------------
      DebugTln(httpServer.arg(0));
      //-- Allocate the JsonDocument
      SpiRamJsonDocument  doc(3000);
      DeserializationError err = deserializeJson(doc, httpServer.arg(0).c_str());
      serializeJson(doc, jsonBuff, _JSONBUFF_LEN);
      DebugTln(jsonBuff);
      char field[30]     = {0};
      char newValue[101] = {0};
      //-- convert HH:MM to minutes
      String fieldName = doc["name"];
      String fieldValue = {};
      if ((fieldName.substring(0, 16) == "shld_activeStart") || (fieldName.substring(0, 15) == "shld_activeStop"))
      {
        DebugTf("... Found [%s]\r\n", fieldName.c_str());
        fieldValue = doc["value"].as<String>();
        // Split the string into hours and minutes
        int separatorIndex = fieldValue.indexOf(':');
        uint8_t hours = fieldValue.substring(0, separatorIndex).toInt();    // Extract hours part
        uint8_t minutes = fieldValue.substring(separatorIndex + 1).toInt(); // Extract minutes part
        // Convert HH:MM back to minutes
        uint16_t tmpValue = (hours * 60) + minutes;
        doc["value"] = String(tmpValue);
        DebugTf("[%s]: set to newValue[%d/%s]\r\n", fieldName, tmpValue, doc["value"].as<String>().c_str());
      }
      if (fieldName.substring(0, 12) == "shld_GPIOpin") 
      {
        DebugTf("... Found [%s]\r\n", fieldName.c_str());
        int8_t fieldInt8 = doc["value"].as<int>();
        if (fieldInt8 < 0)  fieldInt8 = -1;
        else if (fieldInt8 > 13) fieldInt8 = 14;
        else fieldInt8 = 13;
        doc["value"] = String(fieldInt8);
        DebugTf("[%s]: set to newValue[%d/%s]\r\n", fieldName, fieldInt8, doc["value"].as<String>().c_str());
      }
      strlcpy(field,    doc["name"]  | "UNKNOWN",  sizeof(field));
      strlcpy(newValue, doc["value"] | "0",        sizeof(newValue));
      updateShieldSettings(field, newValue);
      DebugTf("DSMReditor: Shield Field[%s] changed to [%s]\r\n", field, newValue);
      writeToSysLog("DSMReditor: Shield Field[%s] changed to [%s]", field, newValue);
      memset(field,    0, sizeof(field));
      memset(newValue, 0, sizeof(newValue));
      httpServer.send(200, "application/json", httpServer.arg(0));
    }
    else
    {
      sendShieldSettings();
    }
    return;
  }
  
  if (strcmp(apiId, "debug") == 0)
  {
    sendDeviceDebug(URI, word5);
    return;
  }
  
  sendApiNotFound(URI);

} // processApiV2Dev()


//====================================================
void processApiV2Shield(const char *URI, const char *apiId, const char *word5, const char *word6)
{
  DebugTf("apiId[%s], word5[%s], word6[%s]\r\n", apiId, word5, word6);
  if (strcmp(apiId, "info") == 0)
  {
    sendShieldInfo();
    return;
  }
  
  if (strcmp(apiId, "relays") == 0)
  {
    DebugTln("Handle /api/v2/shield/relays..");
    if (httpServer.method() == HTTP_PUT || httpServer.method() == HTTP_POST)
    {
      SpiRamJsonDocument  doc(2000);
      DeserializationError err = deserializeJson(doc, httpServer.arg(0).c_str());
      serializeJson(doc, jsonBuff, _JSONBUFF_LEN);
      if (doc.containsKey("relay_state0")) 
      {
        relay0.setRelayState(doc["relay_state0"].as<int>());
      }
      if (doc.containsKey("relay_state1")) 
      {
        relay1.setRelayState(doc["relay_state1"].as<int>());
      }
      httpServer.send(200, "application/json", httpServer.arg(0));
      return;
    }
  }

  if (strcmp(apiId, "settings") == 0)
  {
    DebugTln("Handle /api/v2/shield/settings..");
    if (httpServer.method() == HTTP_PUT || httpServer.method() == HTTP_POST)
    {
      DebugTln(httpServer.arg(0));
      //-- Allocate the JsonDocument
      SpiRamJsonDocument  doc(3000);
      DeserializationError err = deserializeJson(doc, httpServer.arg(0).c_str());
      serializeJson(doc, jsonBuff, _JSONBUFF_LEN);
      DebugTln(jsonBuff);
      char field[30]     = {0};
      char newValue[101] = {0};
      //-- convert HH:MM to minutes
      String fieldName = doc["name"];
      String fieldValue = {};
      if ((fieldName.substring(0, 16) == "shld_activeStart") || (fieldName.substring(0, 15) == "shld_activeStop"))
      {
        DebugTf("... Found [%s]\r\n", fieldName.c_str());
        fieldValue = doc["value"].as<String>();
        // Split the string into hours and minutes
        int separatorIndex = fieldValue.indexOf(':');
        uint8_t hours = fieldValue.substring(0, separatorIndex).toInt();    // Extract hours part
        uint8_t minutes = fieldValue.substring(separatorIndex + 1).toInt(); // Extract minutes part
        // Convert HH:MM back to minutes
        uint16_t tmpValue = (hours * 60) + minutes;
        doc["value"] = String(tmpValue);
        DebugTf("[%s]: set to newValue[%d/%s]\r\n", fieldName, tmpValue, doc["value"].as<String>().c_str());
      }
      if (fieldName.substring(0, 12) == "shld_GPIOpin") 
      {
        DebugTf("... Found [%s]\r\n", fieldName.c_str());
        int8_t fieldInt8 = doc["value"].as<int>();
        if (fieldInt8 < 0)  fieldInt8 = -1;
        else if (fieldInt8 > 13) fieldInt8 = 14;
        else fieldInt8 = 13;
        doc["value"] = String(fieldInt8);
        DebugTf("[%s]: set to newValue[%d/%s]\r\n", fieldName, fieldInt8, doc["value"].as<String>().c_str());
      }
      strlcpy(field,    doc["name"]  | "UNKNOWN",  sizeof(field));
      strlcpy(newValue, doc["value"] | "0",        sizeof(newValue));
      updateShieldSettings(field, newValue);
      DebugTf("DSMReditor: Shield Field[%s] changed to [%s]\r\n", field, newValue);
      writeToSysLog("DSMReditor: Shield Field[%s] changed to [%s]", field, newValue);
      memset(field,    0, sizeof(field));
      memset(newValue, 0, sizeof(newValue));
      httpServer.send(200, "application/json", httpServer.arg(0));
    }
    else
    {
      sendShieldSettings();
    }
    return;
  }
/*****
  if (strcmp(apiId, "shield") == 0)
  {
    DebugTln("Handle /api/v2/dev/shield..");
    if (httpServer.method() == HTTP_PUT || httpServer.method() == HTTP_POST)
    {
      //------------------------------------------------------------
      // json string: {"name":"mqtt_broker","value":"192.168.1.2"}
      // json string: {"name":"mqtt_interval","value":12}
      // json string: {"name":"hostname","value":"abc"}
      //------------------------------------------------------------
      DebugTln(httpServer.arg(0));
      //-- Allocate the JsonDocument
      SpiRamJsonDocument  doc(3000);
      DeserializationError err = deserializeJson(doc, httpServer.arg(0).c_str());
      serializeJson(doc, jsonBuff, _JSONBUFF_LEN);
      DebugTln(jsonBuff);
      char field[30]     = {0};
      char newValue[101] = {0};
      //-- convert HH:MM to minutes
      String fieldName = doc["name"];
      String fieldValue = {};
      if ((fieldName.substring(0, 16) == "shld_activeStart") || (fieldName.substring(0, 15) == "shld_activeStop"))
      {
        DebugTf("... Found [%s]\r\n", fieldName.c_str());
        fieldValue = doc["value"].as<String>();
        // Split the string into hours and minutes
        int separatorIndex = fieldValue.indexOf(':');
        uint8_t hours = fieldValue.substring(0, separatorIndex).toInt();    // Extract hours part
        uint8_t minutes = fieldValue.substring(separatorIndex + 1).toInt(); // Extract minutes part
        // Convert HH:MM back to minutes
        uint16_t tmpValue = (hours * 60) + minutes;
        doc["value"] = String(tmpValue);
        DebugTf("[%s]: set to newValue[%d/%s]\r\n", fieldName, tmpValue, doc["value"].as<String>().c_str());
      }
      if (fieldName.substring(0, 12) == "shld_GPIOpin") 
      {
        DebugTf("... Found [%s]\r\n", fieldName.c_str());
        int8_t fieldInt8 = doc["value"].as<int>();
        if (fieldInt8 < 0)  fieldInt8 = -1;
        else if (fieldInt8 > 13) fieldInt8 = 14;
        else fieldInt8 = 13;
        doc["value"] = String(fieldInt8);
        DebugTf("[%s]: set to newValue[%d/%s]\r\n", fieldName, fieldInt8, doc["value"].as<String>().c_str());
      }
      strlcpy(field,    doc["name"]  | "UNKNOWN",  sizeof(field));
      strlcpy(newValue, doc["value"] | "0",        sizeof(newValue));
      updateShieldSettings(field, newValue);
      DebugTf("DSMReditor: Shield Field[%s] changed to [%s]\r\n", field, newValue);
      writeToSysLog("DSMReditor: Shield Field[%s] changed to [%s]", field, newValue);
      memset(field,    0, sizeof(field));
      memset(newValue, 0, sizeof(newValue));
      httpServer.send(200, "application/json", httpServer.arg(0));
    }
    else
    {
      sendShieldSettings();
    }
    return;
  }
  *****/

  if (strcmp(apiId, "debug") == 0)
  {
    sendDeviceDebug(URI, word5);
    return;
  }
  
  sendApiNotFound(URI);

} // processApiV2Shield()

//====================================================
//-- /api/v2/hist/            - list all entries desc
//-- /api/v2/hist/<limit>     - list <limit> enties desc
//-- /api/v2/hist/{desc|asc}  - list all entries {desc|asc}
//-- /api/v2/hist/<limit>/{desc|asc}
//----------------------------------------------------
void processApiV2Hist(const char *URI, const char *apiId, const char *word5, const char *word6)
{
  int8_t    ringType     = 0;
  char      fileName[20] = "";
  uint16_t  recSlot;
  uint8_t   limit = 0;
  bool      sorting = true; //-- true is desc., false is asc
  
  if ((strcmp(word5, "desc") == 0) || (strcmp(word5, "asc") == 0))
  {
    if (strcmp(word5, "desc") == 0) 
          sorting = true; 
    else  sorting = false;
    limit = 0;
  }
  else if ((strcmp(word6, "desc") == 0) || (strcmp(word6, "asc") == 0))
  {
    if (strcmp(word6, "desc") == 0) 
          sorting = true; 
    else  sorting = false;
    limit = atoi(word5);
  }
  else
  {
    limit   = atoi(word5);
    sorting = true; //-- default descending
  }

  if (strcmp(apiId, "actual") == 0 )
  {
    sendJsonActualHist();
    return;
  }
  else if (strcmp(apiId, "hours") == 0 )
  {
    ringType = RNG_HOURS;
    strlcpy(fileName, HOURS_FILE, sizeof(fileName));
  }
  else if (strcmp(apiId, "days") == 0 )
  {
    ringType = RNG_DAYS;
    strlcpy(fileName, DAYS_FILE, sizeof(fileName));
  }
  else if (strcmp(apiId, "months") == 0)
  {
    ringType = RNG_MONTHS;
    if (httpServer.method() == HTTP_PUT || httpServer.method() == HTTP_POST)
    {
      //------------------------------------------------------------
      // json string: {"recid":"29013023"
      //               ,"edt1":2601.146,"edt2":"9535.555"
      //               ,"ert1":378.074,"ert2":208.746
      //               ,"gdt":3314.404}
      //------------------------------------------------------------
      char      record[DATA_RECLEN + 1] = "";

      String jsonIn  = httpServer.arg(0).c_str();
      DebugTln(jsonIn);

      recSlot = buildDataRecordFromJson(record, DATA_RECLEN, jsonIn);
      timeStruct updTime = {0};
      updTime.monthSlot;
      //--- update MONTHS
      writeDataToRingFile(MONTHS_FILE, ringType, record, updTime);
      //--- send OK response --
      httpServer.send(200, "application/json", httpServer.arg(0));

      return;
    }
    else
    {
      strlcpy(fileName, MONTHS_FILE, sizeof(fileName));
    }
  }
  else
  {
    sendApiNotFound(URI);
    return;
  }

  DebugTf("apiId[%s], limit[%d], Sort[%s]\r\n", apiId, limit, (sorting ? "desc" : "asc"));

  sendJsonHist(ringType, fileName, lastTlgrmTime, limit, sorting);
  
} // processApiV2Hist()


//=======================================================================
void sendDeviceInfo()
{
  char compileOptions[200] = "";
  char theTime[20] = {0};

  DebugTln("/api/v2/dev/info");
  snprintf(theTime, sizeof(theTime), "%02d-%02d-%04d %02d:%02d:%02d"
                                                    , localtime(&now)->tm_mday
                                                    , localtime(&now)->tm_mon 
                                                    , localtime(&now)->tm_year + 1900
                                                    , localtime(&now)->tm_hour
                                                    , localtime(&now)->tm_min
                                                    , localtime(&now)->tm_sec
              );

//  char gMsg[_GMSG_LEN] = {};
  memset(jsonBuff, 0, _JSONBUFF_LEN);
  
  //-- Allocate the JsonDocument
  SpiRamJsonDocument  doc(3000);

#ifdef _SHOW_PASSWRDS
  strlcat(compileOptions, "[SHOW_PASSWRDS]", sizeof(compileOptions));
#endif

  doc["devinfo"];

  doc["devinfo"]["author"] = "Willem Aandewiel (www.aandewiel.nl)";
  doc["devinfo"]["fwversion"] = _FW_VERSION;

  snprintf(gMsg,  _GMSG_LEN, "%s %s", __DATE__, __TIME__);
  doc["devinfo"]["compiled"] = gMsg;

  doc["devinfo"]["hostname"]        = devSetting->Hostname;
  if (runAPmode)  doc["devinfo"]["ipaddress"] = WiFi.softAPIP().toString();
  else            doc["devinfo"]["ipaddress"] = WiFi.localIP().toString();
  doc["devinfo"]["macaddress"]      = String(WiFi.macAddress());
  doc["devinfo"]["indexfile"]       = devSetting->IndexPage;
  doc["devinfo"]["free_heap"]       = ESP.getFreeHeap();
  doc["devinfo"]["min_free_heap"]   = esp_get_minimum_free_heap_size();
  doc["devinfo"]["psram_size"]      = ESP.getPsramSize();
  doc["devinfo"]["free_psram_size"] = ESP.getFreePsram();
  doc["devinfo"]["used_psram_size"] = ESP.getPsramSize() - ESP.getFreePsram();
  doc["devinfo"]["chip_model"]      = String( ESP.getChipModel() );
  doc["devinfo"]["sdk_version"]     = String( ESP.getSdkVersion() );
  doc["devinfo"]["cpu_freq"]        = ESP.getCpuFreqMHz();
  doc["devinfo"]["sketch_size"]     = ESP.getSketchSize();
  doc["devinfo"]["free_sketch_space"] = ESP.getFreeSketchSpace();
  doc["devinfo"]["filedevSetting_size"] = _FSYS.totalBytes();
  doc["devinfo"]["flashchip_speed"] = ESP.getFlashChipSpeed();
  FlashMode_t ideMode = ESP.getFlashChipMode();
  doc["devinfo"]["flashchip_mode"]  = flashMode[ideMode];
#if defined( _SPIFFS )
  doc["devinfo"]["filesystem_type"] = "SPIFFS";
#elif defined( _LITTLEFS )
  doc["devinfo"]["filesystem_type"] = "LittleFS";
#else
  doc["devinfo"]["filesystem_type"] = "ERROR! NO FILESYSTEM?!";
#endif
  doc["devinfo"]["compile_options"] = compileOptions;
  if (runAPmode)  doc["devinfo"]["ssid"] = devSetting->Hostname;
  else            doc["devinfo"]["ssid"] = WiFi.SSID();
#ifdef _SHOW_PASSWRDS
  doc["devinfo"]["pskkey"] = (String)WiFi.psk();
#endif
  doc["devinfo"]["wifi_rssi"]       = WiFi.RSSI();
  doc["devinfo"]["time"]            = theTime;
  doc["devinfo"]["uptime"]          = upTime();
  doc["devinfo"]["uptime_sec"]      = upTimeSeconds;
  doc["devinfo"]["daily_reboot"]    = (int)devSetting->DailyReboot;
  doc["devinfo"]["run_as_ap"]       = (int)devSetting->runAPmode;
  doc["devinfo"]["oled_type"]       = (int)devSetting->OledType;
  doc["devinfo"]["oled_flip_screen"] = (int)devSetting->OledFlip;
  doc["devinfo"]["neo_brightness"]  = (int)devSetting->NeoBrightness;
  doc["devinfo"]["smhasfaseinfo"]   = (int)smSetting->SmHasFaseInfo;
  doc["devinfo"]["telegram_interval"] = (int)devSetting->TelegramInterval;
  doc["devinfo"]["telegram_count"]  = (int)telegramCount;
  doc["devinfo"]["telegram_errors"] = (int)telegramErrors;
  doc["devinfo"]["shld_GPIOpin0"]       = (int)shieldSetting[0]->GPIOpin;
  doc["devinfo"]["shld_inversed0"]      = (int)shieldSetting[0]->inversed;
  doc["devinfo"]["shld_activeStart0"]   = (int)shieldSetting[0]->activeStart;
  doc["devinfo"]["shld_activeStop0"]    = (int)shieldSetting[0]->activeStop;
  doc["devinfo"]["shld_onThreshold0"]   = (int)shieldSetting[0]->onThreshold;
  doc["devinfo"]["shld_offThreshold0"]  = (int)shieldSetting[0]->offThreshold;
  doc["devinfo"]["shld_onDelay0"]       = (int)shieldSetting[0]->onDelay;
  doc["devinfo"]["shld_offDelay0"]      = (int)shieldSetting[0]->offDelay;
  doc["devinfo"]["shld_GPIOpin1"]       = (int)shieldSetting[1]->GPIOpin;
  doc["devinfo"]["shld_inversed1"]      = (int)shieldSetting[1]->inversed;
  doc["devinfo"]["shld_activeStart1"]   = (int)shieldSetting[1]->activeStart;
  doc["devinfo"]["shld_activeStop1"]    = (int)shieldSetting[1]->activeStop;
  doc["devinfo"]["shld_onThreshold1"]   = (int)shieldSetting[1]->onThreshold;
  doc["devinfo"]["shld_offThreshold1"]  = (int)shieldSetting[1]->offThreshold;
  doc["devinfo"]["shld_onDelay1"]       = (int)shieldSetting[1]->onDelay;
  doc["devinfo"]["shld_offDelay1"]      = (int)shieldSetting[1]->offDelay;

  snprintf(gMsg,  _GMSG_LEN, "%s:%04d", devSetting->MQTTbroker, devSetting->MQTTbrokerPort);
  doc["devinfo"]["mqtt_broker"]     = gMsg;
  doc["devinfo"]["mqtt_interval"]   = devSetting->MQTTinterval;
  if (mqttIsConnected)
        doc["devinfo"]["mqtt_broker_connected"] = "yes";
  else  doc["devinfo"]["mqtt_broker_connected"] = "no";

  doc["devinfo"]["reboots"]         = (int)nrReboots;
  doc["devinfo"]["last_reset_cpu0"] = lastResetCPU0;
  doc["devinfo"]["last_reset_cpu1"] = lastResetCPU1;
  
  serializeJsonPretty(doc, jsonBuff, _JSONBUFF_LEN);
  //-dbg-DebugTln(jsonBuff);

  httpServer.send(200, "application/json", jsonBuff);

} // sendDeviceInfo()


//=======================================================================
void sendShieldInfo()
{
  char compileOptions[200] = "";
  char theTime[20] = {0};
  time(&now);
  int thisTimeMinutes = (localtime(&now)->tm_hour * 60) + localtime(&now)->tm_min;

  DebugTln("/api/v2/shield/info");

//  char gMsg[_GMSG_LEN] = {};
  memset(jsonBuff, 0, _JSONBUFF_LEN);
  
  //-- Allocate the JsonDocument
  SpiRamJsonDocument  doc(3000);
  doc["shieldinfo"];
  doc["shieldinfo"]["shld_GPIOpin0"]       = (int)shieldSetting[0]->GPIOpin;
  if (relay0.isActive(thisTimeMinutes))
        doc["shieldinfo"]["shld_active0"] = "yes";
  else  doc["shieldinfo"]["shld_active0"] = "no";
  doc["shieldinfo"]["shld_inversed0"]      = (int)shieldSetting[0]->inversed;
  doc["shieldinfo"]["shld_activeStart0"]   = (int)shieldSetting[0]->activeStart;
  doc["shieldinfo"]["shld_activeStop0"]    = (int)shieldSetting[0]->activeStop;
  doc["shieldinfo"]["shld_onThreshold0"]   = (int)shieldSetting[0]->onThreshold;
  doc["shieldinfo"]["shld_offThreshold0"]  = (int)shieldSetting[0]->offThreshold;
  doc["shieldinfo"]["shld_onDelay0"]       = (int)shieldSetting[0]->onDelay;
  doc["shieldinfo"]["shld_offDelay0"]      = (int)shieldSetting[0]->offDelay;
  doc["shieldinfo"]["shld_GPIOpin1"]       = (int)shieldSetting[1]->GPIOpin;

  if (relay1.isActive(thisTimeMinutes))
        doc["shieldinfo"]["shld_active1"] = "yes";
  else  doc["shieldinfo"]["shld_active1"] = "no";
  doc["shieldinfo"]["shld_inversed1"]      = (int)shieldSetting[1]->inversed;
  doc["shieldinfo"]["shld_activeStart1"]   = (int)shieldSetting[1]->activeStart;
  doc["shieldinfo"]["shld_activeStop1"]    = (int)shieldSetting[1]->activeStop;
  doc["shieldinfo"]["shld_onThreshold1"]   = (int)shieldSetting[1]->onThreshold;
  doc["shieldinfo"]["shld_offThreshold1"]  = (int)shieldSetting[1]->offThreshold;
  doc["shieldinfo"]["shld_onDelay1"]       = (int)shieldSetting[1]->onDelay;
  doc["shieldinfo"]["shld_offDelay1"]      = (int)shieldSetting[1]->offDelay;

  serializeJsonPretty(doc, jsonBuff, _JSONBUFF_LEN);
  DebugTln(jsonBuff);

  httpServer.send(200, "application/json", jsonBuff);

} // sendShieldInfo()



//=======================================================================
void sendDeviceTime()
{
  memset(jsonBuff, 0, _JSONBUFF_LEN);
  char theTime[22]    = {0};
  char theTimeRev[22] = {0};

  snprintf(theTime, sizeof(theTime), "%02d-%02d-%04d %02d:%02d:%02d"
                                                    , localtime(&now)->tm_mday
                                                    , localtime(&now)->tm_mon + 1
                                                    , localtime(&now)->tm_year + 1900
                                                    , localtime(&now)->tm_hour
                                                    , localtime(&now)->tm_min
                                                    , localtime(&now)->tm_sec);

  snprintf(theTimeRev, sizeof(theTimeRev), "%04d-%02d-%02d %02d:%02d:%02d"
                                                    , localtime(&now)->tm_year + 1900
                                                    , localtime(&now)->tm_mon + 1
                                                    , localtime(&now)->tm_mday
                                                    , localtime(&now)->tm_hour
                                                    , localtime(&now)->tm_min
                                                    , localtime(&now)->tm_sec);
  DebugTf("/api/v2/dev/time [%s]\r\n", theTime);
  
  //-- Allocate the JsonDocument
  SpiRamJsonDocument  doc(3000);

  doc["devtime"];
  doc["devtime"]["timestamp"]   = lastTlgrmTime.Timestamp;
  doc["devtime"]["time"]        = theTime;
  doc["devtime"]["time_rev"]    = theTimeRev;
  doc["devtime"]["epoch"]       = time(0);
  doc["devtime"]["uptime"]      = upTime();
  doc["devtime"]["uptime_secs"] = upTimeSeconds;

  serializeJsonPretty(doc, jsonBuff, _JSONBUFF_LEN);
  //serializeJson(doc, jsonBuff, _JSONBUFF_LEN);
  httpServer.send(200, "application/json", jsonBuff);

} // sendDeviceTime()


//=======================================================================
void sendSMsettings()
{
  DebugTln("sending settings ...\r");

  memset(jsonBuff, 0, _JSONBUFF_LEN);

  //-- Allocate the JsonDocument
  SpiRamJsonDocument  doc(3000);

  JsonObject nestedRec  = doc["settings"].createNestedObject();
  nestedRec["name"]     =  "pre_dsmr40";
  nestedRec["value"]    =  smSetting->PreDSMR40;
  nestedRec["type"]     = "i"; 
  nestedRec["min"] = 0; nestedRec["max"] = 1; 

  nestedRec = doc["settings"].createNestedObject();
  nestedRec["name"]   = "ed_tariff1";
  nestedRec["value"]  =  round3(smSetting->EDT1);
  nestedRec["type"]   = "f"; 
  nestedRec["min"] = 0; nestedRec["max"] = 10; nestedRec["dec"] = 5;
  
  nestedRec = doc["settings"].createNestedObject();
  nestedRec["name"]   = "ed_tariff2";
  nestedRec["value"]  =  round3(smSetting->EDT2);
  nestedRec["type"]   = "f"; 
  nestedRec["min"] = 0; nestedRec["max"] = 10; nestedRec["dec"] = 5;

  nestedRec = doc["settings"].createNestedObject();
  nestedRec["name"]   = "er_tariff1";
  nestedRec["value"]  =  round3(smSetting->ERT1);
  nestedRec["type"]   = "f"; 
  nestedRec["min"] = 0; nestedRec["max"] = 10; nestedRec["dec"] = 5;

  nestedRec = doc["settings"].createNestedObject();
  nestedRec["name"]   = "er_tariff2";
  nestedRec["value"]  =  round3(smSetting->ERT2);
  nestedRec["type"]   = "f"; 
  nestedRec["min"] = 0; nestedRec["max"] = 10; nestedRec["dec"] = 5;

  nestedRec = doc["settings"].createNestedObject();
  nestedRec["name"]   = "gd_tariff";
  nestedRec["value"]  =  round3(smSetting->GDT);
  nestedRec["type"]   = "f"; 
  nestedRec["min"] = 0; nestedRec["max"] = 10; nestedRec["dec"] = 5;

  nestedRec = doc["settings"].createNestedObject();
  nestedRec["name"]   = "electr_netw_costs";
  nestedRec["value"]  =  round3(smSetting->ENBK);
  nestedRec["type"]   = "f"; 
  nestedRec["min"] = 0; nestedRec["max"] = 100; nestedRec["dec"] = 2;

  nestedRec = doc["settings"].createNestedObject();
  nestedRec["name"]   = "gas_netw_costs";
  nestedRec["value"]  =  round3(smSetting->GNBK);
  nestedRec["type"]   = "f"; 
  nestedRec["min"] = 0; nestedRec["max"] = 100; nestedRec["dec"] = 2;

  nestedRec = doc["settings"].createNestedObject();
  nestedRec["name"]   = "mbus1_type";
  nestedRec["value"]  =  smSetting->Mbus1Type;
  nestedRec["type"]   = "i"; 
  nestedRec["min"] = 0; nestedRec["max"] = 200; 

  nestedRec = doc["settings"].createNestedObject();
  nestedRec["name"]   = "mbus2_type";
  nestedRec["value"]  =  smSetting->Mbus2Type;
  nestedRec["type"]   = "i"; 
  nestedRec["min"] = 1; nestedRec["max"] = 200; 

  nestedRec = doc["settings"].createNestedObject();
  nestedRec["name"]   = "mbus3_type";
  nestedRec["value"]  =  smSetting->Mbus3Type;
  nestedRec["type"]   = "i"; 
  nestedRec["min"] = 1; nestedRec["max"] = 200;

  nestedRec = doc["settings"].createNestedObject();
  nestedRec["name"]   = "mbus4_type";
  nestedRec["value"]  =  smSetting->Mbus4Type;
  nestedRec["type"]   = "i"; 
  nestedRec["min"] = 1; nestedRec["max"] = 200; 

  nestedRec = doc["settings"].createNestedObject();
  nestedRec["name"]   = "sm_has_fase_info";
  nestedRec["value"]  =  smSetting->SmHasFaseInfo;
  nestedRec["type"]   = "i"; 
  nestedRec["min"] = 0; nestedRec["max"] = 1;
  
  serializeJsonPretty(doc, jsonBuff, _JSONBUFF_LEN);
  //-dbg-serializeJson(doc, jsonBuff, _JSONBUFF_LEN);
  //-dbg-Debugln(jsonBuff);
  httpServer.send(200, "application/json", jsonBuff);

} // sendSMsettings()


//=======================================================================
void sendDevSettings()
{
  DebugTln("sending System settings ...\r");

  memset(jsonBuff, 0, _JSONBUFF_LEN);

  //-- Allocate the JsonDocument
  SpiRamJsonDocument  doc(3000);

  JsonObject nestedRec  = doc["system"].createNestedObject();
  nestedRec["name"]     =  "hostname";
  nestedRec["value"]    =  devSetting->Hostname;
  nestedRec["type"]     = "s"; 
  nestedRec["maxlen"]   = sizeof(devSetting->Hostname) -1; 

  nestedRec = doc["system"].createNestedObject();
  nestedRec["name"]     =  "index_page";
  nestedRec["value"]    =  devSetting->IndexPage;
  nestedRec["type"]     = "s"; 
  nestedRec["maxlen"]   = sizeof(devSetting->IndexPage) -1; 
  
  nestedRec = doc["system"].createNestedObject();
  nestedRec["name"]   = "daily_reboot";
  nestedRec["value"]  =  devSetting->DailyReboot;
  nestedRec["type"]   = "i"; 
  nestedRec["min"] = 0; nestedRec["max"] = 1; 
  
  nestedRec = doc["system"].createNestedObject();
  nestedRec["name"]   = "run_as_ap";
  nestedRec["value"]  =  devSetting->runAPmode;
  nestedRec["type"]   = "i"; 
  nestedRec["min"] = 0; nestedRec["max"] = 1; 
  
  nestedRec = doc["system"].createNestedObject();
  nestedRec["name"]   = "no_hour_slots";
  nestedRec["value"]  =  devSetting->NoHourSlots;
  nestedRec["type"]   = "i"; 
  nestedRec["min"] = _NO_HOUR_SLOTS_; nestedRec["max"] = 190; 
  
  nestedRec = doc["system"].createNestedObject();
  nestedRec["name"]   = "no_day_slots";
  nestedRec["value"]  =  devSetting->NoDaySlots;
  nestedRec["type"]   = "i"; 
  nestedRec["min"] = _NO_DAY_SLOTS_; nestedRec["max"] = 155; 
  
  nestedRec = doc["system"].createNestedObject();
  nestedRec["name"]   = "no_month_slots";
  nestedRec["value"]  =  (devSetting->NoMonthSlots -1)/12;
  nestedRec["type"]   = "i"; 
  nestedRec["min"] = ((_NO_MONTH_SLOTS_ -1)/12); nestedRec["max"] = 5; 
  
  nestedRec = doc["system"].createNestedObject();
  nestedRec["name"]   = "alter_ring_slots";
  nestedRec["value"]  =  0;
  nestedRec["type"]   = "i"; 
  nestedRec["min"] = 0; nestedRec["max"] = 1; 

  nestedRec = doc["system"].createNestedObject();
  nestedRec["name"]   = "tlgrm_interval";
  nestedRec["value"]  =  devSetting->TelegramInterval;
  nestedRec["type"]   = "i"; 
  nestedRec["min"] = 2; nestedRec["max"] = 60;

  nestedRec = doc["system"].createNestedObject();
  nestedRec["name"]   = "oled_type";
  nestedRec["value"]  =  devSetting->OledType;
  nestedRec["type"]   = "i"; 
  nestedRec["min"] = 0; nestedRec["max"] = 2; 

  nestedRec = doc["system"].createNestedObject();
  nestedRec["name"]   = "oled_screen_time";
  nestedRec["value"]  =  devSetting->OledSleep;
  nestedRec["type"]   = "i"; 
  nestedRec["min"] = 0; nestedRec["max"] = 300; 

  nestedRec = doc["system"].createNestedObject();
  nestedRec["name"]   = "oled_flip_screen";
  nestedRec["value"]  =  devSetting->OledFlip;
  nestedRec["type"]   = "i"; 
  nestedRec["min"] = 0; nestedRec["max"] = 1;

  nestedRec = doc["system"].createNestedObject();
  nestedRec["name"]   = "neo_brightness";
  nestedRec["value"]  =  devSetting->NeoBrightness;
  nestedRec["type"]   = "i"; 
  nestedRec["min"] = 10; nestedRec["max"] = 250;

  nestedRec = doc["system"].createNestedObject();
  nestedRec["name"]     =  "mqtt_broker";
  nestedRec["value"]    =  devSetting->MQTTbroker;
  nestedRec["type"]     = "s"; 
  nestedRec["maxlen"]   = sizeof(devSetting->MQTTbroker) -1; 

  nestedRec = doc["system"].createNestedObject();
  nestedRec["name"]     =  "mqtt_broker_port";
  nestedRec["value"]    =  devSetting->MQTTbrokerPort;
  nestedRec["type"]     = "1"; 
  nestedRec["min"]      = 1; nestedRec["max"] = 9999; 

  nestedRec = doc["system"].createNestedObject();
  nestedRec["name"]     =  "mqtt_user";
  nestedRec["value"]    =  devSetting->MQTTuser;
  nestedRec["type"]     = "s"; 
  nestedRec["maxlen"]   = sizeof(devSetting->MQTTuser) -1; 

  nestedRec = doc["system"].createNestedObject();
  nestedRec["name"]     =  "mqtt_passwd";
  nestedRec["value"]    =  devSetting->MQTTpasswd;
  nestedRec["type"]     = "s"; 
  nestedRec["maxlen"]   = sizeof(devSetting->MQTTpasswd) -1; 

  nestedRec = doc["system"].createNestedObject();
  nestedRec["name"]     =  "mqtt_toptopic";
  nestedRec["value"]    =  devSetting->MQTTtopTopic;
  nestedRec["type"]     = "s"; 
  nestedRec["maxlen"]   = sizeof(devSetting->MQTTtopTopic) -1; 

  nestedRec = doc["system"].createNestedObject();
  nestedRec["name"]     =  "mqtt_interval";
  nestedRec["value"]    =  devSetting->MQTTinterval;
  nestedRec["type"]     = "i"; 
  nestedRec["min"]      = 0; nestedRec["max"] = 600; 

  serializeJsonPretty(doc, jsonBuff, _JSONBUFF_LEN);
  serializeJson(doc, jsonBuff, _JSONBUFF_LEN);
  //-dbg-Debugln(jsonBuff);
  httpServer.send(200, "application/json", jsonBuff);

} // sendDevSettings()


//=======================================================================
void sendShieldSettings()
{
  DebugTln("sending Shield32 settings ...\r");

  char timeBuff[10] = {};
  uint16_t hours   = 0;
  uint16_t minutes = 0;

  memset(jsonBuff, 0, _JSONBUFF_LEN);

  //-- Allocate the JsonDocument
  SpiRamJsonDocument  doc(3000);
  JsonObject nestedRec = doc["shield"].createNestedObject();
  nestedRec["name"]   = "shld_GPIOpin0";
  nestedRec["value"]  =  shieldSetting[0]->GPIOpin;
  nestedRec["type"]   = "i"; 
  nestedRec["min"] = -1; nestedRec["max"] = 14; 
  
  nestedRec = doc["shield"].createNestedObject();
  nestedRec["name"]   = "shld_inversed0";
  nestedRec["value"]  =  shieldSetting[0]->inversed;
  nestedRec["type"]   = "i"; 
  nestedRec["min"] = 0; nestedRec["max"] = 1; 
  
  DebugTf("[0] shld_activeStart0: %d\r\n", shieldSetting[0]->activeStart);
  nestedRec  = doc["shield"].createNestedObject();
  nestedRec["name"]     =  "shld_activeStart0";
  // Convert time in minutes to HH:MM format
  hours   = shieldSetting[0]->activeStart / 60;
  minutes = shieldSetting[0]->activeStart % 60;
  // Create the HH:MM formatted string
  //timeBuff = String(hours) + ":" + (minutes < 10 ? "0" : "") + String(minutes);
  snprintf(timeBuff, sizeof(timeBuff), "%02d:%02d", hours, minutes);
  DebugTf("[0] hours[%02d] minutes[%02d] => timeBuff: [%s]\r\n", hours, minutes, timeBuff);
  nestedRec["value"]    =  timeBuff;
  nestedRec["type"]     = "s"; 
  nestedRec["maxlen"]   = 6; 

  DebugTf("[0] shld_activeStop0: %d\r\n", shieldSetting[0]->activeStop);
  nestedRec = doc["shield"].createNestedObject();
  nestedRec["name"]     =  "shld_activeStop0";
  // Convert time in minutes to HH:MM format
  hours   = shieldSetting[0]->activeStop / 60;
  minutes = shieldSetting[0]->activeStop % 60;
  // Create the HH:MM formatted string
  //timeBuff = String(hours) + ":" + (minutes < 10 ? "0" : "") + String(minutes);
  snprintf(timeBuff, sizeof(timeBuff), "%02d:%02d", hours, minutes);
  DebugTf("[0] hours[%02d] minutes[%02d] => timeBuff: [%s]\r\n", hours, minutes, timeBuff);
  nestedRec["value"]    =  timeBuff;
  nestedRec["type"]     = "s"; 
  nestedRec["maxlen"]   = 6; 
  
  nestedRec = doc["shield"].createNestedObject();
  nestedRec["name"]   = "shld_onThreshold0";
  nestedRec["value"]  =  shieldSetting[0]->onThreshold;
  nestedRec["type"]   = "i"; 
  nestedRec["min"] = -10000; nestedRec["max"] = 10000; 
  
  nestedRec = doc["shield"].createNestedObject();
  nestedRec["name"]   = "shld_offThreshold0";
  nestedRec["value"]  =  shieldSetting[0]->offThreshold;
  nestedRec["type"]   = "i"; 
  nestedRec["min"] = -10000; nestedRec["max"] = 10000; 
  
  nestedRec = doc["shield"].createNestedObject();
  nestedRec["name"]   = "shld_onDelay0";
  nestedRec["value"]  =  shieldSetting[0]->onDelay;
  nestedRec["type"]   = "i"; 
  nestedRec["min"] = 0; nestedRec["max"] = 36000; 
  
  nestedRec = doc["shield"].createNestedObject();
  nestedRec["name"]   = "shld_offDelay0";
  nestedRec["value"]  =  shieldSetting[0]->offDelay;
  nestedRec["type"]   = "i"; 
  nestedRec["min"] = 0; nestedRec["max"] = 36000; 

//------------ shieldSetting[1] ---------------------------------------
  nestedRec = doc["shield"].createNestedObject();
  nestedRec["name"]   = "shld_GPIOpin1";
  nestedRec["value"]  =  shieldSetting[1]->GPIOpin;
  nestedRec["type"]   = "i"; 
  nestedRec["min"] = -1; nestedRec["max"] = 14; 
  
  nestedRec = doc["shield"].createNestedObject();
  nestedRec["name"]   = "shld_inversed1";
  nestedRec["value"]  =  shieldSetting[1]->inversed;
  nestedRec["type"]   = "i"; 
  nestedRec["min"] = 0; nestedRec["max"] = 1; 
  
  DebugTf("shld_activeStart1: %d\r\n", shieldSetting[1]->activeStart);
  nestedRec  = doc["shield"].createNestedObject();
  nestedRec["name"]     =  "shld_activeStart1";
  // Convert time in minutes to HH:MM format
  hours   = shieldSetting[1]->activeStart / 60;
  minutes = shieldSetting[1]->activeStart % 60;
  // Create the HH:MM formatted string
  //timeBuff = String(hours) + ":" + (minutes < 10 ? "0" : "") + String(minutes);
  snprintf(timeBuff, sizeof(timeBuff), "%02d:%02d", hours, minutes);
  DebugTf("[1] hours[%02d] minutes[%02d] => timeBuff: [%s]\r\n", hours, minutes, timeBuff);
  nestedRec["value"]    =  timeBuff;
  nestedRec["type"]     = "s"; 
  nestedRec["maxlen"]   = 6; 

  DebugTf("shld_activeStop1: %d\r\n", shieldSetting[1]->activeStop);
  nestedRec = doc["shield"].createNestedObject();
  nestedRec["name"]     =  "shld_activeStop1";
  // Convert time in minutes to HH:MM format
  hours   = shieldSetting[1]->activeStop / 60;
  minutes = shieldSetting[1]->activeStop % 60;
  // Create the HH:MM formatted string
  //timeBuff = String(hours) + ":" + (minutes < 10 ? "0" : "") + String(minutes);
  snprintf(timeBuff, sizeof(timeBuff), "%02d:%02d", hours, minutes);
  DebugTf("[1] hours[%02d] minutes[%02d] => timeBuff: [%s]\r\n", hours, minutes, timeBuff);
  nestedRec["value"]    =  timeBuff;
  nestedRec["type"]     = "s"; 
  nestedRec["maxlen"]   = 6; 
  
  nestedRec = doc["shield"].createNestedObject();
  nestedRec["name"]   = "shld_onThreshold1";
  nestedRec["value"]  =  shieldSetting[1]->onThreshold;
  nestedRec["type"]   = "i"; 
  nestedRec["min"] = -10000; nestedRec["max"] = 10000; 
  
  nestedRec = doc["shield"].createNestedObject();
  nestedRec["name"]   = "shld_offThreshold1";
  nestedRec["value"]  =  shieldSetting[1]->offThreshold;
  nestedRec["type"]   = "i"; 
  nestedRec["min"] = -10000; nestedRec["max"] = 10000; 
  
  nestedRec = doc["shield"].createNestedObject();
  nestedRec["name"]   = "shld_onDelay1";
  nestedRec["value"]  =  shieldSetting[1]->onDelay;
  nestedRec["type"]   = "i"; 
  nestedRec["min"] = 0; nestedRec["max"] = 36000; 
  
  nestedRec = doc["shield"].createNestedObject();
  nestedRec["name"]   = "shld_offDelay1";
  nestedRec["value"]  =  shieldSetting[1]->offDelay;
  nestedRec["type"]   = "i"; 
  nestedRec["min"] = 0; nestedRec["max"] = 36000; 

  serializeJsonPretty(doc, jsonBuff, _JSONBUFF_LEN);
  serializeJson(doc, jsonBuff, _JSONBUFF_LEN);
  Debugln(jsonBuff);
  httpServer.send(200, "application/json", jsonBuff);

} // sendShieldSettings()


//=======================================================================
void sendDeviceDebug(const char *URI, String tail)
{
  sendApiNotFound(URI);

} // sendDeviceDebug()


//=======================================================================
void sendJsonV2smApi(const char *firstLevel)
{
  //char jsonBuff[2000] = {};
  memset(jsonBuff, 0, _JSONBUFF_LEN);
  memset(fieldTable, 0, (sizeof(fieldTableStruct) *100));
  fieldTableCount = 0;
  
  tlgrmData.applyEach(buildJsonV2ApiSm());
  if (strcmp(firstLevel, "actual") == 0)
  {
    time(&now);
    addToTable("gas_delivered", gasDelivered);
    uint16_t nowMinutes = (localtime(&now)->tm_hour*60) + localtime(&now)->tm_min;
    DebugTf("[%02d:%02d] >>>> nowMinutes[%d]\r\n", localtime(&now)->tm_hour, localtime(&now)->tm_min, nowMinutes);
    addToTable("relay_active0", (int)relay0.isActive(nowMinutes));
    addToTable("relay_state0", (int)relay0.getRelayState());
    addToTable("relay_active1", (int)relay1.isActive(nowMinutes));
    addToTable("relay_state1", (int)relay1.getRelayState());
  }
  //-- Allocate the JsonDocument
  SpiRamJsonDocument  doc(3000);

  doc[firstLevel];

  for(int i=0; i<fieldTableCount; i++)
  {
    //DebugTf("name[%-30.30s] -> ", fieldTable[i].cName);
    switch (fieldTable[i].cType)
    {
      case 'i': //Debugf("val(%c)[%d]\r\n", fieldTable[i].cType, fieldTable[i].type.iValue);
                doc[firstLevel][fieldTable[i].cName] = fieldTable[i].type.iValue;
                break;
      case 'u': //Debugf("val(%c)[%d]\r\n", fieldTable[i].cType, fieldTable[i].type.uValue);
                doc[firstLevel][fieldTable[i].cName] = fieldTable[i].type.uValue;
                break;
      case 'f': //Debugf("val(%c)[%.3f]\r\n", fieldTable[i].cType, fieldTable[i].type.fValue);
                doc[firstLevel][fieldTable[i].cName] = round3(fieldTable[i].type.fValue);
                break;
      default:  //Debugf("val(%c)[%s]\r\n", fieldTable[i].cType, fieldTable[i].type.cValue);
                doc[firstLevel][fieldTable[i].cName] = fieldTable[i].type.cValue;
    }
  }

  serializeJsonPretty(doc, jsonBuff, _JSONBUFF_LEN);
  //serializeJson(doc, jsonBuff, _JSONBUFF_LEN);
  httpServer.send(200, "application/json", jsonBuff);

} // sendJsonV2smApi()


//=======================================================================
//  {
//    "store": [ {
//        "1": {
//          "actual": { 
//              "recid": "221216102323",
//              "power_delivered_l1": 6185.35791,
//              "power_delivered_l2": 9109.216797,
//              "power_delivered_l3": 1182.099976,
//              "power_returned_l1": 2500.5,
//              "power_returned_l2": 300.15,
//              "power_returned_l3": 200.51,
//              "gas_delivered": 4939.404199
//            },
//        "2": {
//          "actual": { 
//              "recid": "221216102333",
//              "power_delivered_l1": 6186.35791,
//              "power_delivered_l2": 9110.216797,
//              "power_delivered_l3": 1183.099976,
//              "power_returned_l1": 2600.5,
//              "power_returned_l2": 300.15,
//              "power_returned_l3": 200.51,
//              "gas_delivered": 4939.804199
//            }, ..
//        }
//      ]
//  }
//=======================================================================
void sendJsonActualHist()
{
  char cRecnr[10] = {0};
  
  SpiRamJsonDocument  doc(200000); 
  memset(jsonBuff, 0, _JSONBUFF_LEN);
  
  DebugTf("(2) Total Psram [%d bytes], Used [%d bytes], Psram Free [%d bytes]\r\n"
                       , ESP.getPsramSize()
                                        , (ESP.getPsramSize() - ESP.getFreePsram())
                                                               , ESP.getFreePsram());

  doc["store"];

  for (int i=0; i<_MAX_ACTUAL_STORE; i++)
  {
    int s = (i+actualStoreSlot+1) % _MAX_ACTUAL_STORE;
    if ( strlen(actualStore[s].timestamp) < 12) { continue; }
    //-- built JSON string ..
    snprintf(cRecnr, sizeof(cRecnr), "%d", i);
    JsonObject nestedRec = doc["store"][cRecnr].createNestedObject();
    nestedRec["actual"]["timestamp"]          = actualStore[s].timestamp;
    nestedRec["actual"]["power_delivered_l1"] = round3(actualStore[s].power_delivered_l1);
    nestedRec["actual"]["power_delivered_l2"] = round3(actualStore[s].power_delivered_l2);
    nestedRec["actual"]["power_delivered_l3"] = round3(actualStore[s].power_delivered_l3);
    nestedRec["actual"]["power_returned_l1"]  = round3(actualStore[s].power_returned_l1);
    nestedRec["actual"]["power_returned_l2"]  = round3(actualStore[s].power_returned_l2);
    nestedRec["actual"]["power_returned_l3"]  = round3(actualStore[s].power_returned_l3);
    nestedRec["actual"]["gas_delivered"]      = round3(actualStore[s].gas_delivered);

    if (Verbose2)
    {
      Debugf("[%3d][%5d][%-12.12s] PwrDel[%10.3f] PwrRet[%10.3f] GasDel[%10.3f]\r\n"
              , i, actualStore[s].count, actualStore[s].timestamp
                                       , (actualStore[s].power_delivered_l1
                                          + actualStore[s].power_delivered_l2
                                          + actualStore[s].power_delivered_l3)
                                       , (actualStore[s].power_returned_l1
                                          + actualStore[s].power_returned_l2
                                          + actualStore[s].power_returned_l3)
                                       , actualStore[s].gas_delivered);
    }
  
  } //  for i...
  
  serializeJsonPretty(doc, jsonBuff, _JSONBUFF_LEN);
  DebugTf("jsonBuff length is [%d]bytes\r\n", strlen(jsonBuff));
  if (sizeof(jsonBuff) >= _JSONBUFF_LEN )
  {
      writeToSysLog("ERROR!!! jsonString > MAX_BUFF (%d > %d)!"
                                  , strlen(jsonBuff), _JSONBUFF_LEN);
  }
  httpServer.send(200, "application/json", jsonBuff);

} //  sendJsonActualHist()


//=======================================================================
//  {"hours":[
//    {"recnr": 0, "recid": "22112117", "slot": 36,"edt1": 327.865, "edt2": 761.109,"ert1": 128.623, "ert2": 84.245,"gdt": 0.000},
//    {"recnr": 1, "recid": "22112116", "slot": 35,"edt1": 327.619, "edt2": 760.185,"ert1": 128.583, "ert2": 84.226,"gdt": 0.000},
//    {"recnr": 2, "recid": "22112115", "slot": 34,"edt1": 326.777, "edt2": 757.131,"ert1": 128.441, "ert2": 84.153,"gdt": 0.000},
//    {"recnr": 3, "recid": "22112114", "slot": 33,"edt1": 325.262, "edt2": 751.726,"ert1": 128.196, "ert2": 84.024,"gdt": 241.074},
//  ]}
//=======================================================================
void sendJsonHist(int8_t ringType, const char *fileName, timeStruct useTime, uint8_t limit, bool sortDesc)
{
  uint16_t  startSlot, offset, readSlot, slot, nrSlots, maxSlots, recNr = 0;
  char      typeApi[10] = {0};
  char      buffer[DATA_RECLEN +2] = {0};
  char      recID[10]  = {0};
  float     EDT1, EDT2, ERT1, ERT2, GDT;

  memset(jsonBuff, 0, _JSONBUFF_LEN);

  if (DUE(antiWearTimer))
  {
    writeDataToRingFiles(useTime);
    writeLastStatus();
  }

  switch(ringType)
  {
    case RNG_HOURS:
      startSlot       = (useTime.Hours % devSetting->NoHourSlots);
      nrSlots         = devSetting->NoHourSlots;
      maxSlots        = devSetting->NoHourSlots;
      strlcpy(typeApi, "hours", 9);
      break;
    case RNG_DAYS:
      startSlot       = (useTime.Days % devSetting->NoDaySlots);
      nrSlots         = devSetting->NoDaySlots;
      maxSlots        = devSetting->NoDaySlots;
      strlcpy(typeApi, "days", 9);
      break;
    case RNG_MONTHS:
      startSlot       = (useTime.Months % devSetting->NoMonthSlots);
      nrSlots         = devSetting->NoMonthSlots;
      maxSlots        = devSetting->NoMonthSlots;
      strlcpy(typeApi, "months", 9);
      break;
  }

  if (!_FSYS.exists(fileName))
  {
    DebugTf("File [%s] does not excist!\r\n", fileName);
    return;
  }

  File dataFile = _FSYS.open(fileName, "r+");  // read and write ..
  if (!dataFile)
  {
    DebugTf("Error opening [%s]\r\n", fileName);
    return;
  }

  //-- Allocate the Huge JsonDocument
  DebugTf("(1) Total Psram [%d bytes], Used [%d bytes], Psram Free [%d bytes]\r\n"
                       , ESP.getPsramSize()
                                        , (ESP.getPsramSize() - ESP.getFreePsram())
                                                               , ESP.getFreePsram());

  SpiRamJsonDocument  doc(60000); 

  DebugTf("(2) Total Psram [%d bytes], Used [%d bytes], Psram Free [%d bytes]\r\n"
                       , ESP.getPsramSize()
                                        , (ESP.getPsramSize() - ESP.getFreePsram())
                                                               , ESP.getFreePsram());

  doc[typeApi];

  if (sortDesc)
        startSlot += nrSlots +1; // <==== voorbij actuele slot!
  else  startSlot += nrSlots;    // <==== start met actuele slot!

  if (limit>0) { nrSlots = limit; }
  
  if (Verbose2) DebugTf("sendJsonHist [%s] startSlot[%02d]\r\n", typeApi, (startSlot % nrSlots));

  for (uint8_t s = 0; s < nrSlots; s++)
  {
    if (sortDesc)
    {
      readSlot = (s +startSlot);
      slot    = (readSlot % maxSlots);
      // slot goes from 0 to _NO_OF_SLOTS_
      // we need to add 1 to slot to skip header record!
      offset  = ((slot +1) * (DATA_RECLEN +1));
      if (Verbose1) 
          DebugTf("s[%d], start[%d] -> slot[%d] (offset[%d])\r\n", s, startSlot, slot, offset); 
      dataFile.seek(offset, SeekSet);
      int bytesRead= dataFile.readBytesUntil('\n', buffer, DATA_RECLEN);
      //Debugln(buffer);
      if (bytesRead != DATA_RECLEN)   // '\n' is skipped by readBytesUntil()
      {
        DebugTf("bytesRead[%d] != DATA_RECLEN[%d]\r\n", bytesRead, DATA_RECLEN);
        if (!isValidTimestamp(buffer, 8))   // first 8 bytes is YYMMDDHH
        {
          DebugTf("slot[%02d]==>timeStamp [%-8.8s] not valid!!\r\n", slot, buffer);
          //esp32 writeToSysLog("slot[%02d]==>timeStamp [%-8.8s] not valid!!", slot, buffer);
        }
      }
      else  // all is OK
      {
        //Debug(" all OK ");
        sscanf(buffer, "%[^;];%f;%f;%f;%f;%f", recID
                       , &EDT1, &EDT2, &ERT1, &ERT2, &GDT);
        JsonObject nestedRec = doc[typeApi].createNestedObject();
        nestedRec["recnr"]  = recNr++;
        nestedRec["recid"]  = recID;
        nestedRec["slot"]   = slot;
        nestedRec["edt1"]   = round3(EDT1);
        nestedRec["edt2"]   = round3(EDT2);
        nestedRec["ert1"]   = round3(ERT1);
        nestedRec["ert2"]   = round3(ERT2);
        nestedRec["gdt"]    = round3(GDT);
      }

    } //  desc ...
    else  //  sort asc ...
    {
      readSlot = (startSlot -s);
      slot    = (readSlot % maxSlots);
      // slot goes from 0 to _NO_OF_SLOTS_
      // we need to add 1 to slot to skip header record!
      offset  = ((slot +1) * (DATA_RECLEN +1));
      if (Verbose2) 
          DebugTf("s[%d][%s], start[%d] -> slot[%d] (offset[%d])\r\n", s, typeApi, startSlot, slot, offset); 
      dataFile.seek(offset, SeekSet);
      int bytesRead = dataFile.readBytesUntil('\n', buffer, DATA_RECLEN);
      if (Verbose1) Debugln(buffer);

      if (bytesRead != DATA_RECLEN)   // '\n' is skipped by readBytesUntil()
      {
        DebugTf("bytesRead[%d] != DATA_RECLEN[%d]\r\n", bytesRead, DATA_RECLEN);
        if (!isValidTimestamp(buffer, 8))   // first 8 bytes is YYMMDDHH
        {
          DebugTf("slot[%02d]==>timeStamp [%-13.13s] not valid!!\r\n", slot, buffer);
          //esp32 writeToSysLog("slot[%02d]==>timeStamp [%-13.13s] not valid!!", slot, buffer);
        }
      }
      else  // all is OK
      {
        //Debug(" all OK ");
        sscanf(buffer, "%[^;];%f;%f;%f;%f;%f", recID
                       , &EDT1, &EDT2, &ERT1, &ERT2, &GDT);
        JsonObject nestedRec = doc[typeApi].createNestedObject();
        nestedRec["recnr"]  = recNr++;
        nestedRec["recid"]  = recID;
        nestedRec["slot"]   = slot;
        nestedRec["edt1"]   = round3(EDT1);
        nestedRec["edt2"]   = round3(EDT2);
        nestedRec["ert1"]   = round3(ERT1);
        nestedRec["ert2"]   = round3(ERT2);
        nestedRec["gdt"]    = round3(GDT);
      }

    } //  asc ...
  
  } //  for al slots ..

  dataFile.close();

  serializeJsonPretty(doc, jsonBuff, _JSONBUFF_LEN);
  DebugTf("jsonBuff length is [%d]bytes\r\n", strlen(jsonBuff));
  if (sizeof(jsonBuff) >= _JSONBUFF_LEN )
  {
      writeToSysLog("ERROR!!! jsonString > MAX_BUFF (%d > %d)!"
                                  , strlen(jsonBuff), _JSONBUFF_LEN);
  }
  httpServer.send(200, "application/json", jsonBuff);

} // sendJsonHist()


//====================================================
bool isInFieldsArray(const char *lookUp, int elemts)
{
  if (elemts == 0) return true;

  for (int i=0; i<elemts; i++)
  {
    //if (Verbose2)
    //  DebugTf("[%2d] Looking for [%s] in array[%s]\r\n", i, lookUp, fieldsArray[i]);
    if (strncmp(lookUp, fieldsArray[i], strlen(fieldsArray[i])) == 0) return true;
  }
  return false;

} // isInFieldsArray()


//====================================================
void copyToFieldsArray(const char inArray[][35], int elemts)
{
  int i = 0;
  memset(fieldsArray, 0, sizeof(fieldsArray));
  //if (Verbose2) DebugTln("start copying ....");

  for ( i=0; i<elemts; i++)
  {
    strncpy(fieldsArray[i], inArray[i], 34);
    //if (Verbose1) DebugTf("[%2d] => inArray[%s] fieldsArray[%s]\r\n", i, inArray[i], fieldsArray[i]);

  }
  fieldsElements = i;

} // copyToFieldsArray()


//====================================================
void listFieldsArray(char inArray[][35])
{
  int i = 0;

  for ( i=0; strlen(inArray[i]) == 0; i++)
  {
    DebugTf("[%2d] => inArray[%s]\r\n", i, inArray[i]);
  }

} // listFieldsArray()


//====================================================
void sendApiNotFound(const char *URI)
{
  httpServer.sendHeader("Access-Control-Allow-Origin", "*");
  httpServer.setContentLength(CONTENT_LENGTH_UNKNOWN);
  httpServer.send ( 404, "text/html", "<!DOCTYPE HTML><html><head>");

  strlcpy(gMsg, "<style>body { background-color: lightgray; font-size: 15pt;}", _GMSG_LEN);
  strlcat(gMsg, "</style></head><body>",  _GMSG_LEN);
  httpServer.sendContent(gMsg);

  strlcpy(gMsg, "<h1>DSMR-logger</h1><b1>", _GMSG_LEN);
  httpServer.sendContent(gMsg);

  strlcpy(gMsg, "<br>[<b>", _GMSG_LEN);
  strlcat(gMsg, URI, _GMSG_LEN);
  strlcat(gMsg, "</b>] is not a valid ", _GMSG_LEN);
  httpServer.sendContent(gMsg);

  strlcpy(gMsg, "<a href=", _GMSG_LEN);
  strlcat(gMsg, "\"https://mrwheel-docs.gitbook.io/DSMRloggerAPI/beschrijving-restapis\">", _GMSG_LEN);
  strlcat(gMsg, "restAPI</a> call.", _GMSG_LEN);
  httpServer.sendContent(gMsg);

  strlcpy(gMsg, "</body></html>\r\n", _GMSG_LEN);
  httpServer.sendContent(gMsg);

  //esp32 writeToSysLog("[%s] is not a valid restAPI call!!", URI);

} // sendApiNotFound()



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
