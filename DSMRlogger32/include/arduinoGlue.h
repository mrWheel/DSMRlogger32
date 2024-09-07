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
//--pio- #include <ezTime.h>
#include <TelnetStream.h>
#include <ArduinoJson.h>
#include <dsmr2.h>
#include <PubSubClient.h>
#include <TimeSyncLib.h>

extern TimeSync     timeSync;
extern struct tm    timeinfo;
extern time_t       now;

//============ Defines & Macros====================
#define   MAXLINELENGTH     500   // longest normal line is 47 char (+3 for \r\n\0)
#define I2C_ADDRESS 0x3C
#define RST_PIN -1
#define _FW_VERSION "v5.0.5 (24-04-2023)"
#define _SPIFFS
#define FORMAT_SPIFFS_IF_FAILED true
//-- from Debug.h -----------
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
//#define writeToSysLog(...) ({ 
//        sysLog.writeDbg(sysLog.buildD("[%04d-%02d-%02d %02d:%02d:%02d][%-12.12s] "  
//                                                               , tzEurope.year()    
//                                                               , tzEurope.month()   
//                                                               , tzEurope.day()     
//                                                               , tzEurope.hour()    
//                                                               , tzEurope.minute()  
//                                                               , tzEurope.second()  
//                                                               , __FUNCTION__)      
//                                                               ,__VA_ARGS__); 
//                             })

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
#define SECS_PER_MONTH     2592000
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
extern myWiFiStruct myWiFi;

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


extern Adafruit_NeoPixel neoPixels;

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

bool isInFieldsArray(const char *lookUp, int elemts);       
void addToTable(const char *cName, const char *cValue);     
void addToTable(const char *cName, String sValue);          
void addToTable(const char *cName, uint32_t uValue);        
void addToTable(const char *cName, int32_t iValue);         
void addToTable(const char *cName, float fValue);           
void pushToActualStore(const char *cName, String sValue);   
void pushToActualStore(const char *cName, float fValue);    
void pushTlgrmToActualStore();                              

//=======================================================================
template<typename Item>
Item &typecastValue(Item &i)
{
  return i;
}

float typecastValue(TimestampedFixedValue i);               
float typecastValue(FixedValue i);                          

extern char            fieldName[40];                     		//-- from DSMRlogger32
extern uint16_t        fieldTableCount;                   		//-- from DSMRlogger32
extern char            fieldsArray[50][35];                   		//-- from DSMRlogger32
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
    return heap_caps_malloc(size, MALLOC_CAP_SPIRAM);
  }

  void deallocate(void* pointer) 
  {
    heap_caps_free(pointer);
  }

  void* reallocate(void* ptr, size_t new_size) 
  {
    return heap_caps_realloc(ptr, new_size, MALLOC_CAP_SPIRAM);
  }
};

//-- from FSmanager.ino
//struct _catStruct
//{
//  char fDir[35];
//  char fName[35];
//  int fSize;
//};

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

extern MyData      DSMRdata;

using SpiRamJsonDocument = BasicJsonDocument<SpiRamAllocator>;

