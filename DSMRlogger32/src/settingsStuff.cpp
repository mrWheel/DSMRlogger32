/*
***************************************************************************
**  Program  : settingsStuff, part of DSMRlogger32
**  Version  : v5.n
**
**  Copyright (c) 2020 .. 2023 Willem Aandewiel
**
**  TERMS OF USE: MIT License. See bottom of file.
***************************************************************************
*/
#include "settingsStuff.h"

//=======================================================================
void writeSmSettings()
{
  yield();
  DebugTf("Writing to [%s] ..\r\n", _SETTINGS_FILE);
  
  File file = _FSYS.open(_SETTINGS_FILE, "w"); // open for reading and writing
  if (!file)
  {
    DebugTf("open(%s, 'w') FAILED!!! --> Bailout\r\n", _SETTINGS_FILE);
    return;
  }
  yield();

  if (devSetting->TelegramInterval < 2)  devSetting->TelegramInterval = 10;

  DebugTln(F("Start writing setting data .."));

  //-- Allocate the JsonDocument
  SpiRamJsonDocument  doc(3000);
  
  //-- Fill JSON document from settings
  doc["preDSMR40"]          = smSetting->PreDSMR40;
  doc["EnergyDeliveredT1"]  = String(smSetting->EDT1, 5);
  doc["EnergyDeliveredT2"]  = String(smSetting->EDT2, 5);
  doc["EnergyReturnedT1"]   = String(smSetting->ERT1, 5);
  doc["EnergyReturnedT2"]   = String(smSetting->ERT2, 5);
  doc["mBus1Type"]          = smSetting->Mbus1Type;
  doc["mBus2Type"]          = smSetting->Mbus2Type;
  doc["mBus3Type"]          = smSetting->Mbus3Type;
  doc["mBus4Type"]          = smSetting->Mbus4Type;
  doc["GasDeliveredT"]      = String(smSetting->GDT,  5);
  doc["EnergyVasteKosten"]  = String(smSetting->ENBK, 2);
  doc["GasVasteKosten"]     = String(smSetting->GNBK, 2);
  doc["SmHasFaseInfo"]      = smSetting->SmHasFaseInfo;

  //DebugTln("---------------------------------------------------");
  //serializeJsonPretty(doc, Serial);
  //Debugln();
  //DebugTln("---------------------------------------------------");
  // Serialize JSON to file
  bool success = serializeJsonPretty(doc, file) > 0;
  if (!success)
  {
    DebugTln("Failed to serialize and write devSetting settings to file");
  }

  file.close();

  Debugln(F(" done"));
  if (Verbose1)
  {
    DebugTln(F("Wrote this:"));
    DebugT(F("EnergyDeliveredT1 = "));
    Debugln(String(smSetting->EDT1, 5));
    DebugT(F("EnergyDeliveredT2 = "));
    Debugln(String(smSetting->EDT2, 5));
    DebugT(F("EnergyReturnedT1 = "));
    Debugln(String(smSetting->ERT1, 5));
    DebugT(F("EnergyReturnedT2 = "));
    Debugln(String(smSetting->ERT2, 5));
    DebugT(F("mBus1Type = "));
    Debugln(smSetting->Mbus1Type);
    DebugT(F("mBus2Type = "));
    Debugln(smSetting->Mbus2Type);
    DebugT(F("mBus3Type = "));
    Debugln(smSetting->Mbus3Type);
    DebugT(F("mBus4Type = "));
    Debugln(smSetting->Mbus4Type);
    DebugT(F("GasDeliveredT = "));
    Debugln(String(smSetting->GDT,  5));
    DebugT(F("EnergyVasteKosten = "));
    Debugln(String(smSetting->ENBK, 2));
    DebugT(F("GasVasteKosten = "));
    Debugln(String(smSetting->GNBK, 2));
    DebugT(F("OledType = "));
    if (devSetting->OledType == 1)      Debugln("SDD1306");
    else if (devSetting->OledType == 2) Debugln("SH1306");
    else                             Debugln("None");
    DebugT(F("OledSleep = "));
    Debugln(devSetting->OledSleep);
    DebugT(F("OledFlip = "));
    if (devSetting->OledFlip)  Debugln(F("Yes"));
    else                    Debugln(F("No"));

    DebugT(F("SmHasFaseInfo"));
    if (smSetting->SmHasFaseInfo == 1) Debugln("Yes");
    else                             Debugln("No");

  } // Verbose1

} // writeSmSettings()


