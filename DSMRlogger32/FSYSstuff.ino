/*
***************************************************************************
**  Program  : FSYSstuff, part of DSMRlogger32
**  Version  : v5.n
**
**  Copyright (c) 2020 .. 2023 Willem Aandewiel
**
**  TERMS OF USE: MIT License. See bottom of file.
***************************************************************************
*/
struct listFileStruct
{
  char    Name[32];
  int32_t Size;
};

int16_t   bytesWritten, bytesRead;

//====================================================================
void readLastStatus()
{
  char buffer[100]  = {0};
  char dummy[50]    = {0};
  char spiffsTimestamp[20] = {0};

  File _file = _FSYS.open(_STATUS_FILE, "r");
  if (!_file)
  {
    DebugTf("read(): No [%s] found ..\r\n", _STATUS_FILE);
    writeLastStatus();
    return;
  }
  if(_file.available())
  {
    bytesRead = _file.readBytesUntil('\n', buffer, sizeof(buffer));
    if (Verbose2) DebugTf("read lastUpdate[%s]\r\n", buffer);
    sscanf(buffer, "%[^;]; %u; %u; %u; %[^;]"
                                          , spiffsTimestamp
                                          , &nrReboots
                                          , &slotErrors
                                          , &telegramCount
                                          , dummy);
    if (Verbose1)
    {
      DebugTf("values timestamp[%s], nrReboots[%u], slotErrors[%u], telegramCount[%u], dummy[%s]\r\n"
                                          , spiffsTimestamp
                                          , nrReboots
                                          , slotErrors
                                          , telegramCount
                                          , dummy);
    }
    yield();
  }
  _file.close();
  if (strlen(spiffsTimestamp) != 13)
  {
    strlcpy(spiffsTimestamp, "220101010101X", sizeof(spiffsTimestamp));
  }
  telegramsAtStart = telegramCount;
  snprintf(lastTlgrmTime.Timestamp, _TIMESTAMP_LEN, "%s", spiffsTimestamp);

}  // readLastStatus()


//====================================================================
void writeLastStatus()
{
  if (ESP.getFreeHeap() < 8500)   // to prevent firmware from crashing!
  {
    DebugTf("Bailout due to low heap (%d bytes)\r\n", ESP.getFreeHeap());
    //esp32 writeToSysLog("Bailout low heap (%d bytes)", ESP.getFreeHeap());
    return;
  }
  char buffer[100] = "";
  DebugTf("writeLastStatus() => %s; %u; %u; %u;\r\n"  , lastTlgrmTime.Timestamp
                                                      , nrReboots
                                                      , slotErrors
                                                      , telegramCount);
  File _file = _FSYS.open(_STATUS_FILE, "w");
  if (!_file)
  {
    DebugTf("write(): No [%s] found ..\r\n", _STATUS_FILE);
  }
  snprintf(buffer, sizeof(buffer), "%-13.13s; %010u; %010u; %010u; %s;\n"
                                                    , lastTlgrmTime.Timestamp
                                                    , nrReboots
                                                    , slotErrors
                                                    , telegramCount
                                                    , "meta data");
  _file.print(buffer);
  _file.flush();
  _file.close();

} // writeLastStatus()


//===========================================================================================
void buildDataRecordFromSM(char *recIn, timeStruct useTime)
{
  static float GG = 1;
  char record[DATA_RECLEN + 1] = {0};
  char key[10] = {0};

  //32 uint16_t recSlot = timestampToHourSlot(useTime);
  uint16_t recSlot = (useTime.Hours % devSetting->NoHourSlots);
  strCpyFrm(key, 10, useTime.Timestamp, 0, 8);

  snprintf(record, sizeof(record), (char *)DATA_FORMAT, key
                      , (float)tlgrmData.energy_delivered_tariff1
                      , (float)tlgrmData.energy_delivered_tariff2
                      , (float)tlgrmData.energy_returned_tariff1
                      , (float)tlgrmData.energy_returned_tariff2
                      , (float)gasDelivered);
  
  if (Verbose2) DebugTf("record[%s]\r\n", record);
  
  fillRecord(record, DATA_RECLEN);
  strlcpy(recIn, record, DATA_RECLEN);

} // buildDataRecordFromSM()


//===========================================================================================
uint16_t buildDataRecordFromJson(char *recIn, int recLen, String jsonIn)
{
  char      record[DATA_RECLEN + 1] = "";
  String    wOut[10];
  String    wPair[5];
  char      uKey[15]  = "";
  float     uEDT1     = 0.0;
  float     uEDT2     = 0.0;
  float     uERT1     = 0.0;
  float     uERT2     = 0.0;
  float     uGDT      = 0.0;
  uint16_t  recSlot;

  DebugTln(jsonIn);

  //-- Allocate the JsonDocument
  DynamicJsonDocument  doc(3000);
  DeserializationError err = deserializeJson(doc, jsonIn);
  serializeJson(doc, jsonBuff, _JSONBUFF_LEN);
  //-dbg-Debugln(jsonBuff);

  strlcpy(uKey, doc["recid"], sizeof(uKey));
  uEDT1 = doc["edt1"];
  uEDT2 = doc["edt2"];
  uERT1 = doc["ert1"];
  uERT2 = doc["ert2"];
  uGDT  = doc["gdt"];
  snprintf(record, sizeof(record), (char *)DATA_FORMAT, uKey, (float)uEDT1
                                                            , (float)uEDT2
                                                            , (float)uERT1
                                                            , (float)uERT2
                                                            , (float)uGDT);
  Debugln(record);
  
  strlcat(uKey, "0101X", _TIMESTAMP_LEN);
  //recSlot = timestampToMonthSlot(uKey, strlen(uKey));
  timeStruct thisTimestamp;
  strlcpy(thisTimestamp.Timestamp, uKey, _TIMESTAMP_LEN);
  //recSlot = timestampToMonthSlot(uKey);
  recSlot = (thisTimestamp.Months % devSetting->NoMonthSlots);

  DebugTf("MONTHS: Write [%s] to slot[%02d] in %s\r\n", uKey, recSlot, MONTHS_FILE);
  snprintf(record, sizeof(record), (char *)DATA_FORMAT, uKey, (float)uEDT1
                                                            , (float)uEDT2
                                                            , (float)uERT1
                                                            , (float)uERT2
                                                            , (float)uGDT);

  // DATA + \n + \0
  fillRecord(record, DATA_RECLEN);

  strlcpy(recIn, record, recLen);

  return recSlot;

} // buildDataRecordFromJson()