//============ Extern Variables ============
extern PubSubClient    MQTTclient;                        		//-- from DSMRlogger32
extern HardwareSerial  SMserial;                          		//-- from DSMRlogger32
extern bool            Verbose1;                          		//-- from DSMRlogger32
extern bool            Verbose2;                          		//-- from DSMRlogger32
extern time_t          actT;                              		//-- from DSMRlogger32
extern int             actualElements;                    		//-- from DSMRlogger32
extern uint32_t        actualStoreCount;                  		//-- from DSMRlogger32
extern uint16_t        actualStoreSlot;                   		//-- from DSMRlogger32
extern int             actualTableElements;               		//-- from DSMRlogger32
extern char            actualTableArray[][35];
extern char            actualArray[][35];
extern char            infoArray[][35];
extern uint32_t        antiWearTimer;                     		//-- from DSMRlogger32
extern bool            boolDisplay;                       		//-- from DSMRlogger32
extern bool            buttonState;                       		//-- from DSMRlogger32
extern bool            doLog;                             		//-- from DSMRlogger32
extern bool            filesysMounted;                    		//-- from DSMRlogger32
extern bool            filesysNotPopulated;               		//-- from DSMRlogger32
extern const char*     flashMode[];                       		//-- from DSMRlogger32
extern float           gasDelivered;                      		//-- from DSMRlogger32
extern uint32_t        glowTimer0;                        		//-- from DSMRlogger32
extern uint32_t        glowTimer1;                        		//-- from DSMRlogger32
extern bool            hasAlternativeIndex;               		//-- from DSMRlogger32
extern HTTPUpdateServer httpUpdater;                       		//-- from DSMRlogger32
extern int             infoElements;                      		//-- from DSMRlogger32
//extern bool            isInFieldsArray;                   		//-- from DSMRlogger32
extern int8_t          lastMonth;                         		//-- from DSMRlogger32
extern char            lastResetCPU0[100];                		//-- from DSMRlogger32
extern char            lastResetCPU1[100];                		//-- from DSMRlogger32
extern uint16_t        lastResetCount;                    		//-- from DSMRlogger32
extern timeStruct      lastTlgrmTime;                     		//-- from DSMRlogger32
extern uint32_t        loopCount;                         		//-- from DSMRlogger32
extern bool            mqttIsConnected;                   		//-- from DSMRlogger32
extern uint8_t         msgMode;                           		//-- from DSMRlogger32
extern char            newTimestamp[_TIMESTAMP_LEN];      		//-- from DSMRlogger32
extern uint32_t        nrReboots;                         		//-- from DSMRlogger32
extern uint8_t         ntpEventId;                        		//-- from DSMRlogger32
extern bool            onlyIfPresent;                     		//-- from DSMRlogger32
extern String          pTimestamp;                        		//-- from DSMRlogger32
extern int8_t          prevNtpHour;                       		//-- from DSMRlogger32
extern timeStruct      prevTlgrmTime;                     		//-- from DSMRlogger32
extern bool            runAPmode;                         		//-- from DSMRlogger32
extern bool            showRaw;                           		//-- from DSMRlogger32
extern int8_t          showRawCount;                      		//-- from DSMRlogger32
extern bool            skipHeartbeats;                    		//-- from DSMRlogger32
extern P1Reader        slimmeMeter;                       		//-- from DSMRlogger32
extern uint32_t        slotErrors;                        		//-- from DSMRlogger32
extern ESPSL           sysLog;                            		//-- from DSMRlogger32
extern uint32_t        telegramCount;                     		//-- from DSMRlogger32
extern uint32_t        telegramErrors;                    		//-- from DSMRlogger32
extern uint32_t        telegramsAtStart;                  		//-- from DSMRlogger32
extern int8_t          thisDay;                           		//-- from DSMRlogger32
extern int8_t          thisHour;                          		//-- from DSMRlogger32
extern int8_t          thisMonth;                         		//-- from DSMRlogger32
extern int8_t          thisYear;                          		//-- from DSMRlogger32
extern MyData          tlgrmData;                         		//-- from DSMRlogger32
extern bool            tmpAlterRingSlots;                 		//-- from DSMRlogger32
extern uint8_t         tmpNoDaySlots;                     		//-- from DSMRlogger32
extern uint8_t         tmpNoHourSlots;                    		//-- from DSMRlogger32
extern uint8_t         tmpNoMonthSlots;                   		//-- from DSMRlogger32
extern uint32_t        unixTimestamp;                     		//-- from DSMRlogger32
extern uint64_t        upTimeSeconds;                     		//-- from DSMRlogger32
extern uint64_t        upTimeStart;                       		//-- from DSMRlogger32
extern bool            updatedRINGfiles;                  		//-- from DSMRlogger32
extern char            _bol[128];                         		//-- from Debug
extern char            buffer[100];                       		//-- from FSYSstuff
extern int16_t         bytesRead;                         		//-- from FSYSstuff
extern int16_t         bytesWritten;                      		//-- from FSYSstuff
//extern char            dummy[50];                         		//-- from FSYSstuff
extern char            dumpRec[];          		//-- from FSYSstuff
extern char            key[10];                           		//-- from FSYSstuff
extern timeStruct      newTime;                           		//-- from FSYSstuff
extern uint16_t        noSlots;                           		//-- from FSYSstuff
extern uint16_t        recSlot;                           		//-- from FSYSstuff
extern char            record[];           		//-- from FSYSstuff
extern char            record[];           		//-- from FSYSstuff
extern char            spiffsTimestamp[20];               		//-- from FSYSstuff
extern timeStruct      tmpTime;                           		//-- from FSYSstuff
extern uint16_t        useSlot;                           		//-- from FSYSstuff
extern char            cBuff[100];                        		//-- from FSmanager
extern char            fName[33];                         		//-- from FSmanager
extern char            mName[33];                         		//-- from FSmanager
extern String          temp;                              		//-- from FSmanager
extern IPAddress       MQTTbrokerIP;                      		//-- from MQTTstuff
extern char            MQTTbrokerIPchar[20];              		//-- from MQTTstuff
extern String          MQTTclientId;                      		//-- from MQTTstuff
extern char            lastMQTTtimestamp[15];             		//-- from MQTTstuff
extern char            mqttBuff[100];                     		//-- from MQTTstuff
extern int8_t          reconnectAttempts;                 		//-- from MQTTstuff
extern char            crcChar[10];                       		//-- from handleSlimmeMeter
extern float           CUR_l1;                            		//-- from handleTestdata
extern float           CUR_l2;                            		//-- from handleTestdata
extern float           CUR_l3;                            		//-- from handleTestdata
extern double          C_l1;                              		//-- from handleTestdata
extern double          C_l2;                              		//-- from handleTestdata
extern double          C_l3;                              		//-- from handleTestdata
extern double          ED_T1;                             		//-- from handleTestdata
extern double          ED_T2;                             		//-- from handleTestdata
extern double          ER_T1;                             		//-- from handleTestdata
extern double          ER_T2;                             		//-- from handleTestdata
extern uint8_t         ETariffInd;                        		//-- from handleTestdata
extern float           GDelivered;                        		//-- from handleTestdata
extern float           IPD_l1;                            		//-- from handleTestdata
extern float           IPD_l2;                            		//-- from handleTestdata
extern float           IPD_l3;                            		//-- from handleTestdata
extern float           IPR_l1;                            		//-- from handleTestdata
extern float           IPR_l2;                            		//-- from handleTestdata
extern float           IPR_l3;                            		//-- from handleTestdata
extern float           PDelivered;                        		//-- from handleTestdata
extern float           PReturned;                         		//-- from handleTestdata
extern int8_t          State;                             		//-- from handleTestdata
extern double          V_l1;                              		//-- from handleTestdata
extern double          V_l2;                              		//-- from handleTestdata
extern double          V_l3;                              		//-- from handleTestdata
extern char            actDSMR[3];                        		//-- from handleTestdata
extern int16_t         actDay;                            		//-- from handleTestdata
extern int16_t         actHour;                           		//-- from handleTestdata
extern uint32_t        actInterval;                       		//-- from handleTestdata
extern int16_t         actMinute;                         		//-- from handleTestdata
extern int16_t         actMonth;                          		//-- from handleTestdata
extern int16_t         actSec;                            		//-- from handleTestdata
extern int16_t         actSpeed;                          		//-- from handleTestdata
extern int16_t         actYear;                           		//-- from handleTestdata
extern int16_t         calcCRC;                           		//-- from handleTestdata
extern uint16_t        currentCRC;                        		//-- from handleTestdata
extern int16_t         forceBuildRecs;                    		//-- from handleTestdata
extern bool            forceBuildRingFiles;               		//-- from handleTestdata
extern uint32_t        nextESPcheck;                      		//-- from handleTestdata
extern uint32_t        nextGuiUpdate;                     		//-- from handleTestdata
extern uint32_t        nextMinute;                        		//-- from handleTestdata
extern char            savDSMR[3];                        		//-- from handleTestdata
extern char            telegramLine[MAXLINELENGTH];       		//-- from handleTestdata
extern int16_t         testTlgrmLines;                    		//-- from handleTestdata
extern char            dummy[DATA_RECLEN];                		//-- from menuStuff
extern WebServer       httpServer;                        		//-- from networkStuff
extern bool            isConnected;                       		//-- from networkStuff
extern SSD1306AsciiWire oled;                              		//-- from oledStuff
//extern void            oled_Print_Msg;                    		//-- from oledStuff
extern char            record[];           		//-- from processTelegram
extern float           EDT1;                              		//-- from restAPI
extern float           EDT2;                              		//-- from restAPI
extern float           ERT1;                              		//-- from restAPI
extern float           ERT2;                              		//-- from restAPI
extern float           GDT;                               		//-- from restAPI
extern char            buffer[];            		//-- from restAPI
extern char            crcChar[10];                       		//-- from restAPI
extern char            lLine[_SYSLOG_LINE_LEN];           		//-- from restAPI
extern char            newValue[101];                     		//-- from restAPI
extern char            recID[10];                         		//-- from restAPI
extern char            typeApi[10];                       		//-- from restAPI
extern uint32_t        disconnectWiFiStart;               		//-- from wifiEvents
extern bool            firstConnectionLost;               		//-- from wifiEvents
extern bool            lostWiFiConnection;                		//-- from wifiEvents
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
void connectMQTT();                                         
bool connectMQTT_FSM();                                     
void sendMQTTData();                                        
//-- from handleTestdata.ino -----------
void handleTestdata();                                      
int16_t buildTelegram(int16_t line, char telegramLine[]);   
int16_t buildTelegram30(int16_t line, char telegramLine[]); 
void updateMeterValues(uint8_t period);                     
String Format(double x, int len, int d);                    
int FindCharInArrayRev(unsigned char array[], char c, int len);
int16_t decodeTelegram(int len);                            
//-- from handleSlimmeMeter.ino -----------
void handleSlimmemeter();                                   
void processSlimmemeterRaw();                               
void processSlimmemeter();                                  
void modifySmFaseInfo();                                    
float modifyMbusDelivered();                                
//-- from timeStuff.ino -----------
void logNtpTime();                                          
void saveTimestamp(const char *timeStamp);                  
timeStruct buildTimeStruct(const char *timeStamp, uint16_t hourSlots , uint16_t daySlots , uint16_t monthSlots);
timeStruct calculateTime(timeStruct useTime, int16_t units, int8_t ringType);
String buildDateTimeString(const char *timeStamp, int len); 
void epochToTimestamp(time_t t, char *ts, int8_t len);      
int8_t MinuteFromTimestamp(const char *timeStamp);          
int8_t HourFromTimestamp(const char *timeStamp);            
int8_t DayFromTimestamp(const char *timeStamp);             
int8_t MonthFromTimestamp(const char *timeStamp);           
int8_t YearFromTimestamp(const char *timeStamp);            
time_t epoch(const char *timeStamp, int8_t len, bool syncTime);
//-- from FSmanager.ino -----------
void setupFSmanager();                                      
bool handleList();                                          
void deleteRecursive(const char *path);                     
bool handleFile(String &&path);                             
void handleUpload();                                        
void formatFS();                                            
void listFS();                                              
void sendResponce();                                        
const String formatBytes(size_t const &bytes);              
void reBootESP();                                           
void doRedirect(String msg, int wait, const char *URL, bool reboot);
String getContentType(String filename);                     
int sortFunction(const void *cmp1, const void *cmp2);       
//-- from oledStuff.ino -----------
void checkFlashButton();                                    
void oled_Init();                                           
void oled_Clear();                                          
void oled_Print_Msg(uint8_t line, String message, uint16_t wait);
//-- from processTelegram.ino -----------
void processTelegram();                                     
//-- from settingsStuff.ino -----------
void writeSmSettings();                                     
void readSmSettings(bool show);                             
void updateSmSettings(const char *field, const char *newValue);
void writeDevSettings(bool show);                           
void readDevSettings(bool show);                            
void showDevSettings();                                     
void updateDevSettings(const char *field, const char *newValue);
//-- from restAPI.ino -----------
void processAPI();                                          
void processApiV2Sm(const char* apiId, const char* oneField);
void processApiV2Dev(const char *URI, const char *apiId, const char *word5, const char *word6);
void processApiV2Hist(const char *URI, const char *apiId, const char *word5, const char *word6);
void sendDeviceInfo();                                      
void sendDeviceTime();                                      
void sendSMsettings();                                      
void sendDevSettings();                                     
void sendDeviceDebug(const char *URI, String tail);         
void sendJsonV2smApi(const char *firstLevel);               
void sendJsonActualHist();                                  
void sendJsonHist(int8_t ringType, const char *fileName, timeStruct useTime, uint8_t limit, bool sortDesc);
void copyToFieldsArray(const char inArray[][35], int elemts);
void listFieldsArray(char inArray[][35]);                   
void sendApiNotFound(const char *URI);                      
//-- from networkStuff.ino -----------
void configModeCallback (WiFiManager *myWiFiManager);       
void startWiFi(const char *hostname, int timeOut, bool eraseCredentials);
void startTelnet();                                         
void startMDNS(const char *Hostname);                       
//-- from helperStuff.ino -----------
void pulseHeart(bool force);                                
void pulseHeart();                                          
void resetWatchdog();                                       
boolean isValidIP(IPAddress ip);                            
bool isValidTimestamp(const char *timeStamp, int8_t len);   
int8_t splitString(String inStrng, char delimiter, String wOut[], uint8_t maxWords);
String upTime();                                            
void strCpyFrm(char *dest, int maxLen, const char *src, uint8_t frm, uint8_t to);
float strToFloat(const char *s, int dec);                   
double round1(double value);                                
double round2(double value);                                
double round3(double value);                                
void getLastResetReason(RESET_REASON reason, char *txtReason, int txtReasonLen);
unsigned int CRC16(unsigned int crc, unsigned char *buf, int len);
//-- from menuStuff.ino -----------
void displayHoursHist(bool Telnet=true);                    
void displayDaysHist(bool Telnet=true);                     
void displayMonthsHist(bool Telnet=true);                   
void displayBoardInfo();                                    
void handleKeyInput(char inChar);                           
void wait4KeyInput();                                       
//-- from DSMRlogger32.ino -----------
void displayStatus();                                       
void delayms(unsigned long delay_ms);                       
void doTaskTelegram();                                      
void doSystemTasks();                                       
//-- from DSMRsetupStuff.ino -----------
void setupFileSystem();                                     
void setupSysLogger();                                      
void setupPsram();                                          
bool setupIsFsPopulated();                                  
//-- from FSYSstuff.ino -----------
void readLastStatus();                                      
void writeLastStatus();                                     
void buildDataRecordFromSM(char *recIn, timeStruct useTime);
uint16_t buildDataRecordFromJson(char *recIn, int recLen, String jsonIn);
void writeDataToRingFile(char *fileName, int8_t ringType, char *record, timeStruct slotTime);
void writeDataToRingFiles(timeStruct useTime);              
void readOneSlot(char *record, const char *fileName , uint16_t readSlot , uint16_t maxSlots);
void readAllSlots(char *record, int8_t ringType, const char *fileName, timeStruct thisTime);
bool createRingFile(const char *fileName, timeStruct useTime, int8_t ringType);
bool createRingFile(const char *fileName, timeStruct useTime, int8_t ringType, uint16_t noSlots);
bool alterRingFile();                                       
uint16_t readRingHistoryDepth(const char *fileName, int8_t ringType);
void fillRecord(char *record, int8_t maxLen);               
int32_t freeSpace();                                        
void listFilesystem();                                      
void eraseFile();                                           
bool DSMRfileExist(const char *fileName, const char* funcName, bool doDisplay);
int sortListFiles(const void *cmp1, const void *cmp2);      
//-- from arduinoGlue.h -----------
//void apply(Item &i);                                        
void deallocate(void* pointer);                             
//-- from wifiEvents.h -----------
void WiFiEvent(WiFiEvent_t event);                          
//-- from neoPixelStuff.h -----------
void neoPixOff(int neoPixNr);                               
void neoPixOn(int neoPixNr, neoPixColor color);             
void blinkNeoPixels(uint8_t times, uint16_t speed);         
//-- from safeTimers.h -----------
uint32_t __Due__(uint32_t &timer_due, uint32_t timer_interval, byte timerType);
uint32_t __TimeLeft__(uint32_t timer_due);                  
uint32_t getParam(int i, ...);                              

#endif // ARDUINOGLUE_H
