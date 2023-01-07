/*
***************************************************************************
**  Program  : setupStuff, part of DSMRlogger32
**  Version  : v5.n
**
**  Copyright (c) 2020 .. 2023 Willem Aandewiel
**
**  TERMS OF USE: MIT License. See bottom of file.
***************************************************************************
*/



//===========================================================================================
void setupFileSystem()
{
#ifdef _SPIFFS
  DebugTln("USING [SPIFFS] as Filesystem!");
  //DebugT("FORMAT SPIFFS ...... ");
  //if (_FSYS.format())
  //      DebugTln("SPIFFS formatted!");
  //else  DebugTln("Error formatting SPIFFS!");
#else
  DebugTln("USING [LittleFS] as Filesystem!");
#endif
  if (_FSYS.begin())
  {
    #ifdef _SPIFFS
        DebugTln("SPIFFS Mount succesfull!");
    #else
        DebugTln("LittleFS Mount succesfull!");
    #endif
    File nF = _FSYS.open("/!doNotFormat", "w");
    nF.close();
    filesysMounted = true;
  }
  else
  {
    //-- Serious problem with the Filesystem
    #ifdef _SPIFFS
      DebugTln("SPIFFS Mount failed!");
    #else
      DebugTln("LittleFS Mount failed!");
    #endif
    filesysMounted = false;
  }
} //  setupFileSystem()


//===========================================================================================
void setupSysLogger()
{
  DebugTln("initiate SysLogger..");
  
  //sysLog.setDebugLvl(4);
  //sysLog.setOutput(&Serial, 115200);

  if (!sysLog.begin(_SYSLOG_LINES, _SYSLOG_LINE_LEN)) 
  {
    DebugTln("sysLog.begin() error!");
    delay(1000);
  }
  writeToSysLog("    ");                         
  writeToSysLog("-- new start ------------------------------------");                         
  writeToSysLog("**** Booting....[%s]", String(_FW_VERSION).c_str());
  
} //  setupSysLogger()


