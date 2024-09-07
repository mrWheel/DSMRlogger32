#ifndef DSMRLOGGER32_H
#define DSMRLOGGER32_H



/*
***************************************************************************
**  Program  : DSMRlogger32.h - definitions for DSMRlogger32
**  Version  : v5.n
**
**  Copyright (c) 2021, 2023 Willem Aandewiel
**
**  TERMS OF USE: MIT License. See bottom of file.
***************************************************************************
*/
//============ Includes ====================
#include "oledStuff.h"
#include "handleTestdata.h"
#include "FSmanager.h"
#include "handleSlimmeMeter.h"
#include "FSYSstuff.h"
#include "networkStuff.h"
#include "processTelegram.h"
#include "DSMRsetupStuff.h"
#include "menuStuff.h"
#include "settingsStuff.h"
#include "MQTTstuff.h"
#include "restAPI.h"
#include "timeStuff.h"
#include "helperStuff.h"
#include "wifiEvents.h"

#include "arduinoGlue.h"

//--- sanaty check ---
#if !defined(_LITTLEFS) && !defined(_SPIFFS)
  #error "We need one of these: LITTLEFS or SPIFFS !"
#endif
#if defined(_LITTLEFS) && defined(_SPIFFS)
  #error "Only one of these: LITTLEFS or SPIFFS can be used!"
#endif

//------[FS@2.0.0]
//#include <FS.h>                                   		//-- moved to arduinoGlue.h

#ifdef _SPIFFS
	//-- moved to arduinoGlue.h //   #define _FSYS SPIFFS
  //------[SPIFFS@2.0.0] // part of ESP32 Core https://github.com/ESP32/Arduino
  //------[SPIFFS_SysLogger-2.0.1@2.0.1]
//#include <SPIFFS_SysLogger.h>                     		//-- moved to arduinoGlue.h
#endif
#ifdef _LITTLEFS
	//-- moved to arduinoGlue.h //   #define _FSYS LittleFS
//#include <LittleFS.h>                             		//-- moved to arduinoGlue.h
//#include <LittleFS_SysLogger.h>                   		//-- moved to arduinoGlue.h
#endif

ESPSL     sysLog;

/* example of debug info with time information ----------------------------------------------*/
	//-- moved to arduinoGlue.h // #define writeToSysLog(...) ({ \
	//-- moved to arduinoGlue.h //         sysLog.writeDbg(sysLog.buildD("[%04d-%02d-%02d %02d:%02d:%02d][%-12.12s] "  \
	//-- moved to arduinoGlue.h //                                                                , tzEurope.year()          \
	//-- moved to arduinoGlue.h //                                                                , tzEurope.month()         \
	//-- moved to arduinoGlue.h //                                                                , tzEurope.day()           \
	//-- moved to arduinoGlue.h //                                                                , tzEurope.hour()          \
	//-- moved to arduinoGlue.h //                                                                , tzEurope.minute()        \
	//-- moved to arduinoGlue.h //                                                                , tzEurope.second()        \
	//-- moved to arduinoGlue.h //                                                                , __FUNCTION__)            \
	//-- moved to arduinoGlue.h //                                          ,__VA_ARGS__); \
	//-- moved to arduinoGlue.h //                              })
	//-- moved to arduinoGlue.h // #define _SYSLOG_LINES       150
	//-- moved to arduinoGlue.h // #define _SYSLOG_LINE_LEN    120

//#include <HardwareSerial.h>                       		//-- moved to arduinoGlue.h
//#include <esp32/rom/rtc.h>
//#include <rom/rtc.h>                              		//-- moved to arduinoGlue.h

//-- use UART1 for SMserial
HardwareSerial SMserial (1);
//------ [WiFi@1.2.7 WiFi@2.0.0]
//#include <WiFi.h>                                 		//-- moved to arduinoGlue.h
//------ [WebServer@2.0.0]  - part of ESP32 Core https://github.com/ESP32/Arduino
//#include <WebServer.h>                            		//-- moved to arduinoGlue.h
//------ [ESPmDNS@2.0.0]    - part of ESP32 Core https://github.com/ESP32/Arduino
//#include <ESPmDNS.h>                              		//-- moved to arduinoGlue.h

