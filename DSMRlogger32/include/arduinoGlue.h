#ifndef ARDUINOGLUE_H
#define ARDUINOGLUE_H


//============ Includes ====================
#include <SSD1306Ascii.h>
#include <SSD1306AsciiWire.h>
#include <WiFi.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <WiFiManager.h>
#include <FS.h>
#include <esp_heap_caps.h>
#include <Adafruit_NeoPixel.h>
#include <SPIFFS_SysLogger.h>
#include <HardwareSerial.h>
#include <rom/rtc.h>
#include <HTTPUpdateServer.h>
#include <TelnetStream.h>
#include <ArduinoJson.h>
#include <dsmr2.h>
#include <PubSubClient.h>
#include <TimeSyncLib.h>

//-- used in DSMRlogger32.cpp
extern struct tm    timeinfo;                                          //-- from timeStuff
//-- used in handleTestdata.cpp, DSMRlogger32.cpp, restAPI.cpp, menuStuff.cpp, FSYSstuff.cpp, handleSlimmeMeter.cpp
extern time_t       now;                                               //-- from timeStuff

//============ Defines & Macros====================
#define   MAXLINELENGTH     500   // longest normal line is 47 char (+3 for \r\n\0)
#define I2C_ADDRESS 0x3C
#define RST_PIN -1
//#define _FW_VERSION "v5.0.5 (24-04-2023)"
#define _SPIFFS
#define FORMAT_SPIFFS_IF_FAILED true
//-- from Debug.h -----------
//-- not used anywhere
void _debugBOL(const char *fn, int line);                   

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
#define _NEO_PIXELS_COUNT      2
#define _NEO_PIN              23
#define _NEO_CHANNEL           0
#define SKIP_MISSED_TICKS             0
#define SKIP_MISSED_TICKS_WITH_SYNC   1
#define CATCH_UP_MISSED_TICKS         2
#define DECLARE_TIMER_EXTERN(timerName, ...) \
    extern uint32_t timerName##_interval; \
    extern uint32_t timerName##_due; \
    extern byte timerName##_type;