//===========================================================================================
void writeDataToRingFile(char *fileName, int8_t ringType, char *record, timeStruct slotTime)
{
  uint16_t offset = 0;
  uint16_t slotNr = 0;

  fillRecord(record, DATA_RECLEN);

  if (!isValidTimestamp(record, 8))
  {
    DebugTf("timeStamp [%-13.13s] not valid\r\n", record);
    writeToSysLog("timeStamp [%-13.13s] not valid\r\n", record);
    slotErrors++;
    return;
  }

  if (!_FSYS.exists(fileName))
  {
    DebugTf("RING bestand [%s] niet gevonden --> create!\r\n", fileName);
    writeToSysLog("RING bestand [%s] niet gevonden --> create!", fileName);

    switch(ringType)
    {
      case RNG_HOURS:
        DebugTf("slotTime.Hours[%3d], devSetting[%3d]\r\n", slotTime.Hours
                                                          , devSetting->NoHourSlots);
        createRingFile(fileName, slotTime, ringType, devSetting->NoHourSlots);
        break;
      case RNG_DAYS:
        DebugTf("slotTime.Days[%3d], devSetting[%3d]\r\n", slotTime.Days
                                                         , devSetting->NoDaySlots);
        createRingFile(fileName, slotTime, ringType, devSetting->NoDaySlots);
        break;
      case RNG_MONTHS:
        DebugTf("slotTime.Months[%3d], devSetting[%3d]\r\n", slotTime.Months
                                                           , devSetting->NoMonthSlots);
        createRingFile(fileName, slotTime, ringType, devSetting->NoMonthSlots);
        break;
    }
  }

  File dataFile = _FSYS.open(fileName, "r+");  // read and write ..
  if (!dataFile)
  {
    DebugTf("Error opening [%s]\r\n", fileName);
    writeToSysLog("Error opening [%s]\r\n", fileName);
    return;
  }

  //-- do some logging
  switch(ringType)
  {
    case RNG_HOURS:
      slotNr = slotTime.hourSlot;
      DebugTf("HOURS:  Write [%-8.8s] to slot[%02d]\r\n", slotTime.Timestamp, slotNr);
      //writeToSysLog("HOURS:  Write [%-8.8s] to slot[%02d]\r\n", slotTime.Timestamp, slotNr);
      break;
    case RNG_DAYS:
      slotNr = slotTime.daySlot;
      DebugTf("DAYS:   Write [%-6.6s] to slot[%02d]\r\n", slotTime.Timestamp, slotNr);
      //writeToSysLog("DAYS:   Write [%-6.6s]   to slot[%02d]\r\n", slotTime.Timestamp, slotNr);
      break;
    case RNG_MONTHS:
      slotNr = slotTime.monthSlot;
      DebugTf("MONTHS: Write [%-4.4s]   to slot[%02d]\r\n", slotTime.Timestamp, slotNr);
      //writeToSysLog("MONTHS: Write [%-4.4s]     to slot[%02d]\r\n", slotTime.Timestamp, slotNr);
      break;
  }
  //-- slot goes from 0 to _NO_OF_SLOTS_
  //-- we need to add 1 to slot to skip header record!
  offset = ((slotNr + 1) * (DATA_RECLEN +1));
  dataFile.seek(offset, SeekSet);
  bytesWritten = dataFile.println(record) -1; // don't count '\n'!
  if (bytesWritten != DATA_RECLEN)
  {
    DebugTf("ERROR! slot[%02d]: written [%d] bytes but should have been [%d]\r\n", slotNr, bytesWritten, DATA_RECLEN);
    writeToSysLog("ERROR! slot[%02d]: written [%d] bytes but should have been [%d]", slotNr, bytesWritten, DATA_RECLEN);
  }
  dataFile.close();

} // writeDataToRingFile()


//===========================================================================================
void writeDataToRingFiles(timeStruct useTime)
{
  char record[DATA_RECLEN + 1] = {0};
  
  buildDataRecordFromSM(record, useTime);

  //-- update HOURS
  writeDataToRingFile(HOURS_FILE, RNG_HOURS,  record, useTime);

  //-- update DAYS
  writeDataToRingFile(DAYS_FILE, RNG_DAYS,record, useTime);

  //-- update MONTHS
  writeDataToRingFile(MONTHS_FILE, RNG_MONTHS, record, useTime);

} // writeDataToRingFiles(timeStruct useTime)


//===========================================================================================
void readOneSlot(char *record, const char *fileName
                                        , uint16_t readSlot
                                        , uint16_t maxSlots)
{
  uint16_t  slot, offset;
  char      buffer[DATA_RECLEN +1] = {0};
  float     EDT1, EDT2, ERT1, ERT2, GDT;

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

  slot    = (readSlot % maxSlots);
  // slot goes from 0 to _NO_OF_SLOTS_
  // we need to add 1 to slot to skip header record!
  offset  = ((slot +1) * (DATA_RECLEN +1));
  dataFile.seek(offset, SeekSet);
  bytesRead = dataFile.readBytesUntil('\n', buffer, DATA_RECLEN); 
  strlcpy(record, buffer, DATA_RECLEN);
  if (Verbose2) DebugTf("slot[%02d][%5d] -> [%s]\r\n", slot, offset, buffer);

  if (!isValidTimestamp(buffer, 8))   // first 8 bytes is YYMMDDHH
  {
    {
      //DebugTf("slot[%02d]==>timeStamp [%-8.8s] not valid!!\r\n", slot, buffer);
      if (Verbose1) DebugTf("slot[%02d]==>[%s]\r\n", slot, buffer);
      writeToSysLog("slot[%02d]==>timeStamp [%-8.8s] not valid!!", slot, buffer);
    }
  }
  else
  {
    Debugf("[%02d] %s\r\n", slot, buffer);
  }
  dataFile.close();

} // readOneSlot()