//=======================================================================
void readSmSettings(bool show)
{
  String sTmp, nColor;
  String words[10];

  File file;

  DebugTf(" %s ..\r\n", _SETTINGS_FILE);
  
  if (!_FSYS.exists(_SETTINGS_FILE))
  {
    DebugTln(F(" .. file not found! --> created file!"));
    smSetting->PreDSMR40      = 0;
    smSetting->EDT1           = 0.0;
    smSetting->EDT2           = 0.0;
    smSetting->ERT1           = 0.0;
    smSetting->ERT2           = 0.0;
    smSetting->GDT            = 0.0;
    smSetting->ENBK           = 0.0;
    smSetting->GNBK           = 0.0;
    smSetting->Mbus1Type      = 3;
    smSetting->SmHasFaseInfo  = 1;
    writeSmSettings();
  }

  file = _FSYS.open(_SETTINGS_FILE, "r");
  if (!file)
  {
      DebugTf(" .. something went wrong opening [%s]\r\n", _SETTINGS_FILE);
      delay(100);
      return;
  }
 
  DebugTln(F("Reading settings:\r"));

  //-- Allocate the JsonDocument
  SpiRamJsonDocument  doc(3000);

  //-- Parse the JSON object in the file
  DeserializationError err = deserializeJson(doc, file);

  //-- This may fail if the JSON is invalid
  if (err)
  {
    DebugT("Failed to deserialize logger settings: ");
    Debugln(err.f_str());
    file.close();
    return;
  }

  file.close();

  //DebugTln("---------------------------------------------------");
  //serializeJsonPretty(doc, Serial);
  //Debugln();
  //DebugTln("---------------------------------------------------");

  //-- Extract settings from the JSON document
  if (doc["preDSMR40"])         { smSetting->PreDSMR40 = doc["preDSMR40"].as<int>(); }
  if (doc["EnergyDeliveredT1"]) { smSetting->EDT1 = doc["EnergyDeliveredT1"].as<float>(); }
  if (doc["EnergyDeliveredT2"]) { smSetting->EDT2 = doc["EnergyDeliveredT2"].as<float>(); }
  if (doc["EnergyReturnedT1"])  { smSetting->ERT1  = doc["EnergyReturnedT1"].as<float>(); }
  if (doc["EnergyReturnedT2"])  { smSetting->ERT2  = doc["EnergyReturnedT2"].as<float>(); }
  if (doc["mBus1Type"])         { smSetting->Mbus1Type = doc["mBus1Type"].as<int>(); }
  if (doc["mBus2Type"])         { smSetting->Mbus2Type = doc["mBus2Type"].as<int>(); }
  if (doc["mBus3Type"])         { smSetting->Mbus3Type = doc["mBus3Type"].as<int>(); }
  if (doc["mBus4Type"])         { smSetting->Mbus4Type = doc["mBus4Type"].as<int>(); }
  if (doc["GasDeliveredT"])     { smSetting->GDT = doc["GasDeliveredT"].as<float>(); }
  if (doc["EnergyVasteKosten"]) { smSetting->ENBK = doc["EnergyVasteKosten"].as<float>(); }
  if (doc["GasVasteKosten"])    { smSetting->GNBK = doc["GasVasteKosten"].as<float>(); }
  if (doc["SmHasFaseInfo"])     { smSetting->SmHasFaseInfo  = doc["SmHasFaseInfo"].as<int>(); }

  DebugTln(F(" .. done\r"));

  if (smSetting->SmHasFaseInfo != 0)  smSetting->SmHasFaseInfo = 1;
  else                                smSetting->SmHasFaseInfo = 0;

  if (!show) return;

  Debugln(F("\r\n==== Smart Meter settings ========================================\r"));
  Debugf("   Pre DSMR 40 (0=No, 1=Yes) : %s\r\n",     smSetting->PreDSMR40 ? "Yes":"No");
  Debugf("   Energy Delivered Tarief 1 : %9.7f\r\n",  smSetting->EDT1);
  Debugf("   Energy Delivered Tarief 2 : %9.7f\r\n",  smSetting->EDT2);
  Debugf("   Energy Delivered Tarief 1 : %9.7f\r\n",  smSetting->ERT1);
  Debugf("   Energy Delivered Tarief 2 : %9.7f\r\n",  smSetting->ERT2);
  Debugf("        Gas Delivered Tarief : %9.7f\r\n",  smSetting->GDT);
  Debugf("     Energy Netbeheer Kosten : %9.2f\r\n",  smSetting->ENBK);
  Debugf("        Gas Netbeheer Kosten : %9.2f\r\n",  smSetting->GNBK);
  Debugf("                 MBus 1 Type : %d\r\n",     smSetting->Mbus1Type);
  Debugf("                 MBus 2 Type : %d\r\n",     smSetting->Mbus2Type);
  Debugf("                 MBus 3 Type : %d\r\n",     smSetting->Mbus3Type);
  Debugf("                 MBus 4 Type : %d\r\n",     smSetting->Mbus4Type);
  Debugf("  SM Fase Info (0=No, 1=Yes) : %s\r\n",     smSetting->SmHasFaseInfo ? "Yes":"No");

  Debugln(F("-\r"));

} // readSmSettings()


