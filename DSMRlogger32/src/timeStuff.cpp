/*
***************************************************************************
**  Program  : timeStuff, part of DSMRlogger32
**  Version  : v5.n
**
**  Copyright (c) 2020 .. 2023 Willem Aandewiel
**
**  TERMS OF USE: MIT License. See bottom of file.
***************************************************************************
*/
#include "timeStuff.h"


//===========================================================================================
void logNtpTime()
{
  DebugTf("log NTP Date/Time: %02d-%02d-%04d %02d:%02d:%02d\r\n"
                                             , localtime(&now)->tm_mday
                                             , localtime(&now)->tm_mon+1
                                             , localtime(&now)->tm_year+1900
                                             , localtime(&now)->tm_hour
                                             , localtime(&now)->tm_min
                                             , localtime(&now)->tm_sec);
  if ((localtime(&now)->tm_hour == 12) || (ntpEventId == 0))
  {
    writeToSysLog("NTP Date/Time: %02d-%02d-%04d %02d:%02d:%02d"
                                             , localtime(&now)->tm_mday
                                             , localtime(&now)->tm_mon+1
                                             , localtime(&now)->tm_year+1900
                                             , localtime(&now)->tm_hour
                                             , localtime(&now)->tm_min
                                             , localtime(&now)->tm_sec);
  }
  //ntpEventId = setEvent(logNtpTime, now()+3600);

} //  logNtpTime()


//===========================================================================================
void saveTimestamp(const char *timeStamp)
{
  //-- save this timestamp as "previous timestamp"
  prevTlgrmTime = lastTlgrmTime;
  //-- fill all other fields
  strlcpy(lastTlgrmTime.Timestamp, timeStamp, _TIMESTAMP_LEN);
  lastTlgrmTime = buildTimeStruct(lastTlgrmTime.Timestamp, devSetting->NoHourSlots
                                                         , devSetting->NoDaySlots 
                                                         , devSetting->NoMonthSlots);

} //  saveTimestamp()

/****
//===========================================================================================
timeStruct buildTimeStruct(const char *timeStamp, uint16_t hourSlots
                                                , uint16_t daySlots
                                                , uint16_t monthSlots)
{
  if (hourSlots  < _NO_HOUR_SLOTS_)  hourSlots  = _NO_HOUR_SLOTS_;
  if (daySlots   < _NO_DAY_SLOTS_)   daySlots   = _NO_DAY_SLOTS_;
  if (monthSlots < _NO_MONTH_SLOTS_) monthSlots = _NO_MONTH_SLOTS_;
  timeStruct thisTime;
  //-- fill all other fields
  strlcpy(thisTime.Timestamp, timeStamp, _TIMESTAMP_LEN);

  switch(strlen(thisTime.Timestamp))
  {
    case  4: strlcat(thisTime.Timestamp, "01010101", _TIMESTAMP_LEN);  break;
    case  5: strlcat(thisTime.Timestamp, "1010101", _TIMESTAMP_LEN);  break;
    case  6: strlcat(thisTime.Timestamp, "010101", _TIMESTAMP_LEN);  break;
    case  7: strlcat(thisTime.Timestamp, "10101", _TIMESTAMP_LEN);  break;
    case  8: strlcat(thisTime.Timestamp, "0101", _TIMESTAMP_LEN);  break;
    case  9: strlcat(thisTime.Timestamp, "101", _TIMESTAMP_LEN);  break;
    case 10: strlcat(thisTime.Timestamp, "01", _TIMESTAMP_LEN);  break;
    case 11: strlcat(thisTime.Timestamp, "1", _TIMESTAMP_LEN);  break;
  }

  thisTime.Year   = String(timeStamp).substring(0, 2).toInt();    // YY
  thisTime.Month  = String(timeStamp).substring(2, 4).toInt();    // MM
  thisTime.Day    = String(timeStamp).substring(4, 6).toInt();    // DD
  thisTime.Hour   = String(timeStamp).substring(6, 8).toInt();    // HH
  thisTime.Minute = String(timeStamp).substring(8, 10).toInt();   // MM
  thisTime.Second = String(timeStamp).substring(10, 12).toInt();  // SS

  //-- save Europe epoch ..
  time_t  epochEurope = time(0);
  //--pio- Timezone tzSM;
  //-- set SM time
  //--pio- tzSM.setTime(thisTime.Hour, thisTime.Minute, thisTime.Second,
  //--pio-              thisTime.Day,  thisTime.Month,  thisTime.Year);
                   // Fill the tm structure
    timeinfo.tm_year = thisTime.Year + (2000 - 1900);  // Years since 1900
    timeinfo.tm_mon  = thisTime.Month;        // Months since January (0-11)
    timeinfo.tm_mday = thisTime.Day;          // Day of the month
    timeinfo.tm_hour = thisTime.Hour;         // Hours (0-23)
    timeinfo.tm_min  = thisTime.Minute;       // Minutes (0-59)
    timeinfo.tm_sec  = thisTime.Second;       // Seconds (0-59)

    // Convert the tm structure to epoch time
    time_t smEpoch = mktime(&timeinfo);

  //-- create SM epoch .. and other fields
  thisTime.epoch      = smEpoch;
  thisTime.Weekday    = timeinfo()->tm.weekday();  //-- 1 = Sunday
  thisTime.Months     = ((tzSM.year() -1) * 12) + tzSM.month();
  thisTime.monthsHist = monthSlots;
  thisTime.monthSlot  = (thisTime.Months % monthSlots);
  thisTime.Days       = (thisTime.epoch / SECS_PER_DAY);
  thisTime.daysHist   = daySlots;
  thisTime.daySlot    = (thisTime.Days % daySlots);
  thisTime.Hours      = (thisTime.epoch / SECS_PER_HOUR);
  thisTime.hoursHist  = hourSlots;
  thisTime.hourSlot  = (thisTime.Hours % hourSlots);
  //-- reset Europe time
  tzEurope.setTime(epochEurope);

  return thisTime;
  
} //  buildTimeStruct()
***/