//===========================================================================================
void readAllSlots(char *record, int8_t ringType, const char *fileName, timeStruct thisTime)
{
  int16_t startSlot, endSlot, nrSlots, recNr = 0;
  int16_t typeTime, typeHist, typeStart;

  switch(ringType)
  {
    case RNG_HOURS:
      typeHist    = thisTime.hoursHist;
      typeTime    = thisTime.Hour;
      typeStart   = (thisTime.Hours+1) % typeHist;
      break;
    case RNG_DAYS:
      typeHist    = thisTime.daysHist;
      typeTime    = thisTime.Day;
      typeStart   = (thisTime.Days+1) % typeHist;
      break;
    case RNG_MONTHS:
      typeHist    = thisTime.monthsHist;
      typeTime    = thisTime.Month;
      typeStart   = (thisTime.Months+1) % typeHist;
      break;
  }

  //startSlot += nrSlots;
  DebugTf("[%-8.8s]/[%02d] start[%02d], nrSlots[%d]\r\n", thisTime.Timestamp
                                                        , typeTime
                                                        , typeTime, typeHist);
  for( uint16_t i=0; i<typeHist; i++ )
  {
    int16_t s = (i+typeStart) % typeHist;
    readOneSlot(record, fileName, s, typeHist);
  }

} // readAllSlots()


//===========================================================================================
bool createRingFile(const char *fileName, timeStruct useTime, int8_t ringType)
{
  char       record[DATA_RECLEN + 2]  = {0};
  char       dumpRec[DATA_RECLEN + 2] = {0};  //-- temp. for debugging
  uint16_t   useSlot = 0;
  uint16_t   noSlots;
  timeStruct tmpTime = useTime;
  timeStruct newTime = {0};

  DebugTf("Hours[%d], Days[%d], Months[%d]\r\n", useTime.Hours
                                               , useTime.Days
                                               , useTime.Months);

  DebugTf("Free Psram [%d]bytes\r\n", ESP.getFreePsram() );

  DebugTf("fileName[%s], fileRecLen[%d]", fileName, DATA_RECLEN);
  writeToSysLog("CREATE: fileName[%s], fileRecLen[%d]", fileName, DATA_RECLEN);

  switch(ringType)
  {
    case RNG_HOURS:   noSlots = useTime.Hours;  break;
    case RNG_DAYS:    noSlots = useTime.Days;   break;
    case RNG_MONTHS:  noSlots = useTime.Months; break;
    default:      return false;
  }
  Debugf(", History[%d]\r\n", noSlots);
  
  File dataFile  = _FSYS.open(fileName, "a");  // create File
  // -- first write fileHeader ----------------------------------------
  //-- you cannot modify *fileHeader!!!
  snprintf(gMsg, DATA_RECLEN, (char*)DATA_CSV_HEADER, noSlots);
  fillRecord(gMsg, DATA_RECLEN);
  Debugf("[-1] [%s] [%2d]bytes\r\n", gMsg, strlen(gMsg));
  dataFile.seek(0, SeekSet);
  bytesWritten = dataFile.println(gMsg) -1; // don't count '\n' 
  if (bytesWritten != DATA_RECLEN)
  {
    DebugTf("ERROR!! slotNr[%d]: written [%d] bytes but should have been [%d] for Header\r\n", 0, bytesWritten, DATA_RECLEN);
    writeToSysLog("ERROR!! Header slot: written [%d] bytes but should have been [%d] for Header", bytesWritten, DATA_RECLEN);
  }
  DebugTln(F(".. that went well! Now add data records ..\r"));

  buildDataRecordFromSM(record, useTime);
  DebugTf("record is [%d]bytes\r\n", strlen(record));
  
  //-- first fill file with room for all (noSlots+1) slots
  for(int r = 0; r < noSlots; r++)
  {
    if ((r%10)==0) Debugf("%d ", r);
    else         Debug('.');
    if (Verbose2) DebugTf("Write [%s] Key[%-8.8s], [%2d]bytes\r\n", fileName, record, strlen(record));
    dataFile.seek(((r+1) * (DATA_RECLEN +1)), SeekSet);
    bytesWritten = dataFile.println(record) -1; // don't count '\n' 
    if (bytesWritten != DATA_RECLEN)
    {
      DebugTf("\r\nERROR!! recNo[%d]: written [%d] bytes but should have been [%d] \r\n", r, bytesWritten, DATA_RECLEN);
      writeToSysLog("ERROR!! recNo[%d]: written [%d] bytes but should have been [%d]", r, bytesWritten, DATA_RECLEN);
    }
  }
  Debugln();
  DebugTf("total slots[%3d]\r\n", noSlots);
  dataFile.close();

  //DebugTln("\r\nFile -room- created!");
  //readAllSlots(dumpRec, ringType, fileName, useTime);
  //DebugTln("================================================\r\n");
  
  dataFile = _FSYS.open(fileName, "r+");  // read and write ..

  tmpTime = useTime;

  switch(ringType)
  {
    case RNG_MONTHS:
        newTime = tmpTime; //calculateTime(tmpTime, useTime.Months, ringType);
        useSlot = newTime.monthSlot;
        DebugTf("Start Year[%d], Month[%d] (slot[%d]of[%d])\r\n", newTime.Year
                                                          , newTime.Month
                                                          , useSlot
                                                          , noSlots);
        break;
    case RNG_DAYS:
        newTime = tmpTime; //calculateTime(tmpTime, useTime.Days, ringType);
        useSlot = newTime.daySlot;
        DebugTf("Start Year[%02d], Month[%02d], Day[%02d] (slot[%d]of[%d])\r\n"
                                                          , newTime.Year
                                                          , newTime.Month
                                                          , newTime.Day
                                                          , useSlot
                                                          , noSlots);
        break;
    case RNG_HOURS:
        newTime = tmpTime; //calculateTime(tmpTime, useTime.Hours, ringType);
        useSlot = newTime.hourSlot;
        DebugTf("Start [%02d-%02d-%02d], Hour[%02d] (slot[%d]of[%d])\r\n"
                                                          , newTime.Year
                                                          , newTime.Month
                                                          , newTime.Day
                                                          , newTime.Hour
                                                          , useSlot
                                                          , noSlots);
        break;
  } // switch ..

  fillRecord(record, DATA_RECLEN);

  strlcpy(gMsg, useTime.Timestamp, _TIMESTAMP_LEN);
  
  //-- now fill with the correct key's
  for(int r = 0; r < noSlots; r++)
  {
    if (Verbose1) Debugf("[%3d]slot[%02d] record[%s](%d bytes)\r\n", r, useSlot, record, strlen(record));

    if (Verbose2) 
      DebugTf("Write slot[%02d] Data[%-9.9s][%d]bytes\r\n", useSlot, record, strlen(record));
    dataFile.seek(((useSlot+1) * (DATA_RECLEN +1)), SeekSet);
    bytesWritten = dataFile.println(record) -1; // don't count '\n' 
    if (bytesWritten != DATA_RECLEN)
    {
      DebugTf("ERROR!! recNo[%d]: written [%d] bytes but should have been [%d] \r\n", r, bytesWritten,  DATA_RECLEN);
      writeToSysLog("ERROR!! recNo[%d]: written [%d] bytes but should have been [%d]", r, bytesWritten, DATA_RECLEN);
    }
    //-- change to next slot
    switch(ringType)
    {
      case RNG_MONTHS:
          newTime = calculateTime(newTime, -1, ringType);
          snprintf(gMsg, 15, "%02d%02d0000", newTime.Year, newTime.Month);
          newTime = buildTimeStruct(gMsg, useTime.Hours, useTime.Days, useTime.Months);
          if (Verbose1) DebugTf("new Months Key[%s], Slot[%02d]\r\n", gMsg, newTime.monthSlot);
          useSlot = newTime.monthSlot;
          break;
      case RNG_DAYS:
          newTime = calculateTime(newTime, -1, ringType);
          snprintf(gMsg, 15, "%02d%02d%02d000000", newTime.Year
                                                      , newTime.Month
                                                      , newTime.Day);
          newTime = buildTimeStruct(gMsg, useTime.Hours, useTime.Days, useTime.Months);
          if (Verbose1) DebugTf("new Days Key[%s], Slot[%02d]\r\n", gMsg, newTime.daySlot);
          useSlot = newTime.daySlot;
          break;
      case RNG_HOURS:
          newTime = calculateTime(newTime, -1, ringType);
          snprintf(gMsg, 15, "%02d%02d%02d%02d0000", newTime.Year
                                                        , newTime.Month
                                                        , newTime.Day
                                                        , newTime.Hour);
          newTime = buildTimeStruct(gMsg, useTime.Hours, useTime.Days, useTime.Months);
          if (Verbose1) 
             DebugTf("new Hours Key[%s], Slot[%02d]\r\n", gMsg, newTime.hourSlot);
          useSlot = newTime.hourSlot;
          break;
    } // switch ..
    //-- move new Key to record
    for(int k=0; k<8; k++) { record[k] = gMsg[k]; }
    
  } // for ..

  dataFile.close();

  dataFile  = _FSYS.open(fileName, "r+");       // open for Read & writing
  if (!dataFile)
  {
    DebugTf("Something is very wrong creating [%s]\r\n", fileName);
    writeToSysLog("Something is very wrong creating [%s]", fileName);
    return false;
  }
  else
  {
    writeToSysLog("RING file [%s] created!", fileName);
  }
  dataFile.close();

  return true;

} //  createRingFile()