//=======================================================================
void updateSmSettings(const char *field, const char *newValue)
{
  DebugTf("-> field[%s], newValue[%s]\r\n", field, newValue);

  //if (!stricmp(field, "pre_DSMR40"))        smSetting->PreDSMR40    = String(newValue).toInt();
  if (!strcasecmp(field, "pre_DSMR40"))     smSetting->PreDSMR40    = String(newValue).toInt();
  if (!strcasecmp(field, "ed_tariff1"))        smSetting->EDT1         = String(newValue).toFloat();
  if (!strcasecmp(field, "ed_tariff2"))        smSetting->EDT2         = String(newValue).toFloat();
  if (!strcasecmp(field, "er_tariff1"))        smSetting->ERT1         = String(newValue).toFloat();
  if (!strcasecmp(field, "er_tariff2"))        smSetting->ERT2         = String(newValue).toFloat();
  if (!strcasecmp(field, "electr_netw_costs")) smSetting->ENBK         = String(newValue).toFloat();

  if (!strcasecmp(field, "mbus1_type"))        smSetting->Mbus1Type    = String(newValue).toInt();
  if (!strcasecmp(field, "mbus2_type"))        smSetting->Mbus2Type    = String(newValue).toInt();
  if (!strcasecmp(field, "mbus3_type"))        smSetting->Mbus3Type    = String(newValue).toInt();
  if (!strcasecmp(field, "mbus4_type"))        smSetting->Mbus4Type    = String(newValue).toInt();
  if (!strcasecmp(field, "gd_tariff"))         smSetting->GDT          = String(newValue).toFloat();
  if (!strcasecmp(field, "gas_netw_costs"))    smSetting->GNBK         = String(newValue).toFloat();

  if (!strcasecmp(field, "sm_has_fase_info"))
  {
    smSetting->SmHasFaseInfo = String(newValue).toInt();
    if (smSetting->SmHasFaseInfo != 0)  smSetting->SmHasFaseInfo = 1;
    else                              smSetting->SmHasFaseInfo = 0;
  }
  writeSmSettings();

} // updateSmSettings()