//#include <WiFiUdp.h>                              		//-- moved to arduinoGlue.h
//------ [HTTPUpdateServer@2.0.0] - part of ESP32 Core https://github.com/ESP32/Arduino
//#include <HTTPUpdateServer.h>                     		//-- moved to arduinoGlue.h
//------ [WiFiManager@2.0.10-beta] ([DNSServer@2.0.0])
//#include <WiFiManager.h>                          		//-- moved to arduinoGlue.h

// included in main program: #include <TelnetStream.h>       // Version 0.0.1 - https://github.com/jandrassy/TelnetStream

WebServer        httpServer (80);
HTTPUpdateServer httpUpdater(true);

//------ [ezTime@0.8.3]
	//-- moved to arduinoGlue.h // #define AMSTERDAM_POSIX  "CET-1CEST,M3.5.0,M10.5.0/3"    // Time in Amsterdam
//#include <ezTime.h>                               		//-- moved to arduinoGlue.h
//Timezone    tzEurope;

//-- https://github.com/jandrassy/TelnetStream
//------ [TelnetStream@1.2.2]
//#include <TelnetStream.h>                         		//-- moved to arduinoGlue.h
#include "safeTimers.h"

//------ [Adafruit_NeoPixel@1.10.6]
#include "neoPixelStuff.h"

//------ [ArduinoJson@6.19.3]
//#include <ArduinoJson.h>                          		//-- moved to arduinoGlue.h

//-- convenience macros to convert to and from tm years 
//-- from: https://github.com/kachok/arduino-libraries/blob/master/Time/Time.h
	//-- moved to arduinoGlue.h // #define  tmYearToCalendar(Y) ((Y) + 1970)  // full four digit year 
	//-- moved to arduinoGlue.h // #define  CalendarYrToTm(Y)   ((Y) - 1970)

//-- https://github.com/mrWheel/dsmr2Lib.git
//------ [dsmr2Lib-master@0.1] (commit 5e7f07d (16-02-2022 12:40)?)
//#include <dsmr2.h>                                		//-- moved to arduinoGlue.h
	//-- moved to arduinoGlue.h // #define _DEFAULT_HOSTNAME  "DSMR-ESP32"

//-- Slimme Meter UART1 pins
	//-- moved to arduinoGlue.h // #define SMRX                    18 
	//-- moved to arduinoGlue.h // #define SMTX                    -1

	//-- moved to arduinoGlue.h // #define _PULSE_TIME           5000
	//-- moved to arduinoGlue.h // #define _PIN_WD_RESET            0    //-- GPIO00
	//-- moved to arduinoGlue.h // #define _PIN_HEARTBEAT           4
	//-- moved to arduinoGlue.h // #define _DTR_ENABLE              5
	//-- moved to arduinoGlue.h // #define LED_BUILTIN             15    //-- esp32
	//-- moved to arduinoGlue.h // #define _TLGRM_LEN           10000    //-- probably a bit to long
	//-- moved to arduinoGlue.h // #define _JSONBUFF_LEN       200000    //-- 60000 is needed for 190 Hour History
	//-- moved to arduinoGlue.h // #define _GMSG_LEN              512
	//-- moved to arduinoGlue.h // #define _FCHAR_LEN              50
	//-- moved to arduinoGlue.h // #define _HOSTNAME_LEN           30
	//-- moved to arduinoGlue.h // #define _MY_SSID_LEN           100
	//-- moved to arduinoGlue.h // #define _MY_PASSWD_LEN         100
	//-- moved to arduinoGlue.h // #define _INDEXPAGE_LEN          50
	//-- moved to arduinoGlue.h // #define _MQTT_BROKER_LEN       101
	//-- moved to arduinoGlue.h // #define _MQTT_USER_LEN          40
	//-- moved to arduinoGlue.h // #define _MQTT_PASSWD_LEN        40
	//-- moved to arduinoGlue.h // #define _MQTT_TOPTOPIC_LEN      21
	//-- moved to arduinoGlue.h // #define _FIELDTABLE_CNAME_LEN  100
	//-- moved to arduinoGlue.h // #define _FIELDTABLE_CVALUE_LEN 100
	//-- moved to arduinoGlue.h // #define _SETTINGS_FILE     "/DSMRsmSettings.json"
	//-- moved to arduinoGlue.h // #define _SYSTEM_FILE       "/DSMRdevSettings.json"
	//-- moved to arduinoGlue.h // #define _STATUS_FILE       "/DSMRstatus.csv"

	//-- moved to arduinoGlue.h // #define LED_ON                 LOW
	//-- moved to arduinoGlue.h // #define LED_OFF               HIGH
	//-- moved to arduinoGlue.h // #define _GLOW_TIME             300
	//-- moved to arduinoGlue.h // #define _FLASH_BUTTON            0
	//-- moved to arduinoGlue.h // #define _FSYS_MAX_FILES         30
	//-- moved to arduinoGlue.h // #define _MQTT_BUFF_MAX         200