#define DECLARE_TIMER_MIN(timerName, ...) \
  uint32_t timerName##_interval = (getParam(0, __VA_ARGS__, 0) * 60 * 1000), \
                                         timerName##_due  = millis() \
                                             +timerName##_interval \
                                             +random(timerName##_interval / 3); \
  byte     timerName##_type = getParam(1, __VA_ARGS__, 0);
#define DECLARE_TIMER_SEC(timerName, ...) \
  uint32_t timerName##_interval = (getParam(0, __VA_ARGS__, 0) * 1000), \
                                         timerName##_due  = millis() \
                                             +timerName##_interval \
                                             +random(timerName##_interval / 3); \
  byte     timerName##_type = getParam(1, __VA_ARGS__, 0);
#define DECLARE_TIMER_MS(timerName, ...) \
  uint32_t timerName##_interval = (getParam(0, __VA_ARGS__, 0)), \
                                         timerName##_due  = millis() \
                                             +timerName##_interval \
                                             +random(timerName##_interval / 3); \
  byte     timerName##_type = getParam(1, __VA_ARGS__, 0);
#define DECLARE_TIMER   DECLARE_TIMER_MS
#define CHANGE_INTERVAL_MIN(timerName, ...) \
  timerName##_interval = (getParam(0, __VA_ARGS__, 0) *60*1000); \
  timerName##_due  = millis() +timerName##_interval;
#define CHANGE_INTERVAL_SEC(timerName, ...) \
  timerName##_interval = (getParam(0, __VA_ARGS__, 0) *1000); \
  timerName##_due  = millis() +timerName##_interval;
#define CHANGE_INTERVAL_MS(timerName, ...) \
  timerName##_interval = (getParam(0, __VA_ARGS__, 0) ); \
  timerName##_due  = millis() +timerName##_interval;
#define CHANGE_INTERVAL CHANGE_INTERVAL_MS
#define TIME_LEFT(timerName)          ( __TimeLeft__(timerName##_due) )
#define TIME_LEFT_MS(timerName)       ( (TIME_LEFT(timerName) ) )
#define TIME_LEFT_MIN(timerName)      ( (TIME_LEFT(timerName) ) / (60 * 1000))
#define TIME_LEFT_SEC(timerName)      ( (TIME_LEFT(timerName) ) / 1000 )
#define TIME_PAST(timerName)          ( (timerName##_interval - TIME_LEFT(timerName)) )
#define TIME_PAST_MS(timerName)       ( (TIME_PAST(timerName) )
#define TIME_PAST_SEC(timerName)      ( (TIME_PAST(timerName) / 1000) )
#define TIME_PAST_MIN(timerName)      ( (TIME_PAST(timerName) / (60*1000)) )
#define RESTART_TIMER(timerName)      ( timerName##_due = millis()+timerName##_interval );
#define DUE(timerName)                ( __Due__(timerName##_due, timerName##_interval, timerName##_type) )
#define _FSYS SPIFFS

#define writeToSysLog(...) ({ \
        sysLog.writeDbg(sysLog.buildD("[%04d-%02d-%02d %02d:%02d:%02d][%-12.12s] "  \
                            , localtime(&now)->tm_year+1900       \
                            , localtime(&now)->tm_mon+1           \
                            , localtime(&now)->tm_mday            \
                            , localtime(&now)->tm_hour            \
                            , localtime(&now)->tm_min             \
                            , localtime(&now)->tm_sec             \
                            , __FUNCTION__)      \
                            , ##__VA_ARGS__);    \
                           })
 
#define _SYSLOG_LINES       150
#define _SYSLOG_LINE_LEN    120
#define AMSTERDAM_POSIX  "CET-1CEST,M3.5.0,M10.5.0/3"    // Time in Amsterdam
#define  tmYearToCalendar(Y) ((Y) + 1970)  // full four digit year 
#define  CalendarYrToTm(Y)   ((Y) - 1970)
#define _DEFAULT_HOSTNAME  "DSMR-ESP32"
#define SMRX                    18 
#define SMTX                    -1
#define _PULSE_TIME           5000
#define _PIN_WD_RESET            0    //-- GPIO00
#define _PIN_HEARTBEAT           4
#define _DTR_ENABLE              5
#define LED_BUILTIN             15    //-- esp32
#define _SHIELD_TIME            10
#define _TLGRM_LEN           10000    //-- probably a bit to long
#define _JSONBUFF_LEN       200000    //-- 60000 is needed for 190 Hour History
#define _GMSG_LEN              512
#define _FCHAR_LEN              50
#define _HOSTNAME_LEN           30
#define _MY_SSID_LEN           100
#define _MY_PASSWD_LEN         100
#define _INDEXPAGE_LEN          50
#define _MQTT_BROKER_LEN       101
#define _MQTT_USER_LEN          40
#define _MQTT_PASSWD_LEN        40
#define _MQTT_TOPTOPIC_LEN      21
#define _FIELDTABLE_CNAME_LEN  100
#define _FIELDTABLE_CVALUE_LEN 100
#define _SETTINGS_FILE     "/DSMRsmSettings.json"
#define _SYSTEM_FILE       "/DSMRdevSettings.json"
#define _STATUS_FILE       "/DSMRstatus.csv"
#define LED_ON                 LOW
#define LED_OFF               HIGH
#define _GLOW_TIME             300
#define _FLASH_BUTTON            0
#define _FSYS_MAX_FILES         30
#define _MQTT_BUFF_MAX         200
#define _TIMESTAMP_LEN          14  // yymmddhhmmssX\0
#define _PSRAM_LIMIT           500
#define RNG_HOURS         1
#define RNG_DAYS          2
#define RNG_MONTHS        3
#define RNG_YEARS         4
#define DATA_FORMAT       "%-8.8s;%10.3f;%10.3f;%10.3f;%10.3f;%10.3f;"
#define DATA_CSV_HEADER   "YYMMDDHH;      EDT1;      EDT2;      ERT1;      ERT2;       GDT;#%5d"
#define DATA_RECLEN       75  //-- compatible with reclen API firmware
#define HOURS_FILE        "/RINGhours.csv"
#define _NO_HOUR_SLOTS_   (48 +1)
#define DAYS_FILE         "/RINGdays.csv"
#define _NO_DAY_SLOTS_    (14 +1)
#define MONTHS_FILE       "/RINGmonths.csv"
#define _NO_MONTH_SLOTS_  (24 +1)
#define SECS_PER_HOUR         3600
#define SECS_PER_DAY         86400
#define _MAX_ACTUAL_STORE  500 //--155

//============ Structs, Unions & Enums ============
//-- from DSMRlogger32.h
enum    { TAB_UNKNOWN, TAB_ACTUEEL, TAB_LAST24HOURS, TAB_LAST7DAYS, TAB_LAST24MONTHS, TAB_GRAPHICS, TAB_SYSINFO, TAB_EDITOR };

//-- from DSMRlogger32.h
struct myWiFiStruct
{
  char      SSID[_MY_SSID_LEN];
  char      password[_MY_PASSWD_LEN];
  IPAddress ipWiFi; 
  IPAddress ipGateway;
  IPAddress ipDNS; 
  IPAddress ipSubnet;
};
//-- used in DSMRlogger32.cpp
extern myWiFiStruct myWiFi;                                            //-- from networkStuff

//-- from DSMRlogger32.h
union t
{
  char      cValue[_FIELDTABLE_CVALUE_LEN];
  int32_t   iValue;
  uint32_t  uValue;
  float     fValue;
};

//-- from DSMRlogger32.h
struct fieldTableStruct
{
  char      cName[_FIELDTABLE_CNAME_LEN];
  t         type;
  char      cType;
};

//-- from DSMRlogger32.h
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

//-- from DSMRlogger32.h
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
  int8_t    ShieldGpio;
  int8_t    ShieldInversed;
  int16_t   ShieldOnThreshold;
  int16_t   ShieldOffThreshold;
  int32_t   ShieldOnDelay;   //-- seconden
  int32_t   ShieldOffDelay;  //-- seconden
};

//-- from DSMRlogger32.h
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

//-- from DSMRlogger32.h
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

//-- from neoPixelStuff.h
enum neoPixColor {
        neoPixWhiteLow
      , neoPixWhite
      , neoPixRed
      , neoPixGreenLow
      , neoPixGreen
      , neoPixBlue
      , neoPixFade
      , neoPixBlink
};


//-- used in DSMRlogger32.cpp
extern Adafruit_NeoPixel neoPixels;               //-- from settingsStuff

//-- from FSYSstuff.ino
struct listFileStruct
{
  char    Name[32];
  int32_t Size;
};

//-- from DSMRlogger32.ino
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

//-- Used in: DSMRlogger32.cpp, restAPI.cpp
bool isInFieldsArray(const char *lookUp, int elemts);       
//-- Used in: DSMRlogger32.cpp, restAPI.cpp, helperStuff.cpp, MQTTstuff.cpp
void addToTable(const char *cName, const char *cValue);     
//-- Used in: DSMRlogger32.cpp, restAPI.cpp, helperStuff.cpp, MQTTstuff.cpp
void addToTable(const char *cName, String sValue);          
//-- Used in: DSMRlogger32.cpp, restAPI.cpp, helperStuff.cpp, MQTTstuff.cpp
void addToTable(const char *cName, uint32_t uValue);        
//-- Used in: DSMRlogger32.cpp, restAPI.cpp, helperStuff.cpp, MQTTstuff.cpp
void addToTable(const char *cName, int32_t iValue);         
//-- Used in: DSMRlogger32.cpp, restAPI.cpp, helperStuff.cpp, MQTTstuff.cpp
void addToTable(const char *cName, float fValue);           
//-- Used in: DSMRlogger32.cpp, helperStuff.cpp
void pushToActualStore(const char *cName, String sValue);   
//-- Used in: DSMRlogger32.cpp, helperStuff.cpp
void pushToActualStore(const char *cName, float fValue);    
//-- Used in: helperStuff.cpp, processTelegram.cpp
void pushTlgrmToActualStore();                              

//=======================================================================
template<typename Item>
Item &typecastValue(Item &i)
{
  return i;
}

//-- Used in: DSMRlogger32.cpp
float typecastValue(TimestampedFixedValue i);               
//-- Used in: DSMRlogger32.cpp
float typecastValue(FixedValue i);                          

//-- not used extern char            fieldName[40];                     		//-- from DSMRlogger32
//-- used in restAPI.cpp, helperStuff.cpp
extern uint16_t        fieldTableCount;                   		//-- from DSMRlogger32
//-- used in restAPI.cpp
extern char            fieldsArray[50][35];                   		//-- from DSMRlogger32
//-- used in DSMRlogger32.cpp
extern int             fieldsElements;                    		//-- from DSMRlogger32

//-- from DSMRlogger32.ino
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

};