//=======================================================================
void writeDevSettings(bool show)
{
  yield();
  DebugTf("Writing to [%s] ..\r\n", _SYSTEM_FILE);

  File file = _FSYS.open(_SYSTEM_FILE, "w"); // open for reading and writing
  if (!file)
  {
    DebugTf("open(%s, 'w') FAILED!!! --> Bailout\r\n", _SYSTEM_FILE);
    return;
  }
  yield();

  if (strlen(devSetting->IndexPage) < 7) strlcpy(devSetting->IndexPage, "DSMRindex.html", (_INDEXPAGE_LEN -1));
  if (devSetting->MQTTbrokerPort < 1)    devSetting->MQTTbrokerPort = 1883;
  if (devSetting->NoHourSlots  < _NO_HOUR_SLOTS_)  devSetting->NoHourSlots  = _NO_HOUR_SLOTS_;
  if (devSetting->NoDaySlots   < _NO_DAY_SLOTS_)   devSetting->NoDaySlots   = _NO_DAY_SLOTS_;
  if (devSetting->NoMonthSlots < _NO_MONTH_SLOTS_) devSetting->NoMonthSlots = _NO_MONTH_SLOTS_;

  DebugTln("Start writing devSetting's ..");
  DebugTf("NoHourSlots [%3d]\r\n", devSetting->NoHourSlots);
  DebugTf("NoDaySlots  [%3d]\r\n", devSetting->NoDaySlots);
  DebugTf("NoMonthSlots[%3d]\r\n", devSetting->NoMonthSlots);
  //-- Allocate the JsonDocument
  SpiRamJsonDocument  doc(3000);

  //-- Fill JSON document from settings
  doc["hostname"]         = devSetting->Hostname;
  doc["indexPage"]        = devSetting->IndexPage;
  doc["dailyReboot"]      = devSetting->DailyReboot;
  doc["runAPmode"]        = devSetting->runAPmode;
  doc["noHourSlots"]      = devSetting->NoHourSlots;  //-- don't change directly
  doc["noDaySlots"]       = devSetting->NoDaySlots;   //-- don't change directly
  doc["noMonthSlots"]     = devSetting->NoMonthSlots; //-- don't change directly
  doc["oledType"]         = devSetting->OledType;
  doc["oledSleep"]        = devSetting->OledSleep;
  doc["oledFlip"]         = devSetting->OledFlip;
  doc["neoBrightness"]    = devSetting->NeoBrightness;
  doc["telegramInterval"] = devSetting->TelegramInterval;
  doc["mqttBroker"]       = devSetting->MQTTbroker;
  doc["mqttBrokerPort"]   = devSetting->MQTTbrokerPort;
  doc["mqttUser"]         = devSetting->MQTTuser;
  doc["mqttPassword"]     = devSetting->MQTTpasswd;
  doc["mqttInterval"]     = devSetting->MQTTinterval;
  doc["mqttTopTopic"]     = devSetting->MQTTtopTopic;

  //DebugTln("---------------------------------------------------");
  //serializeJsonPretty(doc, Serial);
  //Debugln();
  //DebugTln("---------------------------------------------------");
  // Serialize JSON to file
  bool success = serializeJsonPretty(doc, file) > 0;
  if (!success)
  {
    DebugTln("\r\nFailed to serialize and write devSetting's to file ");
  }

  file.close();
  
  if (devSetting->OledType > 2)          devSetting->OledType = 1;
  if (devSetting->OledFlip != 0)         devSetting->OledFlip = 1;
  else                                   devSetting->OledFlip = 0;
  if (devSetting->NeoBrightness <  10)   devSetting->NeoBrightness = 10;
  if (devSetting->NeoBrightness > 250)   devSetting->NeoBrightness = 250;
  if (devSetting->TelegramInterval  < 2) devSetting->TelegramInterval = 10;

  DebugTf("Change nextTelegram timer to [%d] seconds ..\r\n", devSetting->TelegramInterval);
  CHANGE_INTERVAL_SEC(nextTelegram,   devSetting->TelegramInterval);
  CHANGE_INTERVAL_MIN(oledSleepTimer, devSetting->OledSleep);

  if (show) { showDevSettings(); }

  Debugln("done ..");

} // writeDevSettings()