//===========================================================================================
bool createRingFile(const char *fileName, timeStruct useTime, int8_t ringType, uint16_t noSlots)
{
  useTime.Hours   = noSlots;
  useTime.Days    = noSlots;
  useTime.Months  = noSlots;
  return createRingFile(fileName, useTime, ringType);
  
} //  createRingFile()


//===========================================================================================
bool alterRingFile()
{
  static uint16_t newVal = 0;
  timeStruct  dstLastTime, dstTime;
  char        srcFileName[30]    = {0};
  char        dstFileName[]   = "/RINGdest.csv";
  char        altFileName[30] = {0};
  int8_t      ringType;
  char        cType[10]       = {0};
  char        cKey[15]        = {0};
  uint16_t    offset;
  uint16_t    srcActSlot, srcReadSlot, srcMaxSlots, srcOldestSlot, srcLastSlot;
  uint16_t    dstActSlot, dstReadSlot, dstMaxSlots, dstOldestSlot, dstLastSlot;
  uint16_t    extraSlots, lessSlots;
  bool        doExpand = true;
  char        record[DATA_RECLEN + 1] = {0};

  neoPixOn(1, neoPixWhite);
  glowTimer1 = millis() + 2000;

  //-- build record from last telegram --
  buildDataRecordFromSM(record, lastTlgrmTime);

  tmpNoMonthSlots = (tmpNoMonthSlots * 12) +1;

  if ((tmpNoHourSlots>=_NO_HOUR_SLOTS_) && (devSetting->NoHourSlots != tmpNoHourSlots))
    ringType  = RNG_HOURS;
  else if ((tmpNoDaySlots>=_NO_DAY_SLOTS_) && (devSetting->NoDaySlots != tmpNoDaySlots))
    ringType  = RNG_DAYS;
  else if ((tmpNoMonthSlots>=_NO_MONTH_SLOTS_) && (devSetting->NoMonthSlots != tmpNoMonthSlots))
    ringType  = RNG_MONTHS;
  else
  {
    DebugTln("Did not change any RING file (not all conditions met)");
    writeToSysLog("Did not change any RING file (not all conditions met)");
    tmpNoHourSlots    = devSetting->NoHourSlots;
    tmpNoDaySlots     = devSetting->NoDaySlots;
    tmpNoMonthSlots   = devSetting->NoMonthSlots;
    tmpAlterRingSlots = false;
    return false;
  }

  switch(ringType)
  {
    case RNG_HOURS: {
          writeDataToRingFile(HOURS_FILE, ringType,  record, prevTlgrmTime);
          strlcpy(srcFileName, HOURS_FILE, sizeof(srcFileName));
          snprintf(altFileName, sizeof(altFileName), "/RINGhours_%d.csv", devSetting->NoHourSlots);
          strlcpy(cType, "HOURS", sizeof(cType));
          srcMaxSlots   = devSetting->NoHourSlots;
          srcLastSlot   = (lastTlgrmTime.Hours % srcMaxSlots);
          srcOldestSlot = (((srcLastSlot+srcMaxSlots) + 1) % srcMaxSlots);
          dstMaxSlots   = tmpNoHourSlots;
          DebugTf("Change HOUR history from [%d] to [%d]\r\n", srcMaxSlots, dstMaxSlots);
          writeToSysLog("Change HOUR history from [%d] to [%d] hours", srcMaxSlots, dstMaxSlots);
          //-- save dstLastTime for later use!
          dstLastTime = buildTimeStruct(record, dstMaxSlots, dstMaxSlots, dstMaxSlots);
          dstLastSlot = dstLastTime.hourSlot;
        }
        break;
    case RNG_DAYS: {
          writeDataToRingFile(DAYS_FILE, ringType, record, prevTlgrmTime);
          strlcpy(srcFileName, DAYS_FILE, sizeof(srcFileName));
          snprintf(altFileName, sizeof(altFileName), "/RINGdays_%d.csv", devSetting->NoDaySlots);
          strlcpy(cType, "DAYS", sizeof(cType));
          srcMaxSlots   = devSetting->NoDaySlots;
          srcLastSlot   = (lastTlgrmTime.Days % srcMaxSlots);
          srcOldestSlot = (((srcLastSlot+srcMaxSlots) + 1) % srcMaxSlots);
          dstMaxSlots   = tmpNoDaySlots;
          DebugTf("Change DAY history from [%d] to [%d]\r\n", srcMaxSlots, dstMaxSlots);
          writeToSysLog("Change DAY history from [%d] to [%d] days", srcMaxSlots, dstMaxSlots);
          //-- save dstLastTime for later use!
          dstLastTime = buildTimeStruct(record, dstMaxSlots, dstMaxSlots, dstMaxSlots);
          dstLastSlot = dstLastTime.daySlot;
        }
        break;
    case RNG_MONTHS: {
          writeDataToRingFile(MONTHS_FILE, ringType, record, prevTlgrmTime);
          strlcpy(srcFileName, MONTHS_FILE, sizeof(srcFileName));
          snprintf(altFileName, sizeof(altFileName), "/RINGmonths_%d.csv", devSetting->NoMonthSlots);
          strlcpy(cType, "MONTHS", sizeof(cType));
          srcMaxSlots   = devSetting->NoMonthSlots;
          srcLastSlot   = (lastTlgrmTime.Months % srcMaxSlots);
          srcOldestSlot = (((srcLastSlot+srcMaxSlots) + 1) % srcMaxSlots);
          dstMaxSlots   = tmpNoMonthSlots;
          DebugTf("Change MONTH history from [%d] to [%d]\r\n", srcMaxSlots, dstMaxSlots);
          writeToSysLog("Change MONTH history from [%d] to [%d] months", srcMaxSlots, dstMaxSlots);
          //-- save dstLastTime for later use!
          dstLastTime = buildTimeStruct(record, dstMaxSlots, dstMaxSlots, dstMaxSlots);
          dstLastSlot = dstLastTime.monthSlot;
        }
        break;
  }
  //-- don't do this more than once!
  if (newVal == dstMaxSlots) return false;

  newVal = dstMaxSlots;

  //-- what do you want to do?
  if (srcMaxSlots > dstMaxSlots)
  {
    doExpand = false; //-- shrinking the history
    extraSlots = 0;
    lessSlots  = srcMaxSlots - dstMaxSlots;
  }
  else
  {
    doExpand = true;  //-- expanding the history
    extraSlots = dstMaxSlots - srcMaxSlots;
    lessSlots  = 0;
  }

  _FSYS.remove(dstFileName);

  //-- create a tmp RING file (make space on FileSystem)
  DebugTf("Create [%s] for [%d] [%s] from [%s]..\r\n", dstFileName, dstMaxSlots, cType, dstTime.Timestamp);
  writeToSysLog("Create [%s] for [%d] [%s]..", dstFileName, dstMaxSlots, cType);
  createRingFile(dstFileName, lastTlgrmTime, ringType, dstMaxSlots);

  if (Verbose2)
  {
    DebugTln("Just created file ==============================================");
    for(int s=0; s<dstMaxSlots; s++)
    {
      readOneSlot(record, dstFileName, s, dstMaxSlots);
    }
    Debugln("\r\n");
  }
  
  //------ ok, now the real altertering starts --------
  DebugTf("Read oldest slot[%02d]!\r\n", srcOldestSlot - lessSlots);
  readOneSlot(record, srcFileName, (srcOldestSlot - lessSlots), srcMaxSlots);
  Debugln();
  DebugTf("[%02d] [%-30.30s] is the (oldest) lowest key!\r\n", (srcOldestSlot - lessSlots)
                                                             ,  record);
  Debugln("\r\n");

  //-- now update with real dates
  File dstFile = _FSYS.open(dstFileName, "r+");  // read and write ..
  if (!dstFile)
  {
    DebugTf("Error opening [%s]\r\n", dstFileName);
    return false;
  }

  dstTime = buildTimeStruct(lastTlgrmTime.Timestamp, dstMaxSlots, dstMaxSlots, dstMaxSlots);
  DebugTf("dstTime.daysHist[%d]\r\n", dstTime.daysHist);
  dstTime = calculateTime(dstTime, ((dstMaxSlots-1)* -1), ringType);
  DebugTf("Start updating [%s]\r\n", dstTime.Timestamp);
  
  for(int s=0; s<dstMaxSlots; s++)
  {
    //-- create key
    for(int p=0; p<8; p++) { record[p] = dstTime.Timestamp[p]; }
    switch(ringType)
    {
      case RNG_HOURS:  dstActSlot = dstTime.hourSlot;  break;
      case RNG_DAYS:   dstActSlot = dstTime.daySlot;   break;
      case RNG_MONTHS: dstActSlot = dstTime.monthSlot; break;
    }
    dstTime.hourSlot  = dstActSlot;
    dstTime.daySlot   = dstActSlot;
    dstTime.monthSlot = dstActSlot;
    writeDataToRingFile(dstFileName, ringType, record, dstTime);
    dstTime = calculateTime(dstTime, 1, ringType);
  }
    
  if (Verbose2)
  {
    DebugTln("dstFileName ==============================================");
    for(int s=0; s<dstMaxSlots; s++)
    {
      readOneSlot(record, dstFileName, s, dstMaxSlots);
    }
    Debugln("\r\n");
  }
  
  if (!_FSYS.exists(srcFileName))
  {
    DebugTf("File [%s] does not excist!\r\n", srcFileName);
    return false;
  }

  File srcFile = _FSYS.open(srcFileName, "r+");  // read and write ..
  if (!srcFile)
  {
    DebugTf("Error opening [%s]\r\n", srcFileName);
    return false;
  }

  //-- now, copy all slots of srcFile to dstFileName
  for (uint8_t s = 0; (s < srcMaxSlots) && (s < dstMaxSlots); s++)
  {
    //-- from newest (last) down to oldest --
    srcActSlot = ((srcMaxSlots -s) + srcLastSlot) % srcMaxSlots;
    offset  = ((srcActSlot +1) * (DATA_RECLEN +1));
    if (Verbose2) 
        DebugTf("s[%d][%s] -> offset[%d]\r\n", srcActSlot, cType, offset); 
    srcFile.seek(offset, SeekSet);
    int bytesRead = srcFile.readBytesUntil('\n', record, DATA_RECLEN);
 
    if (Verbose1) DebugTf("[%2d] %s (%d bytes)\r\n", srcActSlot, record, strlen(record));
    if (bytesRead != DATA_RECLEN)   // '\n' is skipped by readBytesUntil()
    {
        DebugTf("bytesRead[%d] != DATA_RECLEN[%d]\r\n", bytesRead, DATA_RECLEN);
        writeToSysLog("bytesRead[%d] != DATA_RECLEN[%d]", bytesRead, DATA_RECLEN);
        if (!isValidTimestamp(record, 8))   // first 8 bytes is YYMMDDHH
        {
          DebugTf("srcActSlot[%02d]==>Key[%-8.8s] not valid!!\r\n", srcActSlot, record);
          writeToSysLog("srcActSlot[%02d]==>Key[%-8.8s] not valid!!", srcActSlot, record);
        }
        return false;
    } //-- record length error
  
    //-- now: do the actual copying ----
    dstTime = buildTimeStruct(record, dstMaxSlots, dstMaxSlots, dstMaxSlots);
    switch(ringType)
    {
      case RNG_HOURS:   dstActSlot = dstTime.hourSlot;  break;
      case RNG_DAYS:    dstActSlot = dstTime.daySlot;   break;
      case RNG_MONTHS:  dstActSlot = dstTime.monthSlot; break;
    }
    dstOldestSlot = dstActSlot;
    if (Verbose1) DebugTf("[%2d]: srcSlt[%02d], dstSlt[%02d]->[%-30.30s]\r\n", s, srcActSlot, dstActSlot, record);
    writeDataToRingFile(dstFileName, ringType, record, dstTime);
  
  } //  for al slots ..

  //-- done with the srcFile!
  srcFile.close();
  
  if (Verbose2)
  {
    DebugTln("dstFileName ==============================================");
    for(int s=0; s<dstMaxSlots; s++)
    {
      readOneSlot(record, dstFileName, s, dstMaxSlots);
    }
    Debugln("\r\n");
  }
  
  //-- do the house keeping ---
  _FSYS.remove(altFileName);
  if (_FSYS.rename(srcFileName, altFileName))
  {
    DebugTf("rename( %s, %s )\r\n", srcFileName, altFileName);
    writeToSysLog("[%s] saved as [%s]", srcFileName, altFileName);
  }
  else
  {
    DebugTf("ERROR renaming [%s] to [%s]\r\n", srcFileName, altFileName);
    writeToSysLog("ERROR renaming [%s] to [%s]", srcFileName, altFileName);
  }
  if (_FSYS.rename(dstFileName, srcFileName))
  {
    DebugTf("rename( %s, %s )\r\n", dstFileName, srcFileName);
    writeToSysLog("[%s] saved as [%s]", dstFileName, srcFileName);
  }
  else
  {
    DebugTf("ERROR renaming [%s] to [%s]\r\n", dstFileName, srcFileName);
    writeToSysLog("ERROR renaming [%s] to [%s]", dstFileName, srcFileName);
  }
  switch(ringType)
  {
    case RNG_HOURS:   
            devSetting->NoHourSlots  = tmpNoHourSlots;  
            break;
    case RNG_DAYS:    
            snprintf(altFileName, sizeof(altFileName), "RINGdays_%d.csv", devSetting->NoDaySlots);
            devSetting->NoDaySlots   = tmpNoDaySlots;   
            break;
    case RNG_MONTHS:  
            snprintf(altFileName, sizeof(altFileName), "RINGmonths_%d.csv", devSetting->NoMonthSlots);
            devSetting->NoMonthSlots = tmpNoMonthSlots; 
            break;
  }

  writeDevSettings(true);

  DebugTln("Done!");
  
  DebugTf("[%s] altered! .. restart DSMR-logger\r\n\n   ", srcFileName);
  writeToSysLog("[%s] altered! .. restart DSMR-logger", srcFileName);
  delay(1000);
  ESP.restart();
  delay(2000);

  //-- code should never go here!
  tmpNoHourSlots    = devSetting->NoHourSlots;
  tmpNoDaySlots     = devSetting->NoDaySlots;
  tmpNoMonthSlots   = devSetting->NoMonthSlots;
  tmpAlterRingSlots = false;

  return true;
  
} //  alterRingFile()