//-- from DSMRlogger32.ino
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

};

//-- from helperStuff.ino
struct SpiRamAllocator 
{
  void* allocate(size_t size) 
  {
//-- Used in: helperStuff.cpp
    return heap_caps_malloc(size, MALLOC_CAP_SPIRAM);
  }

  void deallocate(void* pointer) 
  {
    heap_caps_free(pointer);
  }

  void* reallocate(void* ptr, size_t new_size) 
  {
//-- Used in: helperStuff.cpp
    return heap_caps_realloc(ptr, new_size, MALLOC_CAP_SPIRAM);
  }
};

//-- from handleTestdata.ino
enum runStates { SInit, SMonth, SDay, SHour, SNormal };

//-- from MQTTstuff.ino
enum states_of_MQTT { MQTT_STATE_INIT, MQTT_STATE_TRY_TO_CONNECT, MQTT_STATE_IS_CONNECTED, MQTT_STATE_ERROR };

using MyData = ParsedData<
               /* String */  identification
               /* String */, p1_version
               /* String */, p1_version_be
               /* String */, timestamp
               /* String */, equipment_id
               /* FixedValue */, energy_delivered_tariff1
               /* FixedValue */, energy_delivered_tariff2
               /* FixedValue */, energy_returned_tariff1
               /* FixedValue */, energy_returned_tariff2
               /* String */, electricity_tariff
               /* FixedValue */, power_delivered
               /* FixedValue */, power_returned
               /* FixedValue */, electricity_threshold
               /* uint8_t */, electricity_switch_position
               /* uint32_t */, electricity_failures
               /* uint32_t */, electricity_long_failures
               /* String */, electricity_failure_log
               /* uint32_t */, electricity_sags_l1
               /* uint32_t */, electricity_sags_l2
               /* uint32_t */, electricity_sags_l3
               /* uint32_t */, electricity_swells_l1
               /* uint32_t */, electricity_swells_l2
               /* uint32_t */, electricity_swells_l3
               /* String */, message_short
               /* String */ //         ,message_long // this one is too big and will crash the MCU
               /* FixedValue */, voltage_l1
               /* FixedValue */, voltage_l2
               /* FixedValue */, voltage_l3
               /* FixedValue */, current_l1
               /* FixedValue */, current_l2
               /* FixedValue */, current_l3
               /* FixedValue */, power_delivered_l1
               /* FixedValue */, power_delivered_l2
               /* FixedValue */, power_delivered_l3
               /* FixedValue */, power_returned_l1
               /* FixedValue */, power_returned_l2
               /* FixedValue */, power_returned_l3
               /* uint16_t */, mbus1_device_type
               /* String */, mbus1_equipment_id_tc
               /* String */, mbus1_equipment_id_ntc
               /* uint8_t */, mbus1_valve_position
               /* TimestampedFixedValue */, mbus1_delivered
               /* TimestampedFixedValue */, mbus1_delivered_ntc
               /* TimestampedFixedValue */, mbus1_delivered_dbl
               /* uint16_t */, mbus2_device_type
               /* String */, mbus2_equipment_id_tc
               /* String */, mbus2_equipment_id_ntc
               /* uint8_t */, mbus2_valve_position
               /* TimestampedFixedValue */, mbus2_delivered
               /* TimestampedFixedValue */, mbus2_delivered_ntc
               /* TimestampedFixedValue */, mbus2_delivered_dbl
               /* uint16_t */, mbus3_device_type
               /* String */, mbus3_equipment_id_tc
               /* String */, mbus3_equipment_id_ntc
               /* uint8_t */, mbus3_valve_position
               /* TimestampedFixedValue */, mbus3_delivered
               /* TimestampedFixedValue */, mbus3_delivered_ntc
               /* TimestampedFixedValue */, mbus3_delivered_dbl
               /* uint16_t */, mbus4_device_type
               /* String */, mbus4_equipment_id_tc
               /* String */, mbus4_equipment_id_ntc
               /* uint8_t */, mbus4_valve_position
               /* TimestampedFixedValue */, mbus4_delivered
               /* TimestampedFixedValue */, mbus4_delivered_ntc
               /* TimestampedFixedValue */, mbus4_delivered_dbl
               >;

