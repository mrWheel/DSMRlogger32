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


//===========================================================================================
void logNtpTime()
{
  DebugTf("log NTP Date/Time: %02d-%02d-%04d %02d:%02d:%02d\r\n"
                                             , tzEurope.day()
                                             , tzEurope.month()
                                             , tzEurope.year()
                                             , tzEurope.hour()
                                             , tzEurope.minute()
                                             , tzEurope.second());
  writeToSysLog("NTP Date/Time: %02d-%02d-%04d %02d:%02d:%02d"
                                             , tzEurope.day()
                                             , tzEurope.month()
                                             , tzEurope.year()
                                             , tzEurope.hour()
                                             , tzEurope.minute()
                                             , tzEurope.second());
  ntpEventId = setEvent(logNtpTime, now()+3600);

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

/**
  DebugTf("nums[%02d][%02d][%02d] - [%02d][%02d][%02d]\r\n"
                          , thisTime.Year, thisTime.Month, thisTime.Day
                          , thisTime.Hour, thisTime.Minute, thisTime.Second);
**/ 
  //-- save Europe epoch ..
  time_t  epochEurope = tzEurope.now();
  Timezone tzSM;
  //-- set SM time
  tzSM.setTime(thisTime.Hour, thisTime.Minute, thisTime.Second,
               thisTime.Day,  thisTime.Month,  thisTime.Year);
  //-- create SM epoch .. and other fields
  thisTime.epoch      = tzSM.now();
  thisTime.Weekday    = tzSM.weekday();  //-- 1 = Sunday
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


//===========================================================================================
timeStruct calculateTime(timeStruct useTime, int16_t units, int8_t ringType)
{
  int16_t orgUnit = 0;
  timeStruct tmpTime = useTime;
  timeStruct newTime = useTime;
  //-- save Europe epoch ..
  time_t   epochEurope = tzEurope.now();
  Timezone tzTmp;
  
  switch(ringType)
  {
    case RNG_HOURS:
        tmpTime.epoch += (units * SECS_PER_HOUR);
        tzTmp.setTime(tmpTime.epoch);
        break;
    case RNG_DAYS:
        tmpTime.epoch += (units * SECS_PER_DAY);
        tzTmp.setTime(tmpTime.epoch);
        break;
    case RNG_MONTHS:
        {
          //DebugTf("[---] Year[%02d], Month[%02d] - ", tmpTime.Year, tmpTime.Month);
          int16_t newMonth = tmpTime.Month + units;
          //Debugf("[add] [%d]units -> [%d]\r\n", units, newMonth);

          if (units < 0)
          {
            while(newMonth < 1) { tmpTime.Year--; newMonth+=12; }
            //DebugTf("[sub] Year[%02d], Month[%02d]\r\n", tmpTime.Year, newMonth);
          }
          else if (units > 0)
          {
            while(newMonth > 12) { tmpTime.Year++; newMonth-=12; }
            //DebugTf("[add] Year[%02d], Month[%02d]\r\n", tmpTime.Year, newMonth);
          }
          tmpTime.Month = newMonth;
          //-- set tmp time
          tzTmp.setTime(tmpTime.Hour, tmpTime.Minute, tmpTime.Second,
                          tmpTime.Day,  tmpTime.Month,  tmpTime.Year);
          tmpTime.epoch = tzTmp.now();
        }
        break;
  }
  //-- create TMP epoch .. and other fields
  tmpTime.Month     = tzTmp.month();
  tmpTime.Months    = ((tzTmp.year() -1) * 12) + tzTmp.month();
  tmpTime.monthSlot = (tmpTime.Months % useTime.monthsHist);
  tmpTime.Day       = tzTmp.day();
  tmpTime.Days      = (tzTmp.now() / SECS_PER_DAY);
  tmpTime.daySlot   = (tmpTime.Days % useTime.daysHist);
  tmpTime.Hour      = tzTmp.hour();
  tmpTime.Hours     = (tzTmp.now() / SECS_PER_HOUR);
  tmpTime.hourSlot  = (tmpTime.Hours % useTime.hoursHist);
  tmpTime.Minute    = tzTmp.minute();
  tmpTime.Second    = tzTmp.second();

  snprintf(tmpTime.Timestamp, _TIMESTAMP_LEN, "%02d%02d%02d%02d%02d%02dX"
                                        , tmpTime.Year
                                        , tmpTime.Month
                                        , tmpTime.Day
                                        , tmpTime.Hour
                                        , tmpTime.Minute
                                        , tmpTime.Second);
  //-- reset Europe time
  tzEurope.setTime(epochEurope);

  if (Verbose1) DebugTf(" ->> new-Timestamp[%s]\r\n", tmpTime.Timestamp);

  return tmpTime;

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
  snprintf(ts, len, "%02d%02d%02d%02d%02d%02d", year(t)-2000, month(t), day(t)
          , hour(t), minute(t), second(t));

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

  if (strlen(fullTimeStamp) < 13) return now();

  time_t nT;
  time_t savEpoch = now();

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