//===========================================================================================
uint16_t readRingHistoryDepth(const char *fileName, int8_t ringType)
{
  char      header[DATA_RECLEN +2] = {0};
  char      skipper[DATA_RECLEN +2] = {0};
  uint16_t  maxSlots;
  int       histDepth = 0;
  bool      needConversion = false;
  
  if (!_FSYS.exists(fileName))
  {
    switch(ringType)
    {
      case RNG_HOURS:   return _NO_HOUR_SLOTS_;
      case RNG_DAYS:    return _NO_DAY_SLOTS_;
      case RNG_MONTHS:  return _NO_MONTH_SLOTS_;
    }
  }

  File dataFile = _FSYS.open(fileName, "r+");  // read and write ..
  if (!dataFile)
  {
    DebugTf("Error opening [%s]\r\n", fileName);
    writeToSysLog("Error opening [%s]", fileName);
    switch(ringType)
    {
      case RNG_HOURS:   return _NO_HOUR_SLOTS_;
      case RNG_DAYS:    return _NO_DAY_SLOTS_;
      case RNG_MONTHS:  return _NO_MONTH_SLOTS_;
    }
  }
  dataFile.seek(0, SeekSet);
  bytesRead = dataFile.readBytesUntil('\n', header, DATA_RECLEN);
  if (Verbose2) DebugTf("header[%s] (%d bytes)\r\n", header, strlen(header));
  if (strlen(header) == 74) //-- this is an old API file
  {
    needConversion = true;
  }
  else
  {
    sscanf(header, "%[^#]#%d", skipper, &histDepth);
  }
  if (histDepth < 10)
  {
    switch(ringType)
    {
      case RNG_HOURS:   histDepth = _NO_HOUR_SLOTS_;  break;
      case RNG_DAYS:    histDepth = _NO_DAY_SLOTS_;   break;
      case RNG_MONTHS:  histDepth = _NO_MONTH_SLOTS_; break;
    }
  }

  if (!needConversion)
  {
    switch(ringType)
    {
      case RNG_HOURS: 
            { if (devSetting->NoHourSlots != histDepth)
              {
                devSetting->NoHourSlots = histDepth;
                DebugTf("[HOURS]  History is [%d] hours\r\n", histDepth);
                writeToSysLog("[HOURS]  History is [%d] hours", histDepth);
              }
            }
            break;
      case RNG_DAYS: 
            { if (devSetting->NoDaySlots != histDepth)
              {
                devSetting->NoDaySlots = histDepth;
                DebugTf("[DAYS]   History is [%d] days\r\n", histDepth);
                writeToSysLog("[DAYS]   History is [%d] days", histDepth);
              }
            }
            break;
      case RNG_MONTHS: 
            { if (devSetting->NoMonthSlots != histDepth)
              {
                devSetting->NoMonthSlots = histDepth;
                DebugTf("[MONTHS] History is [%d] months\r\n", histDepth);
                writeToSysLog("[MONTHS] History is [%d] months", histDepth);
              }
            }
            break;
    }
  }
  
  if (needConversion)
  {
    DebugTf("file [%s] needs conversion ...", fileName);
    writeToSysLog("file [%s] needs conversion ...", fileName);
    switch(ringType)
    {
      case RNG_HOURS:   maxSlots = _NO_HOUR_SLOTS_;  break;
      case RNG_DAYS:    maxSlots = _NO_DAY_SLOTS_;   break;
      case RNG_MONTHS:  maxSlots = _NO_MONTH_SLOTS_; break;
    }
    File tmpFile = _FSYS.open("/tmpFile", "w");
    Debug(" -> ");
    for(int r=0; r<=maxSlots; r++)
    {
      if ((r%10)==0)  Debugf("%d ", r);
      else            Debug('.');
      dataFile.seek((r*(74+1)), SeekSet);
      int bytesRead = dataFile.readBytesUntil('\n', skipper, DATA_RECLEN);
      //DebugTf("[%02d]< [%s][%d bytes]\r\n", r, skipper, strlen(skipper)); 
      fillRecord(skipper, DATA_RECLEN);
      //DebugTf("[%02d]> [%s][%d bytes]\r\n", r, skipper, strlen(skipper)); 
      tmpFile.seek((r*(DATA_RECLEN+1)), SeekSet);
      tmpFile.println(skipper);
    }
    Debug(maxSlots);
    DebugTf("Converted [%d] records!\r\n", maxSlots);
    writeToSysLog("Converted [%d] records!", maxSlots);
    //-- now update  fileHeader ----------------------------------------
    //-- be aware: you should not modify *fileHeader!!!
    snprintf(skipper, DATA_RECLEN, (char*)DATA_CSV_HEADER, maxSlots);
    fillRecord(skipper, DATA_RECLEN);
    DebugTf("Added history depth [%d] to header\r\n", maxSlots);
    writeToSysLog("Added history depth [%d] to header", maxSlots);
    dataFile.seek(0, SeekSet);
    dataFile.println(skipper);

    tmpFile.close();
    dataFile.close();
    _FSYS.remove(fileName);
    _FSYS.rename("/tmpFile", fileName);
    histDepth = maxSlots;
  }
  
  return histDepth;
  
} //  readRingHistoryDepth()