//-- not used extern MyData      DSMRdata;

using SpiRamJsonDocument = BasicJsonDocument<SpiRamAllocator>;

//============ Extern Variables ============
//-- used in DSMRlogger32.cpp, settingsStuff.cpp
extern const char*     _FW_VERSION;                           //-- from DSMRlogger32.cpp
//-- used in DSMRlogger32.cpp
extern PubSubClient    MQTTclient;                        		//-- from DSMRlogger32
//-- used in DSMRlogger32.cpp, MQTTstuff.cpp, restAPI.cpp, menuStuff.cpp, FSYSstuff.cpp, FSmanager.cpp, settingsStuff.cpp
extern bool            Verbose1;                          		//-- from DSMRlogger32
//-- used in handleTestdata.cpp, MQTTstuff.cpp, restAPI.cpp, menuStuff.cpp, FSYSstuff.cpp, handleSlimmeMeter.cpp
extern bool            Verbose2;                          		//-- from DSMRlogger32
//-- used in helperStuff.cpp
extern int             actualElements;                    		//-- from DSMRlogger32
//-- used in helperStuff.cpp
extern uint32_t        actualStoreCount;                  		//-- from DSMRlogger32
//-- used in helperStuff.cpp
extern uint16_t        actualStoreSlot;                   		//-- from DSMRlogger32
extern char            actualTableArray[][35];
extern char            actualArray[][35];
extern char            infoArray[][35];
extern bool            boolDisplay;                       		//-- from DSMRlogger32
extern bool            buttonState;                       		//-- from DSMRlogger32
//-- used in DSMRlogger32.cpp, helperStuff.cpp, DSMRsetupStuff.cpp
extern bool            filesysMounted;                    		//-- from DSMRlogger32
//-- used in handleTestdata.cpp, DSMRlogger32.cpp, networkStuff.cpp, MQTTstuff.cpp, restAPI.cpp, menuStuff.cpp, FSYSstuff.cpp, FSmanager.cpp, settingsStuff.cpp, processTelegram.cpp, helperStuff.cpp, DSMRsetupStuff.cpp, handleSlimmeMeter.cpp
extern const char*     flashMode[];                       		//-- from DSMRlogger32
//-- used in handleTestdata.cpp, restAPI.cpp, FSYSstuff.cpp, helperStuff.cpp, handleSlimmeMeter.cpp
extern float           gasDelivered;                      		//-- from DSMRlogger32
//-- used in helperStuff.cpp
extern uint32_t        glowTimer0;                        		//-- from DSMRlogger32
//-- used in DSMRlogger32.cpp, handleSlimmeMeter.cpp, FSYSstuff.cpp
extern uint32_t        glowTimer1;                        		//-- from DSMRlogger32
//-- used in DSMRlogger32.cpp
extern bool            hasAlternativeIndex;               		//-- from DSMRlogger32
//-- used in DSMRlogger32.cpp, networkStuff.cpp
extern HTTPUpdateServer httpUpdater;                       		//-- from DSMRlogger32
extern int             infoElements;                      		//-- from DSMRlogger32
//-- used in DSMRlogger32.cpp
extern char            lastResetCPU0[100];                		//-- from DSMRlogger32
//-- used in DSMRlogger32.cpp
extern char            lastResetCPU1[100];                		//-- from DSMRlogger32
//-- used in handleTestdata.cpp, DSMRlogger32.cpp, restAPI.cpp, menuStuff.cpp, FSYSstuff.cpp, processTelegram.cpp
extern timeStruct      lastTlgrmTime;                     		//-- from DSMRlogger32
//-- used in settingsStuff.cpp, restAPI.cpp
extern bool            mqttIsConnected;                   		//-- from DSMRlogger32
//-- used in DSMRlogger32.cpp
extern uint8_t         msgMode;                           		//-- from DSMRlogger32
//-- used in DSMRlogger32.cpp, FSYSstuff.cpp, menuStuff.cpp
extern uint32_t        nrReboots;                         		//-- from DSMRlogger32
//-- used in DSMRlogger32.cpp
extern uint8_t         ntpEventId;                        		//-- from DSMRlogger32
//-- used in restAPI.cpp, helperStuff.cpp
extern bool            onlyIfPresent;                     		//-- from DSMRlogger32
//-- used in processTelegram.cpp, FSYSstuff.cpp
extern timeStruct      prevTlgrmTime;                     		//-- from DSMRlogger32
//-- used in DSMRlogger32.cpp, settingsStuff.cpp, menuStuff.cpp
extern bool            runAPmode;                         		//-- from DSMRlogger32
//-- used in menuStuff.cpp
extern bool            showRaw;                           		//-- from DSMRlogger32
//-- used in menuStuff.cpp
extern int8_t          showRawCount;                      		//-- from DSMRlogger32
//-- used in helperStuff.cpp
extern bool            skipHeartbeats;                    		//-- from DSMRlogger32
//-- used in DSMRlogger32.cpp
extern P1Reader        slimmeMeter;                       		//-- from DSMRlogger32
//-- used in DSMRlogger32.cpp, FSYSstuff.cpp, menuStuff.cpp
extern uint32_t        slotErrors;                        		//-- from DSMRlogger32
//-- used in DSMRsetupStuff.cpp, menuStuff.cpp
extern ESPSL           sysLog;                            		//-- from DSMRlogger32
//-- used in DSMRlogger32.cpp, restAPI.cpp, menuStuff.cpp, FSYSstuff.cpp, processTelegram.cpp, handleSlimmeMeter.cpp
extern uint32_t        telegramCount;                     		//-- from DSMRlogger32
//-- used in DSMRlogger32.cpp, restAPI.cpp, menuStuff.cpp, processTelegram.cpp, handleSlimmeMeter.cpp
extern uint32_t        telegramErrors;                    		//-- from DSMRlogger32
//-- used in FSYSstuff.cpp
extern uint32_t        telegramsAtStart;                  		//-- from DSMRlogger32
//-- used in handleTestdata.cpp, restAPI.cpp, FSYSstuff.cpp, processTelegram.cpp, helperStuff.cpp, handleSlimmeMeter.cpp
extern MyData          tlgrmData;                         		//-- from DSMRlogger32
//-- used in FSYSstuff.cpp
extern bool            tmpAlterRingSlots;                 		//-- from DSMRlogger32
//-- used in FSYSstuff.cpp
extern uint8_t         tmpNoDaySlots;                     		//-- from DSMRlogger32
//-- used in FSYSstuff.cpp
extern uint8_t         tmpNoHourSlots;                    		//-- from DSMRlogger32
//-- used in FSYSstuff.cpp
extern uint8_t         tmpNoMonthSlots;                   		//-- from DSMRlogger32
//-- used in helperStuff.cpp
extern uint64_t        upTimeSeconds;                     		//-- from DSMRlogger32
//-- used in helperStuff.cpp
extern uint64_t        upTimeStart;                       		//-- from DSMRlogger32
//-- used in DSMRlogger32.cpp
extern bool            updatedRINGfiles;                  		//-- from DSMRlogger32
//-- used in restAPI.cpp, FSYSstuff.cpp
extern char            buffer[100];                       		//-- from FSYSstuff
//-- used in FSYSstuff.cpp
extern int16_t         bytesRead;                         		//-- from FSYSstuff
//-- used in DSMRlogger32.cpp, FSYSstuff.cpp
extern char            key[10];                           		//-- from FSYSstuff
//-- used in FSYSstuff.cpp
extern timeStruct      newTime;                           		//-- from FSYSstuff
//-- used in FSYSstuff.cpp
extern uint16_t        recSlot;                           		//-- from FSYSstuff
//-- used in restAPI.cpp, processTelegram.cpp, FSYSstuff.cpp
extern char            record[];           		                //-- from FSYSstuff
//-- used in restAPI.cpp, processTelegram.cpp, FSYSstuff.cpp
extern char            record[];           		                //-- from FSYSstuff
//-- used in restAPI.cpp, FSYSstuff.cpp
extern char            fName[33];                         		//-- from FSmanager
//-- used in FSYSstuff.cpp
extern String          temp;                              		//-- from FSmanager
//-- used in restAPI.cpp, handleSlimmeMeter.cpp
extern char            crcChar[10];                       		//-- from handleSlimmeMeter
//-- used in handleTestdata.cpp
extern int8_t          State;                             		//-- from handleTestdata
//-- used in menuStuff.cpp
extern bool            forceBuildRingFiles;               		//-- from handleTestdata
//-- used in FSmanager.cpp, FSYSstuff.cpp, menuStuff.cpp
extern char            dummy[DATA_RECLEN];                		//-- from menuStuff
//-- used in handleTestdata.cpp, restAPI.cpp, networkStuff.cpp, DSMRlogger32.cpp
extern WebServer       httpServer;                        		//-- from networkStuff
//-- used in DSMRlogger32.cpp
extern SSD1306AsciiWire oled;                              		//-- from oledStuff
//-- used in restAPI.cpp, processTelegram.cpp, FSYSstuff.cpp
extern char            record[];           		                //-- from processTelegram
//-- used in settingsStuff.cpp, FSYSstuff.cpp
extern float           EDT1;                              		//-- from restAPI
//-- used in settingsStuff.cpp, FSYSstuff.cpp
extern float           EDT2;                              		//-- from restAPI
//-- used in settingsStuff.cpp, FSYSstuff.cpp
extern float           ERT1;                              		//-- from restAPI
//-- used in settingsStuff.cpp, FSYSstuff.cpp
extern float           ERT2;                              		//-- from restAPI
//-- used in settingsStuff.cpp, FSYSstuff.cpp
extern float           GDT;                               		//-- from restAPI
//-- used in restAPI.cpp, FSYSstuff.cpp
extern char            buffer[];            		              //-- from restAPI
//-- used in restAPI.cpp, handleSlimmeMeter.cpp
extern char            crcChar[10];                       		//-- from restAPI
//-- used in settingsStuff.cpp
extern char            newValue[101];                     		//-- from restAPI
//-- used in DSMRlogger32.cpp
extern bool            firstConnectionLost;               		//-- from wifiEvents
//-- used in DSMRlogger32.cpp, helperStuff.cpp, networkStuff.cpp
extern bool            lostWiFiConnection;                		//-- from wifiEvents
//-- used in DSMRlogger32.cpp
extern int             lostWiFiCount;                     		//-- from wifiEvents