/**** timeStruct
struct timeStruct {
    char Timestamp[13];   // "YYMMDDHHMMSS" with null terminator
    int Year;
    int Month;
    int Day;
    int Hour;
    int Minute;
    int Second;
    time_t epoch;
    int Weekday;   // Day of the week (0-6, where 0 = Sunday)
    int Months;
    int monthSlot;
    int daysHist;
    int daySlot;
    int hoursHist;
    int hourSlot;
    int Days;
    int Hours;
    int monthsHist;
};
***/
// Function to fill missing parts of the timestamp if it has fewer than 12 characters.
//===========================================================================================
void fillMissingTimestamp(char *timestamp) 
{
    const char *defaultFill[] = 
    {
        "010101010101", "10101010101", "0101010101", "101010101", 
        "01010101", "1010101", "010101", "10101", 
        "0101", "101", "01", "1"
    };
    size_t len = strlen(timestamp);
    if (len < 12) 
    {
        strlcat(timestamp, defaultFill[len], _TIMESTAMP_LEN);
    }
}

//===========================================================================================
timeStruct buildTimeStruct(const char *timeStamp, uint16_t hourSlots, uint16_t daySlots, uint16_t monthSlots) 
{
    timeStruct thisTime;
    
    // Ensure valid slots
    if (hourSlots < _NO_HOUR_SLOTS_)    hourSlots = _NO_HOUR_SLOTS_;
    if (daySlots < _NO_DAY_SLOTS_)      daySlots = _NO_DAY_SLOTS_;
    if (monthSlots < _NO_MONTH_SLOTS_)  monthSlots = _NO_MONTH_SLOTS_;
    
    // Copy the timestamp and fill missing parts
    strncpy(thisTime.Timestamp, timeStamp, _TIMESTAMP_LEN - 1);
    thisTime.Timestamp[_TIMESTAMP_LEN - 1] = '\0';
    fillMissingTimestamp(thisTime.Timestamp);
    
    // Extract values from the filled timestamp
    thisTime.Year   = atoi(thisTime.Timestamp) + 2000;
    thisTime.Month  = atoi(thisTime.Timestamp + 2);
    thisTime.Day    = atoi(thisTime.Timestamp + 4);
    thisTime.Hour   = atoi(thisTime.Timestamp + 6);
    thisTime.Minute = atoi(thisTime.Timestamp + 8);
    thisTime.Second = atoi(thisTime.Timestamp + 10);
    
    // Convert to epoch time
    struct tm timeinfo = {0};
    timeinfo.tm_year  = thisTime.Year - 1900;
    timeinfo.tm_mon   = thisTime.Month - 1;
    timeinfo.tm_mday  = thisTime.Day;
    timeinfo.tm_hour  = thisTime.Hour;
    timeinfo.tm_min   = thisTime.Minute;
    timeinfo.tm_sec   = thisTime.Second;
    thisTime.epoch    = mktime(&timeinfo);
    
    // Calculate additional fields
    thisTime.Weekday    = (timeinfo.tm_wday + 1) % 7;  // 0 = Sunday, 6 = Saturday
    thisTime.Months     = (thisTime.Year - 1970) * 12 + thisTime.Month - 1;
    thisTime.monthsHist = monthSlots;
    thisTime.monthSlot  = thisTime.Months % monthSlots;
    thisTime.Days       = thisTime.epoch / SECS_PER_DAY;
    thisTime.daysHist   = daySlots;
    thisTime.daySlot    = thisTime.Days % daySlots;
    thisTime.Hours      = thisTime.epoch / SECS_PER_HOUR;
    thisTime.hoursHist  = hourSlots;
    thisTime.hourSlot   = thisTime.Hours % hourSlots;
    
    return thisTime;
    
} //  buildTimeStruct()buildTimeStruct()