//=======================================================================
void readDevSettings(bool show)
{
  String sTmp, nColor;
  String words[10];

  File file;

  DebugTf(" %s ..\r\n", _SYSTEM_FILE);

  if (!_FSYS.exists(_SYSTEM_FILE))
  {
    DebugTln(F(" .. file not found! --> created file!"));
    strlcpy(devSetting->Hostname, _DEFAULT_HOSTNAME, (_HOSTNAME_LEN -1));
    strlcpy(devSetting->IndexPage, "DSMRindex.html", (_INDEXPAGE_LEN -1));
    devSetting->DailyReboot       = 0;
    devSetting->runAPmode         = 0;
    devSetting->OledType          = 1;
    devSetting->NeoBrightness     = 75;
    devSetting->TelegramInterval  = 10;
    devSetting->MQTTinterval      = 0;
    strlcpy(devSetting->MQTTtopTopic, _DEFAULT_HOSTNAME, (_MQTT_TOPTOPIC_LEN -1));
    writeDevSettings(false);
  }

  file = _FSYS.open(_SYSTEM_FILE, "r");
  if (!file)
  {
    DebugTf(" .. something went wrong opening [%s]\r\n", _SYSTEM_FILE);
    delay(100);
  }

  DebugTln("Reading Device Setting's\r");
  //-- Allocate the JsonDocument (size by trail and error)
  SpiRamJsonDocument  doc(3500);

  //-- Parse the JSON object in the file
  DeserializationError err = deserializeJson(doc, file);

  //-- This may fail if the JSON is invalid
  if (err)
  {
    DebugTln("Failed to deserialize devSetting's: ");
    Debugln(err.f_str());
    file.close();
    return;
  }

  file.close();

  serializeJsonPretty(doc, jsonBuff, _JSONBUFF_LEN);
  //-dbg-Debugln(jsonBuff);
  
  //-- Extract devSetting settings from the JSON document
  if (doc["hostname"])      { strlcpy(devSetting->Hostname,   doc["hostname"], (_HOSTNAME_LEN -1)); }
  if (doc["indexPage"])     { strlcpy(devSetting->IndexPage,  doc["indexPage"], (_INDEXPAGE_LEN -1)); }
  if (doc["dailyReboot"])   { devSetting->DailyReboot         = doc["dailyReboot"].as<int>(); }
  if (doc["runAPmode"])     { devSetting->runAPmode           = doc["runAPmode"].as<int>(); }
  if (doc["oledType"])      { devSetting->OledType            = doc["oledType"].as<int>(); }
  if (doc["oledSleep"])     { devSetting->OledSleep           = doc["oledSleep"].as<int>(); }
  if (doc["oledFlip"])      { devSetting->OledFlip            = doc["oledFlip"].as<int>(); }
  if (doc["neoBrightness"]) { devSetting->NeoBrightness       = doc["neoBrightness"].as<int>(); }
  if (doc["telegramInterval"]) { devSetting->TelegramInterval = doc["telegramInterval"].as<int>(); }
  if (doc["mqttBroker"])    { strlcpy(devSetting->MQTTbroker, doc["mqttBroker"] | "", (_MQTT_BROKER_LEN -1)); }
  if (doc["mqttBrokerPort"]) { devSetting->MQTTbrokerPort     = doc["mqttBrokerPort"].as<int>(); }
  if (doc["mqttUser"])      { strlcpy(devSetting->MQTTuser,   doc["mqttUser"] | "", (_MQTT_USER_LEN -1)); }
  if (doc["mqttPassword"])  { strlcpy(devSetting->MQTTpasswd, doc["mqttPassword"] | "", (_MQTT_PASSWD_LEN -1)); }
  if (doc["mqttInterval"])  { devSetting->MQTTinterval        = doc["mqttInterval"].as<int>(); }
  if (doc["mqttTopTopic"])  { strlcpy(devSetting->MQTTtopTopic, doc["mqttTopTopic"] | _DEFAULT_HOSTNAME, (_MQTT_TOPTOPIC_LEN -1)); }

  devSetting->NoHourSlots  = readRingHistoryDepth(HOURS_FILE,  RNG_HOURS);
  devSetting->NoDaySlots   = readRingHistoryDepth(DAYS_FILE,   RNG_DAYS);
  devSetting->NoMonthSlots = readRingHistoryDepth(MONTHS_FILE, RNG_MONTHS);

  //--- this will take some time to settle in
  //--- probably need a reboot before that to happen :-(
  startMDNS(devSetting->Hostname);


  if (devSetting->NeoBrightness ==  0) devSetting->NeoBrightness =  50;
  if (devSetting->NeoBrightness <  10) devSetting->NeoBrightness =  10;
  if (devSetting->NeoBrightness > 250) devSetting->NeoBrightness = 250;
  neoPixels.setBrightness(devSetting->NeoBrightness);  

  if (strlen(devSetting->IndexPage)    < 7) strlcpy(devSetting->IndexPage, "DSMRindex.html", (_INDEXPAGE_LEN -1));
  if (devSetting->TelegramInterval     < 2) devSetting->TelegramInterval = 10;
  DebugTf("Set nextTelegram timer to [%d] seconds\r\n", devSetting->TelegramInterval);
  CHANGE_INTERVAL_SEC(nextTelegram, devSetting->TelegramInterval)
  if (devSetting->MQTTbrokerPort       < 1) devSetting->MQTTbrokerPort   = 1883;
  if (strlen(devSetting->MQTTtopTopic) < 2) strlcpy(devSetting->MQTTtopTopic, _DEFAULT_HOSTNAME, _MQTT_TOPTOPIC_LEN);

  if (show) { showDevSettings(); }

  DebugTln(F(" .. done\r"));

} // readDevSettings()