//===========================================================================================
void fillRecord(char *record, int8_t maxLen)
{
  int16_t recLen = strlen(record);

  //DebugTf("record[%-12.12s], recLen[%d], maxLen[%d]\r\n", record, recLen, maxLen);
      
  if (Verbose1) 
    DebugTf("Length of record IN is [%d] bytes\r\n", recLen);

  //-- first: remove control chars
  for(int i=0; i<strlen(record); i++)
  {
    if ((record[i] < ' ') || (record[i] > '~')) record[i] = ' ';
  }
  //-- add spaces at the end
  do {
      strlcat(record, "     ", maxLen);
      recLen = strlen(record);
  } while(recLen < (maxLen-1));

  //-- maxLen '20' => '0'....'19''\0'!
  //-- '19' should be '\0' but is ommitted due to println()!
  record[maxLen -3]  = ';';
  record[maxLen -2]  = ';';
  record[maxLen -1]  = '\0';
  //----[maxLen -1]  = '\n'; // '19' added by println()

  if (Verbose2) 
    DebugTf("Length of record OUT is [%d] bytes\r\n", strlen(record));

} // fillRecord()


//===========================================================================================
int32_t freeSpace()
{
  int32_t space;

  //_FSYS.info(SPIFFSinfo);

  space = (int32_t)(_FSYS.totalBytes() - _FSYS.usedBytes());

  return space;

} // freeSpace()