//===========================================================================================
void setupPsram()
{
  DebugTln("initiate Psram ..");
  psramInit();
  
  int32_t PsramStart = ESP.getFreePsram();
  int32_t PsramEnd;

  DebugTf("Total Psram [%d bytes], Used [%d bytes], Psram Free [%d bytes]\r\n"
                       , ESP.getPsramSize()
                                        , (ESP.getPsramSize() - ESP.getFreePsram())
                                                               , ESP.getFreePsram());
  
  tlgrmTmpData = (char *) ps_malloc(_TLGRM_LEN);
  PsramEnd     = ESP.getFreePsram();
  //DebugTf("Claim [%d]bytes for tlgrmTmpData\r\n", _TLGRM_LEN);
  memset(tlgrmTmpData, 0, _TLGRM_LEN);
  //DebugTf("Used [%d]bytes, Psram Free [%d]bytes [after]\r\n", (PsramStart - PsramEnd), ESP.getFreePsram() );
  PsramStart   = PsramEnd;

  tlgrmRaw    = (char *) ps_malloc(_TLGRM_LEN);
  PsramEnd    = ESP.getFreePsram();
  //DebugTf("Claim [%d]bytes for tlgrmRaw\r\n", _TLGRM_LEN);
  memset(tlgrmRaw, 0, _TLGRM_LEN);
  //DebugTf("Used [%d]bytes, Psram Free [%d]bytes [after]\r\n", (PsramStart - PsramEnd), ESP.getFreePsram() );
  PsramStart  = PsramEnd;

  gMsg        = (char *) ps_malloc(_GMSG_LEN);
  PsramEnd    = ESP.getFreePsram();
  //DebugTf("Claim [%d]bytes for gMsg\r\n", _GMSG_LEN);
  memset(gMsg, 0, _GMSG_LEN);
  //DebugTf("Used [%d]bytes, Psram Free [%d]bytes [after]\r\n", (PsramStart - PsramEnd), ESP.getFreePsram() );
  PsramStart  = PsramEnd;

  jsonBuff    = (char *) ps_malloc(_JSONBUFF_LEN);
  PsramEnd    = ESP.getFreePsram();
  //DebugTf("Claim [%d]bytes for jsonBuff\r\n", _JSONBUFF_LEN);
  memset(jsonBuff, 0, _JSONBUFF_LEN);
  //DebugTf("Used [%d]bytes, Psram Free [%d]bytes [after]\r\n", (PsramStart - PsramEnd), ESP.getFreePsram() );
  PsramStart  = PsramEnd;

  fChar       = (char *) ps_malloc(_FCHAR_LEN);
  PsramEnd    = ESP.getFreePsram();
  //DebugTf("Claim [%d]bytes for fChar\r\n", _FCHAR_LEN);
  memset(fChar, 0, _FCHAR_LEN);
  //DebugTf("Used [%d]bytes, Psram Free [%d]bytes [after]\r\n", (PsramStart - PsramEnd), ESP.getFreePsram() );
  PsramStart  = PsramEnd;

  smSetting     = (settingSmStruct *) ps_malloc( sizeof(settingSmStruct) );
  PsramEnd    = ESP.getFreePsram();
  //DebugTf("Claim [%d]bytes for smSetting's\r\n", sizeof(settingSmStruct) );
  memset(smSetting, 0, sizeof(settingSmStruct) );
  //DebugTf("Used [%d]bytes, Psram Free [%d]bytes [after]\r\n", (PsramStart - PsramEnd), ESP.getFreePsram() );
  PsramStart  = PsramEnd;

  devSetting  = (settingDevStruct *) ps_malloc( sizeof(settingDevStruct) );
  PsramEnd    = ESP.getFreePsram();
  //DebugTf("Claim [%d]bytes for devSetting\r\n", sizeof(settingDevStruct) );
  memset(devSetting, 0, sizeof(settingDevStruct) );
  //DebugTf("Used [%d]bytes, Psram Free [%d]bytes [after]\r\n", (PsramStart - PsramEnd), ESP.getFreePsram() );
  PsramStart  = PsramEnd;

  fieldTable  = (fieldTableStruct *) ps_malloc( sizeof(fieldTableStruct) * 100 );
  PsramEnd    = ESP.getFreePsram();
  //DebugTf("Claim [%d]bytes for fieldTable\r\n", (sizeof(fieldTableStruct) *100) );
  memset(fieldTable, 0, (sizeof(fieldTableStruct) *100) );
  //DebugTf("Used [%d]bytes, Psram Free [%d]bytes [after]\r\n", (PsramStart - PsramEnd), ESP.getFreePsram() );
  PsramStart  = PsramEnd;

  actualStore = (actualDataStruct *) ps_malloc( sizeof(actualDataStruct) * _MAX_ACTUAL_STORE );
  PsramEnd    = ESP.getFreePsram();
  //DebugTf("Claim [%d]bytes for actualStore\r\n", (sizeof(actualDataStruct) * _MAX_ACTUAL_STORE) );
  memset(actualStore, 0, (sizeof(actualDataStruct) * _MAX_ACTUAL_STORE) );
  actualStoreCount = 0;
  //DebugTf("Used [%d]bytes, Psram Free [%d]bytes [after]\r\n", (PsramStart - PsramEnd), ESP.getFreePsram() );
  PsramStart  = PsramEnd;

  DebugTf("Total Psram [%d bytes], Used [%d bytes], Psram Free [%d bytes]\r\n"
                       , ESP.getPsramSize()
                                        , (ESP.getPsramSize() - ESP.getFreePsram())
                                                               , ESP.getFreePsram());

} //setupPsram()



//===========================================================================================
bool setupIsFsPopulated()
{
  bool tmpError = false;
  
  if (DSMRfileExist(devSetting->IndexPage, __FUNCTION__, false) )
  {
    if (strcmp(devSetting->IndexPage, "DSMRindex.html") != 0)
    {
      if (devSetting->IndexPage[0] != '/')
      {
        char tempPage[50] = "/";
#ifdef _LITTLEFS
        tempPage[0] = '/';
#endif
        strlcat(tempPage, devSetting->IndexPage, _INDEXPAGE_LEN);
        strlcpy(devSetting->IndexPage, tempPage, _INDEXPAGE_LEN);
      }
      hasAlternativeIndex        = true;
    }
    else  hasAlternativeIndex    = false;
  }
  if (!hasAlternativeIndex && !DSMRfileExist("DSMRindex.html", __FUNCTION__, false) )
  {
    tmpError = true;
  }
  if (!hasAlternativeIndex)    //--- there's no alternative index.html
  {
    DSMRfileExist("DSMRindex.js",    __FUNCTION__, false);
    DSMRfileExist("DSMRindex.css",   __FUNCTION__, false);
    DSMRfileExist("DSMRgraphics.js", __FUNCTION__, false);
  }
  if (!DSMRfileExist("FSmanager.html", __FUNCTION__, true))
  {
    tmpError = true;
  }
  if (!DSMRfileExist("FSmanager.css", __FUNCTION__, true))
  {
    tmpError = true;
  }

  return tmpError;
  
} //  setupIsFsPopulated()


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