//-- (obis 0-0:96.13.1) = 2048 + \r\n\0 => 2051
//-- altered dsmr2lib to 512 (+3)
	//-- moved to arduinoGlue.h // #define _TIMESTAMP_LEN          14  // yymmddhhmmssX\0
	//-- moved to arduinoGlue.h // #define _PSRAM_LIMIT           500

	//-- moved to arduinoGlue.h // #define RNG_HOURS         1
	//-- moved to arduinoGlue.h // #define RNG_DAYS          2
	//-- moved to arduinoGlue.h // #define RNG_MONTHS        3
	//-- moved to arduinoGlue.h // #define RNG_YEARS         4
//-------------------------.........1....1....2....2....3....3....4....4....5....5....6....6....7....7
//-------------------------1...5....0....5....0....5....0....5....0....5....0....5....0....5....0....5
	//-- moved to arduinoGlue.h // #define DATA_FORMAT       "%-8.8s;%10.3f;%10.3f;%10.3f;%10.3f;%10.3f;"
	//-- moved to arduinoGlue.h // #define DATA_CSV_HEADER   "YYMMDDHH;      EDT1;      EDT2;      ERT1;      ERT2;       GDT;#%5d"
//----- room for 74 chars + '\0'
	//-- moved to arduinoGlue.h // #define DATA_RECLEN       75  //-- compatible with reclen API firmware

	//-- moved to arduinoGlue.h // #define HOURS_FILE        "/RINGhours.csv"
	//-- moved to arduinoGlue.h // #define _NO_HOUR_SLOTS_   (48 +1)

	//-- moved to arduinoGlue.h // #define DAYS_FILE         "/RINGdays.csv"
	//-- moved to arduinoGlue.h // #define _NO_DAY_SLOTS_    (14 +1)

	//-- moved to arduinoGlue.h // #define MONTHS_FILE       "/RINGmonths.csv"
	//-- moved to arduinoGlue.h // #define _NO_MONTH_SLOTS_  (24 +1)

//enum    { PERIOD_UNKNOWN, HOURS, DAYS, MONTHS, YEARS };

#include "Debug.h"
#include "wifiEvents.h"   // part of ESP32 Core https://github.com/ESP32/Arduino

//============ Added by Convertor ==========


/**
 * Define the tlgrmData we're interested in, as well as the tlgrmDatastructure to
 * hold the parsed tlgrmData. This list shows all supported fields, remove
 * any fields you are not using from the below list to make the parsing
 * and printing code smaller.
 * Each template argument below results in a field of the same name.
 */
