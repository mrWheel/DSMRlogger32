/*
***************************************************************************
**  Program  : processTelegram, part of DSMRlogger32
**  Version  : v5.n
**
**  Copyright (c) 2020 .. 2023 Willem Aandewiel
**
**  TERMS OF USE: MIT License. See bottom of file.
***************************************************************************
*/
//==================================================================================
void processTelegram()
{
  char record[DATA_RECLEN + 1] = {0};
  
  DebugTf("Telegram[%d]=>tlgrmData.timestamp[%s]\r\n", telegramCount
          , tlgrmData.timestamp.c_str());
  digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
  
  //-- switch last->prev, new is last!!
  saveTimestamp(tlgrmData.timestamp.c_str());
  
  //----- update OLED display ---------
  if (devSetting->OledType > 0)
  {
    String DT   = buildDateTimeString(tlgrmData.timestamp.c_str(), sizeof(tlgrmData.timestamp));

    snprintf(gMsg,  _GMSG_LEN, "%s - %s", DT.substring(0, 10).c_str(), DT.substring(11, 16).c_str());
    oled_Print_Msg(0, gMsg, 0);
    snprintf(gMsg,  _GMSG_LEN, "-Power%7d Watt", (int)(tlgrmData.power_delivered *1000));
    oled_Print_Msg(1, gMsg, 0);
    snprintf(gMsg,  _GMSG_LEN, "+Power%7d Watt", (int)(tlgrmData.power_returned *1000));
    oled_Print_Msg(2, gMsg, 0);
  }

  //--- Skip first 2 telegrams .. just to settle down a bit ;-)
  if ((int32_t)((telegramCount - telegramsAtStart - telegramErrors) < 2) )
  {
    DebugTf("Skip telegram [%d]\r\n", telegramCount);
    return;
  }

  pushTlgrmToActualStore();
  
  //-- if the Year changes update all three RING files
  if ((lastTlgrmTime.Year != prevTlgrmTime.Year) || !updatedRINGfiles)
  {
    buildDataRecordFromSM(record, prevTlgrmTime);
    updatedRINGfiles = true;
    DebugTf("Update MONTHS, DAYS & HOURS RING-files..[%02d-%02d-%02d (%02d)] Changed Year! [%d/%d]\r\n" 
                                                            , prevTlgrmTime.Year
                                                            , prevTlgrmTime.Month
                                                            , prevTlgrmTime.Day
                                                            , prevTlgrmTime.Hour
                                                            , prevTlgrmTime.Year
                                                            , lastTlgrmTime.Year);
    writeToSysLog("Update RING-files [%02d-%02d-%02d (%02d)] Changed Year! [%d/%d] - MONTHS, DAYS & HOURS"
                                                            , prevTlgrmTime.Year
                                                            , prevTlgrmTime.Month
                                                            , prevTlgrmTime.Day
                                                            , prevTlgrmTime.Hour
                                                            , prevTlgrmTime.Year
                                                            , lastTlgrmTime.Year);
    //-- Update all three RING files
    writeDataToRingFile(HOURS_FILE,  RNG_HOURS,  record, prevTlgrmTime);
    writeDataToRingFile(DAYS_FILE,   RNG_DAYS,   record, prevTlgrmTime);
    writeDataToRingFile(MONTHS_FILE, RNG_MONTHS, record, prevTlgrmTime);
  }
  else if (lastTlgrmTime.Month != prevTlgrmTime.Month)  
  {
    buildDataRecordFromSM(record, prevTlgrmTime);
    DebugTf("Update MONTHS, DAYS & HOURS RING-files..[%02d-%02d-%02d (%02d)] Changed Month! [%d/%d]\r\n" 
                                                            , prevTlgrmTime.Year
                                                            , prevTlgrmTime.Month
                                                            , prevTlgrmTime.Day
                                                            , prevTlgrmTime.Hour
                                                            , prevTlgrmTime.Month
                                                            , lastTlgrmTime.Month);
    writeToSysLog("Update RING-files [%02d-%02d-%02d (%02d)] Changed Month! [%d/%d] - MONTHS, DAYS & HOURS"
                                                            , prevTlgrmTime.Year
                                                            , prevTlgrmTime.Month
                                                            , prevTlgrmTime.Day
                                                            , prevTlgrmTime.Hour
                                                            , prevTlgrmTime.Month
                                                            , lastTlgrmTime.Month);
    //-- If the Month changes update all three RING files
    writeDataToRingFile(HOURS_FILE,  RNG_HOURS,  record, prevTlgrmTime);
    writeDataToRingFile(DAYS_FILE,   RNG_DAYS,   record, prevTlgrmTime);
    writeDataToRingFile(MONTHS_FILE, RNG_MONTHS, record, prevTlgrmTime);
  }
  else if (lastTlgrmTime.Day   != prevTlgrmTime.Day) 
  {
    buildDataRecordFromSM(record, prevTlgrmTime);
    DebugTf("Update DAYS & HOURS RING-files..[%02d-%02d-%02d (%02d)] Changed Day! [%d/%d]\r\n" 
                                                            , prevTlgrmTime.Year
                                                            , prevTlgrmTime.Month
                                                            , prevTlgrmTime.Day
                                                            , prevTlgrmTime.Hour
                                                            , prevTlgrmTime.Day
                                                            , lastTlgrmTime.Day);
    writeToSysLog("Update RING-files [%02d-%02d-%02d (%02d)] Changed Day! [%d/%d] - DAYS & HOURS"
                                                            , prevTlgrmTime.Year
                                                            , prevTlgrmTime.Month
                                                            , prevTlgrmTime.Day
                                                            , prevTlgrmTime.Hour
                                                            , prevTlgrmTime.Day
                                                            , lastTlgrmTime.Day);
    //-- If the Day changes update only these two RING files
    writeDataToRingFile(HOURS_FILE, RNG_HOURS,  record, prevTlgrmTime);
    writeDataToRingFile(DAYS_FILE,  RNG_DAYS,   record, prevTlgrmTime);
  }
  else if (lastTlgrmTime.Hour != prevTlgrmTime.Hour)
  {
    buildDataRecordFromSM(record, prevTlgrmTime);
    DebugTf("Update HOURS RING-files..[%02d-%02d-%02d (%02d)] Changed Hour! [%d/%d]\r\n" 
                                                            , prevTlgrmTime.Year
                                                            , prevTlgrmTime.Month
                                                            , prevTlgrmTime.Day
                                                            , prevTlgrmTime.Hour
                                                            , prevTlgrmTime.Hour
                                                            , lastTlgrmTime.Hour);
    /* 
    writeToSysLog("Update HOURS RING-files [%02d-%02d-%02d (%02d)] Changed Hour! [%d/%d]"
                                                            , prevTlgrmTime.Year
                                                            , prevTlgrmTime.Month
                                                            , prevTlgrmTime.Day
                                                            , prevTlgrmTime.Hour
                                                            , prevTlgrmTime.Hour
                                                            , lastTlgrmTime.Hour);
    */
    //-- update all three on every hour change!
    writeDataToRingFile(HOURS_FILE,  RNG_HOURS,  record, prevTlgrmTime);
    writeDataToRingFile(DAYS_FILE,   RNG_DAYS,   record, prevTlgrmTime);
    writeDataToRingFile(MONTHS_FILE, RNG_MONTHS, record, prevTlgrmTime);
  }
  DebugTf("prevHour[%02d] -- lastHour[%02d] ", prevTlgrmTime.Hour, lastTlgrmTime.Hour);
  if (prevTlgrmTime.Hour != lastTlgrmTime.Hour)
  {
    Debugln("Changed!!");
    buildDataRecordFromSM(record, lastTlgrmTime);
    //-- update all three on every hour change!
    writeDataToRingFile(HOURS_FILE,  RNG_HOURS,  record, lastTlgrmTime);
    writeDataToRingFile(DAYS_FILE,   RNG_DAYS,   record, lastTlgrmTime);
    writeDataToRingFile(MONTHS_FILE, RNG_MONTHS, record, lastTlgrmTime);
    DebugTln("write last status..");
    writeLastStatus();
  }
  else
  {
    Debugln();
  }

  if ( DUE(publishMQTTtimer) )
  {
    sendMQTTData();
  }

} // processTelegram()


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
