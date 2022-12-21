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

//=======================================================================
void writeSmSettings()
{
  yield();
  DebugT(F("Writing to ["));
  Debug(_SETTINGS_FILE);
  Debugln(F("] ..."));
  File file = _FSYS.open(_SETTINGS_FILE, "w"); // open for reading and writing
  if (!file)
  {
    DebugTf("open(%s, 'w') FAILED!!! --> Bailout\r\n", _SETTINGS_FILE);
    return;
  }
  yield();

  if (sysSetting->TelegramInterval < 2)  sysSetting->TelegramInterval = 10;

  DebugTln(F("Start writing setting data .."));

  //-- Allocate the JsonDocument
  SpiRamJsonDocument  doc(3000);

  //-- Fill JSON document from settings
  doc["preDSMR40"]          = setting->PreDSMR40;
  doc["EnergyDeliveredT1"]  = String(setting->EDT1, 5);
  doc["EnergyDeliveredT2"]  = String(setting->EDT2, 5);
  doc["EnergyReturnedT1"]   = String(setting->ERT1, 5);
  doc["EnergyReturnedT2"]   = String(setting->ERT2, 5);
  doc["mBus1Type"]          = setting->Mbus1Type;
  doc["mBus2Type"]          = setting->Mbus2Type;
  doc["mBus3Type"]          = setting->Mbus3Type;
  doc["mBus4Type"]          = setting->Mbus4Type;
  doc["GasDeliveredT"]      = String(setting->GDT,  5);
  doc["EnergyVasteKosten"]  = String(setting->ENBK, 2);
  doc["GasVasteKosten"]     = String(setting->GNBK, 2);
  doc["SmHasFaseInfo"]      = setting->SmHasFaseInfo;

  //DebugTln("---------------------------------------------------");
  //serializeJsonPretty(doc, Serial);
  //Debugln();
  //DebugTln("---------------------------------------------------");
  // Serialize JSON to file
  bool success = serializeJsonPretty(doc, file) > 0;
  if (!success)
  {
    DebugTln("Failed to serialize and write sysSetting settings to file");
  }

  file.close();

  Debugln(F(" done"));
  if (Verbose1)
  {
    DebugTln(F("Wrote this:"));
    DebugT(F("EnergyDeliveredT1 = "));
    Debugln(String(setting->EDT1, 5));
    DebugT(F("EnergyDeliveredT2 = "));
    Debugln(String(setting->EDT2, 5));
    DebugT(F("EnergyReturnedT1 = "));
    Debugln(String(setting->ERT1, 5));
    DebugT(F("EnergyReturnedT2 = "));
    Debugln(String(setting->ERT2, 5));
    DebugT(F("mBus1Type = "));
    Debugln(setting->Mbus1Type);
    DebugT(F("mBus2Type = "));
    Debugln(setting->Mbus2Type);
    DebugT(F("mBus3Type = "));
    Debugln(setting->Mbus3Type);
    DebugT(F("mBus4Type = "));
    Debugln(setting->Mbus4Type);
    DebugT(F("GasDeliveredT = "));
    Debugln(String(setting->GDT,  5));
    DebugT(F("EnergyVasteKosten = "));
    Debugln(String(setting->ENBK, 2));
    DebugT(F("GasVasteKosten = "));
    Debugln(String(setting->GNBK, 2));
    DebugT(F("OledType = "));
    if (sysSetting->OledType == 1)      Debugln("SDD1306");
    else if (sysSetting->OledType == 2) Debugln("SH1306");
    else                             Debugln("None");
    DebugT(F("OledSleep = "));
    Debugln(sysSetting->OledSleep);
    DebugT(F("OledFlip = "));
    if (sysSetting->OledFlip)  Debugln(F("Yes"));
    else                    Debugln(F("No"));

    DebugT(F("SmHasFaseInfo"));
    if (setting->SmHasFaseInfo == 1) Debugln("Yes");
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

  setting->EDT1               = 0.778;
  setting->EDT2               = 0.678;
  setting->ERT1               = 0.338;
  setting->ERT2               = 0.438;
  setting->GDT                = 1.8712;
  setting->ENBK               = 19.15;
  setting->GNBK               = 18.11;

  if (!_FSYS.exists(_SETTINGS_FILE))
  {
    DebugTln(F(" .. file not found! --> created file!"));
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
  setting->PreDSMR40      = doc["preDSMR40"].as<int>();
  setting->EDT1           = doc["EnergyDeliveredT1"].as<float>();
  setting->EDT2           = doc["EnergyDeliveredT2"].as<float>();
  setting->ERT1           = doc["EnergyReturnedT1"].as<float>();
  setting->ERT2           = doc["EnergyReturnedT2"].as<float>();
  setting->Mbus1Type      = doc["mBus1Type"].as<int>();
  setting->Mbus2Type      = doc["mBus2Type"].as<int>();
  setting->Mbus3Type      = doc["mBus3Type"].as<int>();
  setting->Mbus4Type      = doc["mBus4Type"].as<int>();
  setting->GDT            = doc["GasDeliveredT"].as<float>();
  setting->ENBK           = doc["EnergyVasteKosten"].as<float>();
  setting->GNBK           = doc["GasVasteKosten"].as<float>();
  setting->SmHasFaseInfo  = doc["SmHasFaseInfo"].as<int>();

  DebugTln(F(" .. done\r"));

  if (setting->SmHasFaseInfo != 0)  setting->SmHasFaseInfo = 1;
  else                              setting->SmHasFaseInfo = 0;

  if (!show) return;

  Debugln(F("\r\n==== Settings ===================================================\r"));
  Debugf("   Pre DSMR 40 (0=No, 1=Yes) : %s\r\n",     setting->PreDSMR40 ? "Yes":"No");
  Debugf("   Energy Delivered Tarief 1 : %9.7f\r\n",  setting->EDT1);
  Debugf("   Energy Delivered Tarief 2 : %9.7f\r\n",  setting->EDT2);
  Debugf("   Energy Delivered Tarief 1 : %9.7f\r\n",  setting->ERT1);
  Debugf("   Energy Delivered Tarief 2 : %9.7f\r\n",  setting->ERT2);
  Debugf("        Gas Delivered Tarief : %9.7f\r\n",  setting->GDT);
  Debugf("     Energy Netbeheer Kosten : %9.2f\r\n",  setting->ENBK);
  Debugf("        Gas Netbeheer Kosten : %9.2f\r\n",  setting->GNBK);
  Debugf("                 MBus 1 Type : %d\r\n",     setting->Mbus1Type);
  Debugf("                 MBus 2 Type : %d\r\n",     setting->Mbus2Type);
  Debugf("                 MBus 3 Type : %d\r\n",     setting->Mbus3Type);
  Debugf("                 MBus 4 Type : %d\r\n",     setting->Mbus4Type);
  Debugf("  SM Fase Info (0=No, 1=Yes) : %s\r\n",     setting->SmHasFaseInfo ? "Yes":"No");

  Debugln(F("-\r"));

} // readSmSettings()


//=======================================================================
void updateSmSettings(const char *field, const char *newValue)
{
  DebugTf("-> field[%s], newValue[%s]\r\n", field, newValue);

  //if (!stricmp(field, "pre_DSMR40"))        setting->PreDSMR40    = String(newValue).toInt();
  if (!strcasecmp(field, "pre_DSMR40"))     setting->PreDSMR40    = String(newValue).toInt();
  if (!strcasecmp(field, "ed_tariff1"))        setting->EDT1         = String(newValue).toFloat();
  if (!strcasecmp(field, "ed_tariff2"))        setting->EDT2         = String(newValue).toFloat();
  if (!strcasecmp(field, "er_tariff1"))        setting->ERT1         = String(newValue).toFloat();
  if (!strcasecmp(field, "er_tariff2"))        setting->ERT2         = String(newValue).toFloat();
  if (!strcasecmp(field, "electr_netw_costs")) setting->ENBK         = String(newValue).toFloat();

  if (!strcasecmp(field, "mbus1_type"))        setting->Mbus1Type    = String(newValue).toInt();
  if (!strcasecmp(field, "mbus2_type"))        setting->Mbus2Type    = String(newValue).toInt();
  if (!strcasecmp(field, "mbus3_type"))        setting->Mbus3Type    = String(newValue).toInt();
  if (!strcasecmp(field, "mbus4_type"))        setting->Mbus4Type    = String(newValue).toInt();
  if (!strcasecmp(field, "gd_tariff"))         setting->GDT          = String(newValue).toFloat();
  if (!strcasecmp(field, "gas_netw_costs"))    setting->GNBK         = String(newValue).toFloat();

  if (!strcasecmp(field, "sm_has_fase_info"))
  {
    setting->SmHasFaseInfo = String(newValue).toInt();
    if (setting->SmHasFaseInfo != 0)  setting->SmHasFaseInfo = 1;
    else                              setting->SmHasFaseInfo = 0;
  }
  writeSmSettings();

} // updateSmSettings()


//=======================================================================
void writeSysSettings(bool show)
{
  yield();
  DebugT(F("Writing to ["));
  Debug(_SYSTEM_FILE);
  Debugln(F("] ..."));
  File file = _FSYS.open(_SYSTEM_FILE, "w"); // open for reading and writing
  if (!file)
  {
    DebugTf("open(%s, 'w') FAILED!!! --> Bailout\r\n", _SYSTEM_FILE);
    return;
  }
  yield();

  if (strlen(sysSetting->IndexPage) < 7) strlcpy(sysSetting->IndexPage, "DSMRindex.html", (_INDEXPAGE_LEN -1));
  if (sysSetting->MQTTbrokerPort < 1)    sysSetting->MQTTbrokerPort = 1883;
  if (sysSetting->NoHourSlots  < _NO_HOUR_SLOTS_)  sysSetting->NoHourSlots  = _NO_HOUR_SLOTS_;
  if (sysSetting->NoDaySlots   < _NO_DAY_SLOTS_)   sysSetting->NoDaySlots   = _NO_DAY_SLOTS_;
  if (sysSetting->NoMonthSlots < _NO_MONTH_SLOTS_) sysSetting->NoMonthSlots = _NO_MONTH_SLOTS_;

  DebugTln("Start writing sysSetting's ..");
  DebugTf("NoHourSlots [%3d]\r\n", sysSetting->NoHourSlots);
  DebugTf("NoDaySlots  [%3d]\r\n", sysSetting->NoDaySlots);
  DebugTf("NoMonthSlots[%3d]\r\n", sysSetting->NoMonthSlots);
  //-- Allocate the JsonDocument
  SpiRamJsonDocument  doc(3000);

  //-- Fill JSON document from settings
  doc["hostname"]         = sysSetting->Hostname;
  doc["indexPage"]        = sysSetting->IndexPage;
  doc["dailyReboot"]      = sysSetting->DailyReboot;
  doc["noHourSlots"]      = sysSetting->NoHourSlots;  //-- don't change directly
  doc["noDaySlots"]       = sysSetting->NoDaySlots;   //-- don't change directly
  doc["noMonthSlots"]     = sysSetting->NoMonthSlots; //-- don't change directly
  doc["oledType"]         = sysSetting->OledType;
  doc["oledSleep"]        = sysSetting->OledSleep;
  doc["oledFlip"]         = sysSetting->OledFlip;
  doc["neoBrightness"]    = sysSetting->NeoBrightness;
  doc["telegramInterval"] = sysSetting->TelegramInterval;
  doc["mqttBroker"]       = sysSetting->MQTTbroker;
  doc["mqttBrokerPort"]   = sysSetting->MQTTbrokerPort;
  doc["mqttUser"]         = sysSetting->MQTTuser;
  doc["mqttPassword"]     = sysSetting->MQTTpasswd;
  doc["mqttInterval"]     = sysSetting->MQTTinterval;
  doc["mqttTopTopic"]     = sysSetting->MQTTtopTopic;

  //DebugTln("---------------------------------------------------");
  //serializeJsonPretty(doc, Serial);
  //Debugln();
  //DebugTln("---------------------------------------------------");
  // Serialize JSON to file
  bool success = serializeJsonPretty(doc, file) > 0;
  if (!success)
  {
    DebugTln("\r\nFailed to serialize and write sysSetting's to file ");
  }

  file.close();
  
  if (sysSetting->OledType > 2)          sysSetting->OledType = 1;
  if (sysSetting->OledFlip != 0)         sysSetting->OledFlip = 1;
  else                                   sysSetting->OledFlip = 0;
  if (sysSetting->NeoBrightness <  10)   sysSetting->NeoBrightness = 10;
  if (sysSetting->NeoBrightness > 250)   sysSetting->NeoBrightness = 250;
  if (sysSetting->TelegramInterval  < 2) sysSetting->TelegramInterval = 10;

  DebugTf("Change nextTelegram timer to [%d] seconds ..\r\n", sysSetting->TelegramInterval);
  CHANGE_INTERVAL_SEC(nextTelegram,   sysSetting->TelegramInterval);
  CHANGE_INTERVAL_MIN(oledSleepTimer, sysSetting->OledSleep);

  if (show) { showSysSettings(); }

  Debugln("done ..");

} // writeSysSettings()


//=======================================================================
void readSysSettings(bool show)
{
  String sTmp, nColor;
  String words[10];

  File file;

  DebugTf(" %s ..\r\n", _SYSTEM_FILE);

  if (!_FSYS.exists(_SYSTEM_FILE))
  {
    DebugTln(F(" .. file not found! --> created file!"));
    writeSysSettings(false);
  }

  file = _FSYS.open(_SYSTEM_FILE, "r");
  if (!file)
  {
    DebugTf(" .. something went wrong opening [%s]\r\n", _SYSTEM_FILE);
    delay(100);
  }

  DebugTln("Reading SysSetting's\r");
  //-- Allocate the JsonDocument (size by trail and error)
  SpiRamJsonDocument  doc(3500);

  //-- Parse the JSON object in the file
  DeserializationError err = deserializeJson(doc, file);

  //-- This may fail if the JSON is invalid
  if (err)
  {
    DebugTln("Failed to deserialize sysSetting's: ");
    Debugln(err.f_str());
    file.close();
    return;
  }

  file.close();

  //serializeJsonPretty(doc, jsonBuff, _JSONBUFF_LEN);
  //Debugln(jsonBuff);

  //-- Extract sysSetting settings from the JSON document
  strlcpy(sysSetting->Hostname,   doc["hostname"] | _DEFAULT_HOSTNAME, _HOSTNAME_LEN);
  strlcpy(sysSetting->IndexPage,  doc["indexPage"] | "DSMRindex.html", _INDEXPAGE_LEN);
  sysSetting->DailyReboot       = doc["dailyReboot"].as<int>();
  sysSetting->OledType          = doc["oledType"].as<int>();
  sysSetting->OledSleep         = doc["oledSleep"].as<int>();
  sysSetting->OledFlip          = doc["oledFlip"].as<int>();
  sysSetting->NeoBrightness     = doc["neoBrightness"].as<int>();
  sysSetting->TelegramInterval  = doc["telegramInterval"].as<int>();
  strlcpy(sysSetting->MQTTbroker, doc["mqttBroker"] | "", _MQTT_BROKER_LEN);
  sysSetting->MQTTbrokerPort    = doc["mqttBrokerPort"].as<int>();
  strlcpy(sysSetting->MQTTuser,   doc["mqttUser"] | "", _MQTT_USER_LEN);
  strlcpy(sysSetting->MQTTpasswd, doc["mqttPassword"] | "", _MQTT_PASSWD_LEN);
  sysSetting->MQTTinterval      = doc["mqttInterval"].as<int>();
  strlcpy(sysSetting->MQTTtopTopic, doc["mqttTopTopic"] | _DEFAULT_HOSTNAME, _MQTT_TOPTOPIC_LEN);

  sysSetting->NoHourSlots  = readRingHistoryDepth(HOURS_FILE,  RNG_HOURS);
  sysSetting->NoDaySlots   = readRingHistoryDepth(DAYS_FILE,   RNG_DAYS);
  sysSetting->NoMonthSlots = readRingHistoryDepth(MONTHS_FILE, RNG_MONTHS);

  //--- this will take some time to settle in
  //--- probably need a reboot before that to happen :-(
  MDNS.begin(sysSetting->Hostname);    // start advertising with new(?) sysSetting->Hostname

  if (sysSetting->NeoBrightness <  10) sysSetting->NeoBrightness =  10;
  if (sysSetting->NeoBrightness > 250) sysSetting->NeoBrightness = 250;
  neoPixels.setBrightness(sysSetting->NeoBrightness);  

  if (strlen(sysSetting->IndexPage)    < 7) strlcpy(sysSetting->IndexPage, "DSMRindex.html", (_INDEXPAGE_LEN -1));
  if (sysSetting->TelegramInterval     < 2) sysSetting->TelegramInterval = 10;
  DebugTf("Set nextTelegram timer to [%d] seconds\r\n", sysSetting->TelegramInterval);
  CHANGE_INTERVAL_SEC(nextTelegram, sysSetting->TelegramInterval)
  if (sysSetting->MQTTbrokerPort       < 1) sysSetting->MQTTbrokerPort   = 1883;
  if (strlen(sysSetting->MQTTtopTopic) < 2) strlcpy(sysSetting->MQTTtopTopic, _DEFAULT_HOSTNAME, _MQTT_TOPTOPIC_LEN);

  if (show) { showSysSettings(); }

  DebugTln(F(" .. done\r"));

} // readSysSettings()


//=======================================================================
void showSysSettings()
{
    Debugln("\r\n==== System ====================================================\r");
    Debugf("                    Hostname : %s\r\n",     sysSetting->Hostname);
    Debugf("                  Index Page : %s\r\n",     sysSetting->IndexPage);
    Debugf("  Daily Reboot (0=No, 1=Yes) : %s\r\n",     sysSetting->DailyReboot ? "Yes":"No");
    Debugf("               Hours History : %d\r\n",     sysSetting->NoHourSlots);
    Debugf("                Days History : %d\r\n",     sysSetting->NoDaySlots);
    Debugf("              Months History : %d\r\n",     sysSetting->NoMonthSlots);
    Debugf("   Telegram Process Interval : %d\r\n",     sysSetting->TelegramInterval);
    Debugf("         OLED Type (0, 1, 2) : %d\r\n",     sysSetting->OledType);
    Debugf("OLED Sleep Min. (0=oneindig) : %d\r\n",     sysSetting->OledSleep);
    Debugf("     Flip Oled (0=No, 1=Yes) : %d\r\n",     sysSetting->OledFlip);
    Debugf("         NeoPixel Brightness : %d\r\n",     sysSetting->NeoBrightness);

    Debugln(F("\r\n==== MQTT settings ==============================================\r"));
    Debugf("          MQTT broker URL/IP : %s:%d", sysSetting->MQTTbroker, sysSetting->MQTTbrokerPort);
    if (MQTTclient.connected()) Debugln(F(" (is Connected!)\r"));
    else                        Debugln(F(" (NOT Connected!)\r"));
    Debugf("                   MQTT user : %s\r\n", sysSetting->MQTTuser);
#ifdef _SHOW_PASSWRDS
    Debugf("               MQTT password : %s\r\n", sysSetting->MQTTpasswd);
#else
    Debug( "               MQTT password : *************\r\n");
#endif
    Debugf("          MQTT send Interval : %d\r\n", sysSetting->MQTTinterval);
    Debugf("              MQTT top Topic : %s\r\n", sysSetting->MQTTtopTopic);

    Debugln("-\r");

} //  showSysSettings()


//=======================================================================
void updateSysSettings(const char *field, const char *newValue)
{
  DebugTf("-> field[%s], newValue[%s]\r\n", field, newValue);

  if (!strcasecmp(field, "hostname"))
  {
    strlcpy(sysSetting->Hostname, newValue, 29);
    if (strlen(sysSetting->Hostname) < 1) strlcpy(sysSetting->Hostname, _DEFAULT_HOSTNAME, 29);
    char *dotPntr = strchr(sysSetting->Hostname, '.') ;
    if (dotPntr != NULL)
    {
      byte dotPos = (dotPntr-sysSetting->Hostname);
      if (dotPos > 0)  sysSetting->Hostname[dotPos] = '\0';
    }
    Debugln();
    DebugTf("Need reboot before new %s.local will be available!\r\n\n", sysSetting->Hostname);
  }

  if (!strcasecmp(field, "index_page"))        strlcpy(sysSetting->IndexPage, newValue, (sizeof(sysSetting->IndexPage) -1));
  if (!strcasecmp(field, "daily_reboot"))      sysSetting->DailyReboot  = String(newValue).toInt();
  //-- don't change the sysSettings .. yet!
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
    sysSetting->OledType     = String(newValue).toInt();
    if (sysSetting->OledType > 2)  sysSetting->OledType = 1;
    oled_Init();
  }
  if (!strcasecmp(field, "oled_screen_time"))
  {
    sysSetting->OledSleep    = String(newValue).toInt();
    CHANGE_INTERVAL_MIN(oledSleepTimer, sysSetting->OledSleep)
  }
  if (!strcasecmp(field, "oled_flip_screen"))
  {
    sysSetting->OledFlip     = String(newValue).toInt();
    if (sysSetting->OledFlip != 0) sysSetting->OledFlip = 1;
    else                           sysSetting->OledFlip = 0;
    oled_Init();
  }
  if (!strcasecmp(field, "neo_brightness"))
  {
    sysSetting->NeoBrightness = String(newValue).toInt();
    if (sysSetting->NeoBrightness <  10) sysSetting->NeoBrightness =  10;
    if (sysSetting->NeoBrightness > 250) sysSetting->NeoBrightness = 250;
    neoPixels.setBrightness(sysSetting->NeoBrightness);  
  }
  if (!strcasecmp(field, "tlgrm_interval"))
  {
    sysSetting->TelegramInterval     = String(newValue).toInt();
    DebugTf("Change nextTelegram timer to [%d] seconds\r\n", sysSetting->TelegramInterval);
    CHANGE_INTERVAL_SEC(nextTelegram, sysSetting->TelegramInterval)
  }

  if (!strcasecmp(field, "mqtt_broker"))
  {
    DebugT("sysSetting->MQTTbroker! to : ");
    memset(sysSetting->MQTTbroker, '\0', sizeof(sysSetting->MQTTbroker));
    strlcpy(sysSetting->MQTTbroker, newValue, 100);
    Debugf("[%s]\r\n", sysSetting->MQTTbroker);
    mqttIsConnected = false;
    CHANGE_INTERVAL_MS(reconnectMQTTtimer, 100); // try reconnecting cyclus timer
  }
  if (!strcasecmp(field, "mqtt_broker_port"))
  {
    sysSetting->MQTTbrokerPort = String(newValue).toInt();
    mqttIsConnected = false;
    CHANGE_INTERVAL_MS(reconnectMQTTtimer, 100); // try reconnecting cyclus timer
  }
  if (!strcasecmp(field, "mqtt_user"))
  {
    strlcpy(sysSetting->MQTTuser, newValue, 35);
    mqttIsConnected = false;
    CHANGE_INTERVAL_MS(reconnectMQTTtimer, 100); // try reconnecting cyclus timer
  }
  if (!strcasecmp(field, "mqtt_passwd"))
  {
    strlcpy(sysSetting->MQTTpasswd, newValue, 35);
    mqttIsConnected = false;
    CHANGE_INTERVAL_MS(reconnectMQTTtimer, 100); // try reconnecting cyclus timer
  }
  if (!strcasecmp(field, "mqtt_interval"))
  {
    sysSetting->MQTTinterval   = String(newValue).toInt();
    CHANGE_INTERVAL_SEC(publishMQTTtimer, sysSetting->MQTTinterval);
  }
  if (!strcasecmp(field, "mqtt_toptopic"))     strlcpy(sysSetting->MQTTtopTopic, newValue, 20);

  writeSysSettings(false);

} // updateSysSettings()

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