//using MyData = ParsedData<
//               /* String */  identification
//               /* String */, p1_version
//               /* String */, p1_version_be
//               /* String */, timestamp
//               /* String */, equipment_id
//               /* FixedValue */, energy_delivered_tariff1
//               /* FixedValue */, energy_delivered_tariff2
//               /* FixedValue */, energy_returned_tariff1
//               /* FixedValue */, energy_returned_tariff2
//               /* String */, electricity_tariff
//               /* FixedValue */, power_delivered
//               /* FixedValue */, power_returned
//               /* FixedValue */, electricity_threshold
//               /* uint8_t */, electricity_switch_position
//               /* uint32_t */, electricity_failures
//               /* uint32_t */, electricity_long_failures
//               /* String */, electricity_failure_log
//               /* uint32_t */, electricity_sags_l1
//               /* uint32_t */, electricity_sags_l2
//               /* uint32_t */, electricity_sags_l3
//               /* uint32_t */, electricity_swells_l1
//               /* uint32_t */, electricity_swells_l2
//               /* uint32_t */, electricity_swells_l3
//               /* String */, message_short
//               /* String */ //         ,message_long // this one is too big and will crash the MCU
//               /* FixedValue */, voltage_l1
//               /* FixedValue */, voltage_l2
//               /* FixedValue */, voltage_l3
//               /* FixedValue */, current_l1
//               /* FixedValue */, current_l2
//               /* FixedValue */, current_l3
//               /* FixedValue */, power_delivered_l1
//               /* FixedValue */, power_delivered_l2
//               /* FixedValue */, power_delivered_l3
//               /* FixedValue */, power_returned_l1
//               /* FixedValue */, power_returned_l2
//               /* FixedValue */, power_returned_l3
//               /* uint16_t */, mbus1_device_type
//               /* String */, mbus1_equipment_id_tc
//               /* String */, mbus1_equipment_id_ntc
//               /* uint8_t */, mbus1_valve_position
//               /* TimestampedFixedValue */, mbus1_delivered
//               /* TimestampedFixedValue */, mbus1_delivered_ntc
//               /* TimestampedFixedValue */, mbus1_delivered_dbl
//               /* uint16_t */, mbus2_device_type
//               /* String */, mbus2_equipment_id_tc
//               /* String */, mbus2_equipment_id_ntc
//               /* uint8_t */, mbus2_valve_position
//               /* TimestampedFixedValue */, mbus2_delivered
//               /* TimestampedFixedValue */, mbus2_delivered_ntc
//               /* TimestampedFixedValue */, mbus2_delivered_dbl
//               /* uint16_t */, mbus3_device_type
//               /* String */, mbus3_equipment_id_tc
//               /* String */, mbus3_equipment_id_ntc
//               /* uint8_t */, mbus3_valve_position
//               /* TimestampedFixedValue */, mbus3_delivered
//               /* TimestampedFixedValue */, mbus3_delivered_ntc
//               /* TimestampedFixedValue */, mbus3_delivered_dbl
//               /* uint16_t */, mbus4_device_type
//               /* String */, mbus4_equipment_id_tc
//               /* String */, mbus4_equipment_id_ntc
//               /* uint8_t */, mbus4_valve_position
//               /* TimestampedFixedValue */, mbus4_delivered
//               /* TimestampedFixedValue */, mbus4_delivered_ntc
//               /* TimestampedFixedValue */, mbus4_delivered_dbl
//               >;


/*				*** enum moved to arduinoGlue.h ***
enum    { TAB_UNKNOWN, TAB_ACTUEEL, TAB_LAST24HOURS, TAB_LAST7DAYS, TAB_LAST24MONTHS, TAB_GRAPHICS, TAB_SYSINFO, TAB_EDITOR };
*/

/*      *** moved to arduinGlue.h ***     */
//struct myWiFiStruct
//{
//  char      SSID[_MY_SSID_LEN];
//  char      password[_MY_PASSWD_LEN];
//  IPAddress ipWiFi; 
//  IPAddress ipGateway;
//  IPAddress ipDNS; 
//  IPAddress ipSubnet;
//};
myWiFiStruct   myWiFi;


/*				*** union moved to arduinoGlue.h ***
union t
{
  char      cValue[_FIELDTABLE_CVALUE_LEN];
  int32_t   iValue;
  uint32_t  uValue;
  float     fValue;
};
*/

/*				*** struct moved to arduinoGlue.h ***
struct fieldTableStruct
{
  char      cName[_FIELDTABLE_CNAME_LEN];
  t         type;
  char      cType;
};
*/