//=======================================================================
void showDevSettings()
{
    Debugln("\r\n==== System settings ============================================\r");
    Debugf("                    Hostname : %s\r\n",     devSetting->Hostname);
    Debugf("                  Index Page : %s\r\n",     devSetting->IndexPage);
    Debugf("  Daily Reboot (0=Nee, 1=Ja) : %s\r\n",     devSetting->DailyReboot ? "Ja":"Nee");
    Debugf("    run als AP (0=Nee, 1=Ja) : %s\r\n",     devSetting->runAPmode ? "Ja":"Nee");
    Debugf("               Hours History : %d\r\n",     devSetting->NoHourSlots);
    Debugf("                Days History : %d\r\n",     devSetting->NoDaySlots);
    Debugf("              Months History : %d\r\n",     devSetting->NoMonthSlots);
    Debugf("   Telegram Process Interval : %d\r\n",     devSetting->TelegramInterval);
    Debugf("         OLED Type (0, 1, 2) : %d\r\n",     devSetting->OledType);
    Debugf("OLED Sleep Min. (0=oneindig) : %d\r\n",     devSetting->OledSleep);
    Debugf("     Flip Oled (0=No, 1=Yes) : %d\r\n",     devSetting->OledFlip);
    Debugf("         NeoPixel Brightness : %d\r\n",     devSetting->NeoBrightness);

    Debugln(F("\r\n==== MQTT settings ==============================================\r"));
    Debugf("          MQTT broker URL/IP : %s:%d", devSetting->MQTTbroker, devSetting->MQTTbrokerPort);
    if (MQTTclient.connected()) Debugln(F(" (is Connected!)\r"));
    else                        Debugln(F(" (NOT Connected!)\r"));
    Debugf("                   MQTT user : %s\r\n", devSetting->MQTTuser);
#ifdef _SHOW_PASSWRDS
    Debugf("               MQTT password : %s\r\n", devSetting->MQTTpasswd);
#else
    Debug( "               MQTT password : *************\r\n");
#endif
    Debugf("          MQTT send Interval : %d\r\n", devSetting->MQTTinterval);
    Debugf("              MQTT top Topic : %s\r\n", devSetting->MQTTtopTopic);

    Debugln("-\r");

} //  showDevSettings()