//===========================================================================================
void listFilesystem()
{
  listFileStruct dirMap[30];
  int fileNr = 0;

  memset(dirMap, 0, sizeof(dirMap));

  File root = _FSYS.open("/");  

  File dir  = root.openNextFile();
  while (dir)
  {
    strlcpy(dirMap[fileNr].Name, dir.name(), 31);
    dirMap[fileNr].Size = dir.size();
    dir = root.openNextFile();  
    if (dir) fileNr++;
  }
  
  qsort(dirMap, fileNr, sizeof(dirMap[0]), sortListFiles);

  DebugTln("\r\n");
  for(int f=0; f<fileNr; f++)
  {
    Debugf(" %-25s %6d bytes \r\n", dirMap[f].Name, dirMap[f].Size);
    yield();
  }

  Debugln("\r\n");
  Debugf("           FSYS Size [%6d]kB\r\n", (_FSYS.totalBytes() / 1024));
  Debugf("           FSYS Used [%6d]kB\r\n", (_FSYS.totalBytes() - freeSpace())/1024);
  if (freeSpace() < 100000)
    Debugf("Available FSYS space [%6d]kB (LOW ON SPACE!!!)\r\n", (freeSpace() / 1024));
  else  
    Debugf("Available FSYS space [%6d]kB\r\n", (freeSpace() / 1024));
  Debugln("\r\n");
  
} // listFilesystem()