/*				*** struct moved to arduinoGlue.h ***
struct settingSmStruct
{
  uint8_t   PreDSMR40;
  uint8_t   SmHasFaseInfo;
  uint8_t   Mbus1Type;
  uint8_t   Mbus2Type;
  uint8_t   Mbus3Type;
  uint8_t   Mbus4Type;
  float     EDT1;
  float     EDT2;
  float     ERT1;
  float     ERT2;
  float     GDT;
  float     ENBK;
  float     GNBK;
};
*/

/*				*** struct moved to arduinoGlue.h ***
struct settingDevStruct
{
  char      Hostname[_HOSTNAME_LEN];
  char      IndexPage[_INDEXPAGE_LEN];
  uint8_t   TelegramInterval;
  uint8_t   OledType;    // 0=none, 1=SSD1306, 2=SH1106
  uint16_t  OledSleep;
  uint8_t   OledFlip;
  uint8_t   NeoBrightness;
  uint8_t   DailyReboot;
  uint8_t   runAPmode;
  uint16_t  NoHourSlots;
  uint16_t  NoDaySlots;
  uint16_t  NoMonthSlots;
  char      MQTTbroker[_MQTT_BROKER_LEN];
  char      MQTTuser[_MQTT_USER_LEN];
  char      MQTTpasswd[_MQTT_PASSWD_LEN];
  char      MQTTtopTopic[_MQTT_TOPTOPIC_LEN];
  int32_t   MQTTinterval;
  int16_t   MQTTbrokerPort;
};
*/

	//-- moved to arduinoGlue.h // #define _MAX_ACTUAL_STORE  500 //--155
/*				*** struct moved to arduinoGlue.h ***
struct actualDataStruct
{
  uint32_t  count;
  char      timestamp[_TIMESTAMP_LEN+1];
  float     power_delivered_l1;
  float     power_delivered_l2;
  float     power_delivered_l3;
  float     power_returned_l1;
  float     power_returned_l2;
  float     power_returned_l3;
  float     gas_delivered;
};
*/

/*				*** struct moved to arduinoGlue.h ***
struct timeStruct
{
  char      Timestamp[_TIMESTAMP_LEN];
  int16_t   Year;
  int8_t    Month;
  int8_t    Day;
  int8_t    Hour;
  int8_t    Minute;
  int8_t    Second;
  int8_t    Weekday;    //--  1 = Sunday
  time_t    epoch;
  uint32_t  Months;     //-- Months since epoch
  uint16_t  monthsHist; //-- number of slots per month
  uint16_t  monthSlot;  //-- active month slot
  uint32_t  Days;       //-- Days since epoch
  uint16_t  daysHist;   //-- number of slots per day
  uint16_t  daySlot;    //-- active day slot
  uint32_t  Hours;      //-- Hours since epoch
  uint16_t  hoursHist;  //-- number of slots per hour
  uint16_t  hourSlot;   //-- active hour slot
};
*/

//-- 
timeStruct  lastTlgrmTime;
timeStruct  prevTlgrmTime;

const char *TzLocation = "Europe/Amsterdam";

const char *flashMode[]    { "QIO", "QOUT", "DIO", "DOUT", "Unknown" };

//-- ESP32 core does no longer accept -1 as a GPIO pin
P1Reader    slimmeMeter(&SMserial, _DTR_ENABLE);

//===========================GLOBAL VAR'S======================================
WiFiClient        wifiClient;

MyData            tlgrmData;

char              *tlgrmTmpData;
char              *tlgrmRaw;
char              *jsonBuff;
char              *gMsg;
char              *fChar;

fieldTableStruct  *fieldTable; 
uint16_t          fieldTableCount = 0;
settingSmStruct   *smSetting;
settingDevStruct  *devSetting;
actualDataStruct  *actualStore;

time_t      actT;

char        newTimestamp[_TIMESTAMP_LEN] = {0};
uint32_t    slotErrors = 0;
uint32_t    nrReboots  = 0;
uint32_t    loopCount  = 0;
uint32_t    telegramCount = 0, telegramsAtStart = 0, telegramErrors = 0;
uint8_t     tmpNoHourSlots, tmpNoDaySlots, tmpNoMonthSlots;
bool        tmpAlterRingSlots = false;
bool        showRaw = false;
int8_t      showRawCount = 0;
float       gasDelivered;
uint32_t    antiWearTimer = 0;
bool        updatedRINGfiles = false;

