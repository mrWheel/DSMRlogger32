/*
***************************************************************************
**  Program  : timeStuff, part of DSMRlogger32
**  Version  : v5.n
**
**  Copyright (c) 2020 .. 2024 Willem Aandewiel
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
  if (prevTlgrmTime.Year < 2000)
  {
    prevTlgrmTime = buildTimeStruct(lastTlgrmTime.Timestamp, devSetting->NoHourSlots
                                                         , devSetting->NoDaySlots 
                                                         , devSetting->NoMonthSlots);
  }
  //-- fill all other fields
  strlcpy(lastTlgrmTime.Timestamp, timeStamp, _TIMESTAMP_LEN);
  lastTlgrmTime = buildTimeStruct(lastTlgrmTime.Timestamp, devSetting->NoHourSlots
                                                         , devSetting->NoDaySlots 
                                                         , devSetting->NoMonthSlots);
  DebugTf("prevTlgrmTime[%s], lastTlgrmTime[%s]\r\n", prevTlgrmTime.Timestamp, lastTlgrmTime.Timestamp);

} //  saveTimestamp()


//-- timeStruct
//struct timeStruct {
//    char Timestamp[13];   //-- "YYMMDDHHMMSS" with null terminator
//    int Year;             //-- EEYY
//    int Month;
//    int Day;
//    int Hour;
//    int Minute;
//    int Second;
//    time_t epoch;
//    int Weekday;   // Day of the week (0-6, where 0 = Sunday)
//    int Months;
//    int monthSlot;
//    int daysHist;
//    int daySlot;
//    int hoursHist;
//    int hourSlot;
//    int Days;
//    int Hours;
//    int monthsHist;
//};

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
} // fillMissingTimestamp()

//===========================================================================================
// Returns the number of days in a given month of a given year.
// 
// Takes into account leap years for February.
// 
// @param year The year to consider.
// @param month The month to consider (0-based, i.e. 0 for January, 2 for February, etc.).
// @return The number of days in the given month of the given year.
int daysInMonth(int year, int month) 
{
    static const int days[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (month == 1) //-- February
    { 
        if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)) {
            return 29;
        }
    }
    return days[month];

} // daysInMonth()

//===========================================================================================
// Helper function to calculate days since epoch (January 1, 1970)
int daysSinceEpoch(int year, int month, int day) 
{
    int days = 0;
    for (int y = 1970; y < year; ++y) 
    {
        days += 365 + (y % 4 == 0 && (y % 100 != 0 || y % 400 == 0));
    }
    for (int m = 0; m < month - 1; ++m) 
    {
        days += daysInMonth(year, m);
    }
    return days + day - 1;

} // daysSinceEpoch()

//===========================================================================================
timeStruct calculateTime(timeStruct useTime, int16_t units, int8_t ringType)
{
  timeStruct newTime = useTime;  // Initialize newTime with useTime to keep all fields

  if (Verbose2) DebugTf("calculateTime[%s], units[%d], ringType[%d]\n", useTime.Timestamp, units, ringType);
  
  // Parse useTime.Timestamp
  int year, month, day, hour, minute, second;
  if (sscanf(useTime.Timestamp, "%2d%2d%2d%2d%2d%2d", &year, &month, &day, &hour, &minute, &second) != 6) {
    DebugTf("Error: Invalid Timestamp format\n");
    return useTime;  // Return original time if parsing fails
  }

  // Adjust year to full 4-digit year 
  year += 2000;

  // Adjust time based on units and ringType
  switch(ringType)
  {
    case RNG_HOURS:
        hour += units;
        while (hour >= 24) 
        {
            hour -= 24;
            day++;
            //-??- if (day > daysInMonth(year, month - 1)) 
            if (day > daysInMonth(year, month)) 
            {
                day = 1;
                month++;
                if (month > 12) {
                    month = 1;
                    year++;
                }
            }
        }
        while (hour < 0) 
        {
            hour += 24;
            day--;
            if (day < 1) 
            {
                month--;
                if (month < 1) 
                {
                    month = 12;
                    year--;
                }
                day = daysInMonth(year, month - 1);
            }
        }
        break;
    case RNG_DAYS:
        day += units;
        while (day > daysInMonth(year, month - 1)) 
        {
            day -= daysInMonth(year, month - 1);
            month++;
            if (month > 12) 
            {
                month = 1;
                year++;
            }
        }
        while (day < 1) 
        {
            month--;
            if (month < 1) 
            {
                month = 12;
                year--;
            }
            day += daysInMonth(year, month - 1);
        }
        break;
    case RNG_MONTHS:
        month += units;
        while (month > 12) 
        {
            month -= 12;
            year++;
        }
        while (month < 1) 
        {
            month += 12;
            year--;
        }
        // Adjust for month lengths
        if (day > daysInMonth(year, month - 1)) 
        {
            day = daysInMonth(year, month - 1);
        }
        break;
    default:
        DebugTf("Error: Invalid ringType\n");
        return useTime;  // Return original time if ringType is invalid
  }

  // Fill in newTime struct
  newTime.Year    = year;
  newTime.Month   = month;
  newTime.Day     = day;
  newTime.Hour    = hour;
  newTime.Minute  = minute;
  newTime.Second  = second;

  // Calculate epoch time (UTC)
  int days = daysSinceEpoch(year, month, day);
  newTime.epoch = (days * 86400) + (hour * 3600) + (minute * 60) + second;

  // Calculate weekday (1 = Sunday, 7 = Saturday)
  newTime.Weekday = ((days + 4) % 7) + 1;  // January 1, 1970 was a Thursday (4)

  //-- subract 1 from Year is for compatibility with the Arduino version
  newTime.Months      = ((newTime.Year -1) * 12) + newTime.Month;
  newTime.monthsHist  = devSetting->NoMonthSlots;
  newTime.monthSlot   = (newTime.Months % newTime.monthsHist);
  newTime.Days        = newTime.epoch / SECS_PER_DAY;
  newTime.daysHist    = devSetting->NoDaySlots;
  newTime.daySlot     = (newTime.Days % newTime.daysHist);
  newTime.Hours       = newTime.epoch / SECS_PER_HOUR;
  newTime.hoursHist   = devSetting->NoHourSlots;
  newTime.hourSlot    = (newTime.Hours % newTime.hoursHist);

  if (Verbose2) DebugTf("old.timeStamp[%s]\r\n", useTime.Timestamp);

  snprintf(newTime.Timestamp, _TIMESTAMP_LEN, "%02d%02d%02d%02d%02d%02d"
                                        , newTime.Year % 100
                                        , newTime.Month
                                        , newTime.Day
                                        , newTime.Hour
                                        , newTime.Minute
                                        , newTime.Second);

  if (Verbose2) 
      DebugTf("new.timeStamp[%s], hour[%2d], hourSlot[%2d], day[%2d], daySlot[%2d], month[%2d], monthSlot[%2d]\r\n"
                                                                              , newTime.Timestamp
                                                                              , newTime.Hour
                                                                              , newTime.hourSlot
                                                                              , newTime.Day
                                                                              , newTime.daySlot
                                                                              , newTime.Month
                                                                              , newTime.monthSlot);

  if (Verbose1)
      DebugTf("new.timeStamp[%s], hours[%d], hist[%d], days[%d], hist[%d], months[%d], hist[%d]\r\n", newTime.Timestamp
                                                                  , newTime.Hours
                                                                  , newTime.hoursHist
                                                                  , newTime.Days
                                                                  , newTime.daysHist
                                                                  , newTime.Months
                                                                  , newTime.monthsHist);

  return newTime;

} //  calculateTime()


//===========================================================================================
timeStruct buildTimeStruct(const char *timeStamp, uint16_t maxHourSlots, uint16_t maxDaySlots, uint16_t maxMonthSlots) 
{
    struct tm  timeInfo = {0};
    timeStruct thisTime = {0};

    if (Verbose2)
        DebugTf("--->given.timeStamp[%s], maxHourSlot[%2d], maxDaySlot[%2d], maxMonthSlot[%2d]\r\n", timeStamp
                                                                                   , maxHourSlots
                                                                                   , maxDaySlots
                                                                                   , maxMonthSlots);
    
    // Ensure valid slots
    if (maxHourSlots  < _NO_HOUR_SLOTS_)   maxHourSlots   = _NO_HOUR_SLOTS_;
    if (maxDaySlots   < _NO_DAY_SLOTS_)    maxDaySlots    = _NO_DAY_SLOTS_;
    if (maxMonthSlots < _NO_MONTH_SLOTS_)  maxMonthSlots  = _NO_MONTH_SLOTS_;
    
    // Copy the timestamp and fill missing parts
    strncpy(thisTime.Timestamp, timeStamp, _TIMESTAMP_LEN - 1);
    thisTime.Timestamp[_TIMESTAMP_LEN - 1] = '\0';
    fillMissingTimestamp(thisTime.Timestamp);

    thisTime = calculateTime(thisTime, 0, RNG_HOURS);

    if (Verbose1)
        DebugTf("returning.timeStamp[%s],    hourSlot[%2d],    daySlot[%2d],    monthSlot[%2d]\r\n", thisTime.Timestamp
                                                                                      , thisTime.hourSlot
                                                                                      , thisTime.daySlot
                                                                                      , thisTime.monthSlot);
    return thisTime;
    
} //  buildTimeStruct()


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