//===========================================================================================
timeStruct calculateTime(timeStruct useTime, int16_t units, int8_t ringType)
{
  time_t actualTime = time(0);  // Save the actual time
  
  timeStruct newTime = useTime;
  
  switch(ringType)
  {
    case RNG_HOURS:
        newTime.epoch += (units * SECS_PER_HOUR);
        break;
    case RNG_DAYS:
        newTime.epoch += (units * SECS_PER_DAY);
        break;
    case RNG_MONTHS:
        newTime.epoch += (units * SECS_PER_MONTH);
        break;
  }

  struct tm *localTime = localtime(&newTime.epoch);
  
  newTime.Year      = localTime->tm_year + 1900;
  newTime.Month     = localTime->tm_mon + 1;
  newTime.Day       = localTime->tm_mday;
  newTime.Hour      = localTime->tm_hour;
  newTime.Minute    = localTime->tm_min;
  newTime.Second    = localTime->tm_sec;
  newTime.Weekday   = localTime->tm_wday + 1;  // tm_wday is 0-6, we want 1-7
  
  newTime.Months    = ((newTime.Year - 1970) * 12) + newTime.Month - 1;
  newTime.monthSlot = (newTime.Months % useTime.monthsHist);
  newTime.Days      = newTime.epoch / SECS_PER_DAY;
  newTime.daySlot   = (newTime.Days % useTime.daysHist);
  newTime.Hours     = newTime.epoch / SECS_PER_HOUR;
  newTime.hourSlot  = (newTime.Hours % useTime.hoursHist);

  snprintf(newTime.Timestamp, _TIMESTAMP_LEN, "%04d%02d%02d%02d%02d%02dX"
                                        , newTime.Year
                                        , newTime.Month
                                        , newTime.Day
                                        , newTime.Hour
                                        , newTime.Minute
                                        , newTime.Second);

  time_t ignoredTime;
  time(&ignoredTime);  // Restore the actual time (this updates the internal time)

  if (Verbose1) DebugTf(" ->> new-Timestamp[%s]\r\n", newTime.Timestamp);

  return newTime;

} //  calculateTime()



//===========================================================================================
String buildDateTimeString(const char *timeStamp, int len)
{
  String tmpTS = String(timeStamp);
  String DateTime = "";
  if (len < 12) return String(timeStamp);
  DateTime   = "20" + tmpTS.substring(0, 2);    // YY
  DateTime  += "-"  + tmpTS.substring(2, 4);    // MM
  DateTime  += "-"  + tmpTS.substring(4, 6);    // DD
  DateTime  += " "  + tmpTS.substring(6, 8);    // HH
  DateTime  += ":"  + tmpTS.substring(8, 10);   // MM
  DateTime  += ":"  + tmpTS.substring(10, 12);  // SS
  return DateTime;

} // buildDateTimeString()


//===========================================================================================
void epochToTimestamp(time_t t, char *ts, int8_t len)
{
  if (len < 12)
  {
    strlcpy(ts, "Error", len);
    return;
  }
  //------------yy  mm  dd  hh  mm  ss
  snprintf(ts, len, "%02d%02d%02d%02d%02d%02d", localtime(&now)->tm_year-2000, localtime(&now)->tm_mon, localtime(&now)->tm_mday
          , localtime(&now)->tm_hour, localtime(&now)->tm_min, localtime(&now)->tm_sec);

} // epochToTimestamp()