char fieldName[40] = "";

char fieldsArray[50][35] = {{0}}; // to lookup fields
int  fieldsElements      = 0;

char actualTableArray[][35] = { "timestamp"
                           , "power_delivered_l1", "power_delivered_l2", "power_delivered_l3"
                           , "power_returned_l1", "power_returned_l2", "power_returned_l3"
                           , "gas_delivered"
                           , "\0"
                         };

int  actualTableElements = (sizeof(actualTableArray)/sizeof(actualTableArray[0]))-1;

char actualArray[][35] = { "timestamp"
                           , "energy_delivered_tariff1", "energy_delivered_tariff2"
                           , "energy_returned_tariff1", "energy_returned_tariff2"
                           , "power_delivered", "power_returned"
                           , "voltage_l1", "voltage_l2", "voltage_l3"
                           , "current_l1", "current_l2", "current_l3"
                           , "power_delivered_l1", "power_delivered_l2", "power_delivered_l3"
                           , "power_returned_l1", "power_returned_l2", "power_returned_l3"
                           , "mbus1_delivered"
                           , "mbus2_delivered"
                           , "mbus3_delivered"
                           , "mbus4_delivered"
                           , "\0"
                         };

int  actualElements = (sizeof(actualArray)/sizeof(actualArray[0]))-1;

char infoArray[][35]   = { "identification", "p1_version", "p1_version_be"
                           , "equipment_id"
                           , "electricity_tariff"
                           , "mbus1_device_type", "mbus1_equipment"
                           , "mbus2_device_type", "mbus2_equipment"
                           , "mbus3_device_type", "mbus3_equipment"
                           , "mbus4_device_type", "mbus4_equipment"
                           , "\0"
                         };

int  infoElements = (sizeof(infoArray)/sizeof(infoArray[0]))-1;

bool onlyIfPresent  = false;

//-- MQTT client publish and subscribe functionality
//-- https://github.com/knolleary/pubsubclient
//------ [PubSubClient@2.7 pubsubclient-master@2.7]
//#include <PubSubClient.h>                         		//-- moved to arduinoGlue.h

PubSubClient MQTTclient(wifiClient);

char      lastResetCPU0[100]    = {0};
char      lastResetCPU1[100]    = {0};
bool      runAPmode             = false;
bool      filesysMounted        = false;
bool      filesysNotPopulated   = false;
bool      hasAlternativeIndex   = false;
bool      mqttIsConnected       = false;
bool      doLog = false, Verbose1 = false, Verbose2 = false;
int8_t    thisHour = -1, prevNtpHour = 0, thisDay = -1, thisMonth = -1, lastMonth, thisYear = 15;
uint32_t  unixTimestamp;
uint8_t   ntpEventId = 0;
uint64_t  upTimeStart, upTimeSeconds;
uint32_t  actualStoreCount = 0;
uint16_t  actualStoreSlot  = 0;

bool      skipHeartbeats = false;

bool      buttonState = LOW;
uint8_t   msgMode = 0;
bool      boolDisplay = true;
//-in wifiEvents.h - bool      lostWiFiConnection = true;

String    pTimestamp;
uint16_t  lastResetCount = 0;
uint32_t  glowTimer0, glowTimer1;

//===========================================================================================
// setup timers
DECLARE_TIMER_SEC(updateDisplay,       5);
DECLARE_TIMER_MIN(updateTlgrmCount, 4560);  //-- iedere zes uur
DECLARE_TIMER_MIN(reconnectWiFi,      30);
DECLARE_TIMER_SEC(nextTelegram,       10);
DECLARE_TIMER_MIN(reconnectMQTTtimer,  2);  //-- try reconnecting cyclus timer
DECLARE_TIMER_SEC(publishMQTTtimer,   60, SKIP_MISSED_TICKS); // interval time between MQTT messages
DECLARE_TIMER_SEC(antiWearTimer,     301);
DECLARE_TIMER_MIN(oledSleepTimer,     10);  //-- sleep the display in 10 minutes

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

#endif // DSMRLOGGER32_H