//===========================================================================================
void eraseFile()
{
  char eName[30] = {0};

  //--- erase buffer's
  while (TelnetStream.available() > 0)
  {
    yield();
    (char)TelnetStream.read();
  }
  while (Serial.available() > 0)
  {
    yield();
    (char)Serial.read();
  }

  Debug("Enter filename to erase: ");
  uint32_t waitTimer = millis();
  while((millis() - waitTimer) < 9000)
  {
    uint32_t timePassed = (millis() - waitTimer);
    if (TelnetStream.available())
    {  
      TelnetStream.setTimeout(10000-timePassed);
      TelnetStream.readBytesUntil('\n', eName, sizeof(eName));
      TelnetStream.setTimeout(1000);
    }
    else if (Serial.available())
    {  
      Serial.setTimeout(10000-timePassed);
      Serial.readBytesUntil('\n', eName, sizeof(eName));
      Serial.setTimeout(1000);
    }
  }
  Debugln('\n');
  if (strlen(eName) == 0) 
  {
    DebugTln("Waiting for input Timed Out!\r\n");
    return;
  }
  
  //--- remove control chars like \r and \n ----
  //--- and shift all char's one to the right --
  for(int i=strlen(eName); i>0; i--)
  {
    eName[i] = eName[i-1];
    if (eName[i] < ' ') eName[i] = '\0';
  }
  //--- add leading slash on position 0
  eName[0] = '/';

  if (_FSYS.exists(eName))
  {
    Debugf("\r\nErasing [%s] from FileSystem\r\n\n", eName);
    _FSYS.remove(eName);
    writeToSysLog("Erased [%s] from by user!", eName);
  }
  else
  {
    Debugf("\r\nfile [%s] not found..\r\n\n", eName);
  }
  //--- empty buffer ---
  while (TelnetStream.available() > 0)
  {
    yield();
    (char)TelnetStream.read();
  }
  while (Serial.available() > 0)
  {
    yield();
    (char)Serial.read();
  }

} // eraseFile()


//===========================================================================================
bool DSMRfileExist(const char *fileName, const char* funcName, bool doDisplay)
{
  char fName[30] = {0};

  if (fileName[0] != '/')
  {
    strlcat(fName, "/", 5);
  }
  strlcat(fName, fileName, sizeof(fName));
  DebugTf("[%-12.12s][%s] %s .. \r\n", funcName, fName
                       , _FSYS.exists(fName) ? "Exists":"Does NOT exist");

  if (devSetting->OledType > 0)
  {
    neoPixOff(1);
    oled_Print_Msg(1, "Bestaat:", 10);
    oled_Print_Msg(2, fName, 10);
    oled_Print_Msg(3, "op FileSystem?", 250);
  }

  if (!_FSYS.exists(fName) )
  {
    pulseHeart(true);
    neoPixOn(1, neoPixRed);
    glowTimer1 = millis() + 1000;
    if (doDisplay)
    {
      //Debugln(F("NO! Error!!"));
      if (devSetting->OledType > 0)
      {
        oled_Print_Msg(3, "Nee! FOUT!", 2000);
      }
      writeToSysLog("Error! File [%s] not found!", fName);
      return false;
    }
    else
    {
      //Debugln(F("NO! "));
      if (devSetting->OledType > 0)
      {
        oled_Print_Msg(3, "Nee! ", 2000);
      }
      writeToSysLog("File [%s] not found! in [%-15.15s]", fName, funcName);
      return false;
    }
  }
  else
  {
    //Debugln("Yes! OK!");
    if (devSetting->OledType > 0)
    {
      oled_Print_Msg(3, "JA! (OK!)", 250);
    }
  }
  return true;

} //  DSMRfileExist()


//---------------------------------------------------------
// qsort requires you to create a sort function
int sortListFiles(const void *cmp1, const void *cmp2)
{
  struct listFileStruct *ia = (struct listFileStruct *)cmp1;
  struct listFileStruct *ib = (struct listFileStruct *)cmp2;

  return strcmp(ia->Name, ib->Name);

} //  sortFunction()


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