//===========================================================================================
int8_t MinuteFromTimestamp(const char *timeStamp)
{
  char aMM[4] = "";
  // 0123456789ab
  // YYMMDDHHmmss MM = 8-9
  strCpyFrm(aMM, 4, timeStamp, 8, 9);
  return String(aMM).toInt();

} // MinuteFromTimestamp()

//===========================================================================================
int8_t HourFromTimestamp(const char *timeStamp)
{
  char aHH[4] = "";
  //DebugTf("timeStamp[%s] => \r\n", timeStamp); // YYMMDDHHmmss HH = 5-6
  strCpyFrm(aHH, 4, timeStamp, 6, 7);
  //Debugf("aHH[%s], nHH[%02d]\r\n", aHH, String(aHH).toInt());
  return String(aHH).toInt();

} // HourFromTimestamp()

//===========================================================================================
int8_t DayFromTimestamp(const char *timeStamp)
{
  char aDD[4] = "";
  // 0123456789ab
  // YYMMDDHHmmss DD = 4-5
  strCpyFrm(aDD, 4, timeStamp, 4, 5);
  return String(aDD).toInt();

} // DayFromTimestamp()

//===========================================================================================
int8_t MonthFromTimestamp(const char *timeStamp)
{
  char aMM[4] = "";
  // 0123456789ab
  // YYMMDDHHmmss MM = 2-3
  strCpyFrm(aMM, 4, timeStamp, 2, 3);
  return String(aMM).toInt();

} // MonthFromTimestamp()

//===========================================================================================
int8_t YearFromTimestamp(const char *timeStamp)
{
  char aYY[4] = "";
  // 0123456789ab
  // YYMMDDHHmmss YY = 0-1
  strCpyFrm(aYY, 4, timeStamp, 0, 1);
  return String(aYY).toInt();

} // YearFromTimestamp()


//===========================================================================================
// calculate epoch from timeStamp
// if syncTime is true, set system time to calculated epoch-time
time_t epoch(const char *timeStamp, int8_t len, bool syncTime)
{
  char fullTimeStamp[_TIMESTAMP_LEN] = {0};

  DebugTf("calculate epoch() from [%s]\r\n", timeStamp);

  strlcat(fullTimeStamp, timeStamp, _TIMESTAMP_LEN);
  if (Verbose2) 
    DebugTf("epoch(%s) strlen([%d])\r\n", fullTimeStamp, strlen(fullTimeStamp));
  switch(strlen(fullTimeStamp))
  {
    case  4:  //--- timeStamp is YYMM
      //trConcat(fullTimeStamp, 15, "01010101X");
      strlcat(fullTimeStamp, "01010101", _TIMESTAMP_LEN);
      break;
    case  6:  //--- timeStamp is YYMMDD
      strlcat(fullTimeStamp, "010101", _TIMESTAMP_LEN);
      break;
    case  8:  //--- timeStamp is YYMMDDHH
      strlcat(fullTimeStamp, "0101", _TIMESTAMP_LEN);
      break;
    case  10:  //--- timeStamp is YYMMDDHHMM
      strlcat(fullTimeStamp, "01", _TIMESTAMP_LEN);
      break;
    case  12:  //--- timeStamp is YYMMDDHHMMSS
      //strlcat(fullTimeStamp, "X", _TIMESTAMP_LEN);
      break;
      //default:  return now();
  }

  if (Verbose2) DebugTf("DateTime: [%02d]-[%02d]-[%02d] [%02d]:[%02d]:[%02d]\r\n"
                          , DayFromTimestamp(timeStamp)
                          , MonthFromTimestamp(timeStamp)
                          , YearFromTimestamp(timeStamp)
                          , HourFromTimestamp(timeStamp)
                          , MinuteFromTimestamp(timeStamp)
                          , 0
                         );
  if (   (MonthFromTimestamp(timeStamp) >  5)
      && (MonthFromTimestamp(timeStamp) < 10)
     )  
        strlcat(fullTimeStamp, "S", _TIMESTAMP_LEN);
  else  strlcat(fullTimeStamp, "W", _TIMESTAMP_LEN);

  if (strlen(fullTimeStamp) < 13) return time(0);  // Error

  time_t nT;
  time_t savEpoch = time(0);  // Save current epoch time

  return nT;

} // epoch()


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
ipGateway[0*
***************************************************************************/