//=======================================================================
void updateDevSettings(const char *field, const char *newValue)
{
  DebugTf("-> field[%s], newValue[%s]\r\n", field, newValue);

  if (!strcasecmp(field, "hostname"))
  {
    strlcpy(devSetting->Hostname, newValue, 29);
    if (strlen(devSetting->Hostname) < 1) strlcpy(devSetting->Hostname, _DEFAULT_HOSTNAME, 29);
    char *dotPntr = strchr(devSetting->Hostname, '.') ;
    if (dotPntr != NULL)
    {
      byte dotPos = (dotPntr-devSetting->Hostname);
      if (dotPos > 0)  devSetting->Hostname[dotPos] = '\0';
    }
    startMDNS(devSetting->Hostname);
    Debugln();
    DebugTf("Need reboot before new %s.local will be available!??\r\n\n", devSetting->Hostname);
  }

  if (!strcasecmp(field, "index_page"))        strlcpy(devSetting->IndexPage, newValue, (sizeof(devSetting->IndexPage) -1));
  if (!strcasecmp(field, "daily_reboot"))      devSetting->DailyReboot  = String(newValue).toInt();
  if (!strcasecmp(field, "run_as_ap"))         devSetting->runAPmode    = String(newValue).toInt();
  //-- don't change the devSettings .. yet!
  if (!strcasecmp(field, "no_hour_slots"))     tmpNoHourSlots  = String(newValue).toInt();
  if (!strcasecmp(field, "no_day_slots"))      tmpNoDaySlots   = String(newValue).toInt();
  if (!strcasecmp(field, "no_month_slots"))    tmpNoMonthSlots = String(newValue).toInt();

  if (!strcasecmp(field, "alter_ring_slots")) 
  {
    tmpAlterRingSlots = String(newValue).toInt();
    if (tmpAlterRingSlots == 1)
    {
      tmpAlterRingSlots = 0;
      alterRingFile();
    }
  }
  
  if (!strcasecmp(field, "oled_type"))
  {
    devSetting->OledType     = String(newValue).toInt();
    if (devSetting->OledType > 2)  devSetting->OledType = 1;
    oled_Init();
  }
  if (!strcasecmp(field, "oled_screen_time"))
  {
    devSetting->OledSleep    = String(newValue).toInt();
    CHANGE_INTERVAL_MIN(oledSleepTimer, devSetting->OledSleep)
  }
  if (!strcasecmp(field, "oled_flip_screen"))
  {
    devSetting->OledFlip     = String(newValue).toInt();
    if (devSetting->OledFlip != 0) devSetting->OledFlip = 1;
    else                           devSetting->OledFlip = 0;
    oled_Init();
  }
  if (!strcasecmp(field, "neo_brightness"))
  {
    devSetting->NeoBrightness = String(newValue).toInt();
    if (devSetting->NeoBrightness <  10) devSetting->NeoBrightness =  10;
    if (devSetting->NeoBrightness > 250) devSetting->NeoBrightness = 250;
    neoPixels.setBrightness(devSetting->NeoBrightness);  
  }
  if (!strcasecmp(field, "tlgrm_interval"))
  {
    devSetting->TelegramInterval     = String(newValue).toInt();
    DebugTf("Change nextTelegram timer to [%d] seconds\r\n", devSetting->TelegramInterval);
    CHANGE_INTERVAL_SEC(nextTelegram, devSetting->TelegramInterval)
  }

  if (!strcasecmp(field, "mqtt_broker"))
  {
    DebugT("devSetting->MQTTbroker! to : ");
    memset(devSetting->MQTTbroker, '\0', sizeof(devSetting->MQTTbroker));
    strlcpy(devSetting->MQTTbroker, newValue, 100);
    Debugf("[%s]\r\n", devSetting->MQTTbroker);
    mqttIsConnected = false;
    CHANGE_INTERVAL_MS(reconnectMQTTtimer, 100); // try reconnecting cyclus timer
  }
  if (!strcasecmp(field, "mqtt_broker_port"))
  {
    devSetting->MQTTbrokerPort = String(newValue).toInt();
    mqttIsConnected = false;
    CHANGE_INTERVAL_MS(reconnectMQTTtimer, 100); // try reconnecting cyclus timer
  }
  if (!strcasecmp(field, "mqtt_user"))
  {
    strlcpy(devSetting->MQTTuser, newValue, 35);
    mqttIsConnected = false;
    CHANGE_INTERVAL_MS(reconnectMQTTtimer, 100); // try reconnecting cyclus timer
  }
  if (!strcasecmp(field, "mqtt_passwd"))
  {
    strlcpy(devSetting->MQTTpasswd, newValue, 35);
    mqttIsConnected = false;
    CHANGE_INTERVAL_MS(reconnectMQTTtimer, 100); // try reconnecting cyclus timer
  }
  if (!strcasecmp(field, "mqtt_interval"))
  {
    devSetting->MQTTinterval   = String(newValue).toInt();
    CHANGE_INTERVAL_SEC(publishMQTTtimer, devSetting->MQTTinterval);
  }
  if (!strcasecmp(field, "mqtt_toptopic"))     strlcpy(devSetting->MQTTtopTopic, newValue, 20);

  writeDevSettings(false);

} // updateDevSettings()

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