extern char              *tlgrmTmpData;
extern char              *tlgrmRaw;
extern char              *jsonBuff;
extern char              *gMsg;
extern char              *fChar;

extern fieldTableStruct  *fieldTable; 
extern settingSmStruct   *smSetting;
extern settingDevStruct  *devSetting;
extern actualDataStruct  *actualStore;

//============ Function Prototypes =========
//-- from MQTTstuff.ino -----------
//-- Used in: DSMRlogger32.cpp, MQTTstuff.cpp
void connectMQTT();                                         
//-- Used in: MQTTstuff.cpp
bool connectMQTT_FSM();                                     
//-- Used in: processTelegram.cpp, MQTTstuff.cpp
void sendMQTTData();                                        
//-- from handleSlimmeMeter.ino -----------
//-- Used in: DSMRlogger32.cpp, handleSlimmeMeter.cpp
void handleSlimmemeter();                                   
//-- Used in: handleSlimmeMeter.cpp
void processSlimmemeterRaw();                               
//-- Used in: handleSlimmeMeter.cpp
void processSlimmemeter();                                  
//-- Used in: handleTestdata.cpp, handleSlimmeMeter.cpp
void modifySmFaseInfo();                                    
//-- Used in: handleTestdata.cpp, handleSlimmeMeter.cpp
float modifyMbusDelivered();                                
//-- from timeStuff.ino -----------
//-- Used in: DSMRlogger32.cpp, timeStuff.cpp
void logNtpTime();                                          
//-- Used in: DSMRlogger32.cpp, processTelegram.cpp, timeStuff.cpp
void saveTimestamp(const char *timeStamp);                  
//-- Used in: timeStuff.cpp, FSYSstuff.cpp
timeStruct buildTimeStruct(const char *timeStamp, uint16_t hourSlots , uint16_t daySlots , uint16_t monthSlots);
//-- Used in: timeStuff.cpp, FSYSstuff.cpp
timeStruct calculateTime(timeStruct useTime, int16_t units, int8_t ringType);
//-- Used in: processTelegram.cpp, timeStuff.cpp
String buildDateTimeString(const char *timeStamp, int len); 
//-- Used in: handleTestdata.cpp, timeStuff.cpp
void epochToTimestamp(time_t t, char *ts, int8_t len);      
//-- from FSmanager.ino -----------
//-- Used in: FSmanager.cpp, DSMRlogger32.cpp
void setupFSmanager();                                      
//-- Used in: FSmanager.cpp, FSYSstuff.cpp
int sortFunction(const void *cmp1, const void *cmp2);       
//-- from oledStuff.ino -----------
//-- Used in: DSMRlogger32.cpp, oledStuff.cpp
void checkFlashButton();                                    
//-- Used in: DSMRlogger32.cpp, settingsStuff.cpp, oledStuff.cpp
void oled_Init();                                           
//-- Used in: DSMRlogger32.cpp, networkStuff.cpp, oledStuff.cpp
void oled_Clear();                                          
//-- Used in: DSMRlogger32.cpp, networkStuff.cpp, FSYSstuff.cpp, oledStuff.cpp, processTelegram.cpp, handleSlimmeMeter.cpp
void oled_Print_Msg(uint8_t line, String message, uint16_t wait);
//-- from processTelegram.ino -----------
//-- Used in: handleTestdata.cpp, processTelegram.cpp, handleSlimmeMeter.cpp
void processTelegram();                                     
//-- from settingsStuff.ino -----------
//-- Used in: DSMRlogger32.cpp, settingsStuff.cpp, menuStuff.cpp
void readSmSettings(bool show);                             
//-- Used in: settingsStuff.cpp, restAPI.cpp
void updateSmSettings(const char *field, const char *newValue);
//-- Used in: settingsStuff.cpp, FSYSstuff.cpp
void writeDevSettings(bool show);                           
//-- Used in: DSMRlogger32.cpp, settingsStuff.cpp, menuStuff.cpp
void readDevSettings(bool show);                            
//-- Used in: settingsStuff.cpp, restAPI.cpp
void updateDevSettings(const char *field, const char *newValue);
//-- from restAPI.ino -----------
//-- Used in: FSmanager.cpp, restAPI.cpp, DSMRlogger32.cpp
void processAPI();                                          
//-- Used in: restAPI.cpp, helperStuff.cpp
void copyToFieldsArray(const char inArray[][35], int elemts);
//-- from networkStuff.ino -----------
//-- Used in: DSMRlogger32.cpp, networkStuff.cpp, menuStuff.cpp
void startWiFi(const char *hostname, int timeOut, bool eraseCredentials);
//-- Used in: DSMRlogger32.cpp, networkStuff.cpp
void startTelnet();                                         
//-- Used in: DSMRlogger32.cpp, settingsStuff.cpp, networkStuff.cpp
void startMDNS(const char *Hostname);                       
//-- from helperStuff.ino -----------
//-- Used in: DSMRlogger32.cpp, networkStuff.cpp, FSYSstuff.cpp, FSmanager.cpp, helperStuff.cpp
void pulseHeart(bool force);                                
//-- Used in: DSMRlogger32.cpp, networkStuff.cpp, FSYSstuff.cpp, FSmanager.cpp, helperStuff.cpp
void pulseHeart();                                          
//-- Used in: DSMRlogger32.cpp, helperStuff.cpp, networkStuff.cpp, menuStuff.cpp
void resetWatchdog();                                       
//-- Used in: helperStuff.cpp, MQTTstuff.cpp
boolean isValidIP(IPAddress ip);                            
//-- Used in: restAPI.cpp, helperStuff.cpp, FSYSstuff.cpp
bool isValidTimestamp(const char *timeStamp, int8_t len);   
//-- Used in: restAPI.cpp, helperStuff.cpp
int8_t splitString(String inStrng, char delimiter, String wOut[], uint8_t maxWords);
//-- Used in: DSMRlogger32.cpp, restAPI.cpp, helperStuff.cpp, menuStuff.cpp
String upTime();                                            
//-- Used in: helperStuff.cpp, timeStuff.cpp, FSYSstuff.cpp
void strCpyFrm(char *dest, int maxLen, const char *src, uint8_t frm, uint8_t to);
//-- Used in: DSMRlogger32.cpp, helperStuff.cpp
float strToFloat(const char *s, int dec);                   
//-- Used in: helperStuff.cpp, MQTTstuff.cpp
double round1(double value);                                
//-- Used in: helperStuff.cpp
double round2(double value);                                
//-- Used in: restAPI.cpp, helperStuff.cpp, MQTTstuff.cpp
double round3(double value);                                
//-- Used in: DSMRlogger32.cpp, helperStuff.cpp
void getLastResetReason(RESET_REASON reason, char *txtReason, int txtReasonLen);
//-- Used in: handleTestdata.cpp, restAPI.cpp, helperStuff.cpp, handleSlimmeMeter.cpp
unsigned int CRC16(unsigned int crc, unsigned char *buf, int len);
//-- from menuStuff.ino -----------
//-- Used in: DSMRlogger32.cpp, menuStuff.cpp
void wait4KeyInput();                                       
//-- from DSMRsetupStuff.ino -----------
//-- Used in: DSMRlogger32.cpp, DSMRsetupStuff.cpp
void setupFileSystem();                                     
//-- Used in: DSMRlogger32.cpp, DSMRsetupStuff.cpp
void setupSysLogger(const char*);                                      
//-- Used in: DSMRlogger32.cpp, DSMRsetupStuff.cpp
void setupPsram();                                          
//-- Used in: DSMRlogger32.cpp, DSMRsetupStuff.cpp
bool setupIsFsPopulated();                                  
//-- from FSYSstuff.ino -----------
//-- Used in: DSMRlogger32.cpp, FSYSstuff.cpp
void readLastStatus();                                      
//-- Used in: DSMRlogger32.cpp, restAPI.cpp, FSYSstuff.cpp, menuStuff.cpp, processTelegram.cpp
void writeLastStatus();                                     
//-- Used in: processTelegram.cpp, FSYSstuff.cpp
void buildDataRecordFromSM(char *recIn, timeStruct useTime);
//-- Used in: restAPI.cpp, FSYSstuff.cpp
uint16_t buildDataRecordFromJson(char *recIn, int recLen, String jsonIn);
//-- Used in: restAPI.cpp, processTelegram.cpp, FSYSstuff.cpp
void writeDataToRingFile(char *fileName, int8_t ringType, char *record, timeStruct slotTime);
//-- Used in: FSYSstuff.cpp, restAPI.cpp, menuStuff.cpp
void writeDataToRingFiles(timeStruct useTime);              
//-- Used in: FSYSstuff.cpp, menuStuff.cpp
void readAllSlots(char *record, int8_t ringType, const char *fileName, timeStruct thisTime);
//-- Used in: settingsStuff.cpp, FSYSstuff.cpp
bool alterRingFile();                                       
//-- Used in: settingsStuff.cpp, FSYSstuff.cpp
uint16_t readRingHistoryDepth(const char *fileName, int8_t ringType);
//-- Used in: FSYSstuff.cpp, menuStuff.cpp
void listFilesystem();                                      
//-- Used in: FSYSstuff.cpp, menuStuff.cpp
void eraseFile();                                           
//-- Used in: DSMRsetupStuff.cpp, FSYSstuff.cpp
bool DSMRfileExist(const char *fileName, const char* funcName, bool doDisplay);
//-- from wifiEvents.h -----------
//-- Used in: DSMRlogger32.cpp, wifiEvents.cpp
void WiFiEvent(WiFiEvent_t event);                          
//-- from neoPixelStuff.h -----------
//-- Used in: DSMRlogger32.cpp, networkStuff.cpp, FSYSstuff.cpp
void neoPixOff(int neoPixNr);                               
//-- Used in: handleTestdata.cpp, DSMRlogger32.cpp, networkStuff.cpp, FSYSstuff.cpp, helperStuff.cpp, handleSlimmeMeter.cpp
void neoPixOn(int neoPixNr, neoPixColor color);             
//-- Used in: DSMRlogger32.cpp, menuStuff.cpp
void blinkNeoPixels(uint8_t times, uint16_t speed);         
//-- from safeTimers.h -----------
uint32_t __Due__(uint32_t &timer_due, uint32_t timer_interval, byte timerType);
uint32_t __TimeLeft__(uint32_t timer_due);                  
uint32_t getParam(int i, ...);                              

#endif // ARDUINOGLUE_H
