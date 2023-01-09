/*
***************************************************************************
**  Program  : helperStuff, part of DSMRlogger32
**  Version  : v5.n
**
**  Copyright (c) 2020 .. 2023 Willem Aandewiel
**
**  TERMS OF USE: MIT License. See bottom of file.
***************************************************************************
*/

//===========================================================================================
void pulseHeart(bool force)
{
  static uint32_t pulseTimer;
  
  if (skipHeartbeats || lostWiFiConnection) return;
  
  if (force || ((millis()-pulseTimer) > _PULSE_TIME))
  {
    pulseTimer = millis();
    digitalWrite(_PIN_HEARTBEAT, !digitalRead(_PIN_HEARTBEAT));
    //-- neo 0 White
    neoPixOn(0, neoPixWhiteLow);
    glowTimer0 = millis();
  }
  if ((millis() - glowTimer0) > _GLOW_TIME)
  {
    neoPixOn(0, neoPixGreenLow);
  }
  
} //  pulseHeart()

//-------------------------------------------------------------------
void pulseHeart()
{
  pulseHeart(false);
  
} //  pulseHeart()


//===========================================================================================
void resetWatchdog()
{
  for(int i=0; i<3; i++)
  {
    digitalWrite(_PIN_WD_RESET, LOW);
    delay(100);
    digitalWrite(_PIN_WD_RESET, HIGH);
    delay(100);
  }
  if (filesysMounted)  writeToSysLog("Watchdog reset!");
    
} //  resetWatchdog()


//===========================================================================================
boolean isValidIP(IPAddress ip)
{
  /* Works as follows:
    *  example:
    *  127.0.0.1
    *   1 => 127||0||0||1 = 128>0 = true
    *   2 => !(false || false) = true
    *   3 => !(false || false || false || false ) = true
    *   4 => !(true && true && true && true) = false
    *   5 => !(false) = true
    *   true && true & true && false && true = false ==> correct, this is an invalid addres
    *
    *   0.0.0.0
    *   1 => 0||0||0||0 = 0>0 = false
    *   2 => !(true || true) = false
    *   3 => !(false || false || false || false) = true
    *   4 => !(true && true && true && tfalse) = true
    *   5 => !(false) = true
    *   false && false && true && true && true = false ==> correct, this is an invalid addres
    *
    *   192.168.0.1
    *   1 => 192||168||0||1 =233>0 = true
    *   2 => !(false || false) = true
    *   3 +> !(false || false || false || false) = true
    *   4 => !(false && false && true && true) = true
    *   5 => !(false) = true
    *   true & true & true && true && true = true ==> correct, this is a valid address
    *
    *   255.255.255.255
    *   1 => 255||255||255||255 =255>0 = true
    *   2 => !(false || false ) = true
    *   3 +> !(true || true || true || true) = false
    *   4 => !(false && false && false && false) = true
    *   5 => !(true) = false
    *   true && true && false && true && false = false ==> correct, this is an invalid address
    *
    *   0.123.12.1       => true && false && true && true && true = false  ==> correct, this is an invalid address
    *   10.0.0.0         => true && false && true && true && true = false  ==> correct, this is an invalid address
    *   10.255.0.1       => true && true && false && true && true = false  ==> correct, this is an invalid address
    *   150.150.255.150  => true && true && false && true && true = false  ==> correct, this is an invalid address
    *
    *   123.21.1.99      => true && true && true && true && true = true    ==> correct, this is annvalid address
    *   1.1.1.1          => true && true && true && true && true = true    ==> correct, this is annvalid address
    *
    *   Some references on valid ip addresses:
    *   - https://www.quora.com/How-do-you-identify-an-invalid-IP-address
    *
    */
  boolean _isValidIP = false;
  _isValidIP = ((ip[0] || ip[1] || ip[2] || ip[3])>0);                             // if any bits are set, then it is not 0.0.0.0
  _isValidIP &= !((ip[0]==0) || (ip[3]==0));                                       // if either the first or last is a 0, then it is invalid
  _isValidIP &= !((ip[0]==255) || (ip[1]==255) || (ip[2]==255) || (ip[3]==255)) ;  // if any of the octets is 255, then it is invalid
  _isValidIP &= !(ip[0]==127 && ip[1]==0 && ip[2]==0 && ip[3]==1);                 // if not 127.0.0.0 then it might be valid
  _isValidIP &= !(ip[0]>=224);                                                     // if ip[0] >=224 then reserved space

  DebugTf( "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
  if (_isValidIP)
    Debugln(F(" = Valid IP"));
  else
    Debugln(F(" = Invalid IP!"));

  return _isValidIP;

} //  isValidIP()


//===========================================================================================
bool isValidTimestamp(const char *timeStamp, int8_t len)
{
  for (int i=0; (i<len && i<12); i++)
  {
    if (timeStamp[i] < '0' || timeStamp[i] > '9')
    {
      return false;
    }
  }
  return true;

} // isValidTimestamp()


//===========================================================================================
int8_t splitString(String inStrng, char delimiter, String wOut[], uint8_t maxWords)
{
  int16_t inxS = 0, inxE = 0, wordCount = 0;

  inStrng.trim();
  while(inxE < inStrng.length() && wordCount < maxWords)
  {
    inxE  = inStrng.indexOf(delimiter, inxS);         //finds location of first ,
    wOut[wordCount] = inStrng.substring(inxS, inxE);  //captures first data String
    wOut[wordCount].trim();
    //DebugTf("[%d] => [%c] @[%d] found[%s]\r\n", wordCount, delimiter, inxE, wOut[wordCount].c_str());
    inxS = inxE;
    inxS++;
    wordCount++;
  }
  // zero rest of the words
  for(int i=wordCount; i< maxWords; i++)
  {
    wOut[wordCount][0] = 0;
  }
  // if not whole string processed place rest in last word
  if (inxS < inStrng.length())
  {
    wOut[maxWords-1] = inStrng.substring(inxS, inStrng.length());  // store rest of String
  }

  return wordCount;

} // splitString()


//===========================================================================================
String upTime()
{
  char    calcUptime[20];
  upTimeSeconds = (millis() / 1000) - upTimeStart;
  
  snprintf(calcUptime, sizeof(calcUptime), "%d(d)-%02d:%02d(H:m)"
           , int((upTimeSeconds / (60 * 60 * 24)) % 365)
           , int((upTimeSeconds / (60 * 60)) % 24)
           , int((upTimeSeconds / (60)) % 60));

  return calcUptime;

} // upTime()


//===========================================================================================
// a 'save' string copy
void strCpyFrm(char *dest, int maxLen, const char *src, uint8_t frm, uint8_t to)
{
  int d=0;
  //DebugTf("dest[%s], src[%s] max[%d], frm[%d], to[%d] =>\r\n", dest, src, maxLen, frm, to);
  dest[0] = '\0';
  for (int i=0; i<=frm; i++)
  {
    if (src[i] == 0) return;
  }
  for (int i=frm; (src[i] != 0  && i<=to && d<maxLen); i++)
  {
    dest[d++] = src[i];
  }
  dest[d] = '\0';

} // strCpyFrm()


//===========================================================================================
float strToFloat(const char *s, int dec)
{
  float r =  0.0;
  int   p =  0;
  int   d = -1;

  r = strtof(s, NULL);
  p = (int)(r*pow(10, dec));
  r = p / pow(10, dec);
  //DebugTf("[%s][%d] => p[%d] -> r[%f]\r\n", s, dec, p, r);
  return r;

} //  strToFloat()


//===========================================================================================
double round1(double value) {
  return (int)(value * 10 + 0.5) / 10.0;
}
//===========================================================================================
double round2(double value) {
  return (int)(value * 100 + 0.5) / 100.0;
}
//===========================================================================================
double round3(double value) {
  return (int)(value * 1000 + 0.5) / 1000.0;
}


//===========================================================================================
void getLastResetReason(RESET_REASON reason, char *txtReason, int txtReasonLen)
{
  switch (reason)
  {
    case 1  : snprintf(txtReason, txtReasonLen, "[%02d] Vbat power on reset", reason); break;
    case 3  : snprintf(txtReason, txtReasonLen, "[%02d] Software reset digital core (ESP.restart())", reason); break;
    case 4  : snprintf(txtReason, txtReasonLen, "[%02d] Legacy watch dog reset digital core", reason); break;
    case 5  : snprintf(txtReason, txtReasonLen, "[%02d] Deep Sleep reset digital core", reason); break;
    case 6  : snprintf(txtReason, txtReasonLen, "[%02d] Reset by SLC module, reset digital core", reason); break;
    case 7  : snprintf(txtReason, txtReasonLen, "[%02d] Timer Group0 Watch dog reset digital core", reason); break;
    case 8  : snprintf(txtReason, txtReasonLen, "[%02d] Timer Group1 Watch dog reset digital core", reason); break;
    case 9  : snprintf(txtReason, txtReasonLen, "[%02d] RTC Watch dog Reset digital core", reason); break;
    case 10 : snprintf(txtReason, txtReasonLen, "[%02d] Instrusion tested to reset CPU", reason); break;
    case 11 : snprintf(txtReason, txtReasonLen, "[%02d] Time Group reset CPU", reason); break;
    case 12 : snprintf(txtReason, txtReasonLen, "[%02d] Software reset CPU", reason); break;
    case 13 : snprintf(txtReason, txtReasonLen, "[%02d] RTC Watch dog Reset CPU", reason); break;
    case 14 : snprintf(txtReason, txtReasonLen, "[%02d] for APP CPU, reseted by PRO CPU", reason); break;
    case 15 : snprintf(txtReason, txtReasonLen, "[%02d] Reset when the vdd voltage is not stable", reason); break;
    case 16 : snprintf(txtReason, txtReasonLen, "[%02d] RTC Watch dog reset digital core and rtc module", reason); break;
    default : snprintf(txtReason, txtReasonLen, "[%03d] NO_MEAN", reason);
  }
} // getLastResetReason()


//=======================================================================
void addToTable(const char *cName, const char *cValue)
{
  strlcpy(fieldTable[fieldTableCount].cName,  cName, _FIELDTABLE_CNAME_LEN);
  strlcpy(fieldTable[fieldTableCount].type.cValue, cValue, _FIELDTABLE_CVALUE_LEN);
  fieldTable[fieldTableCount].cType = 'c';
  fieldTableCount++;
  
} //  addToTable(char, char)

//=======================================================================
void addToTable(const char *cName,  String sValue)
{
  strlcpy(fieldTable[fieldTableCount].cName,  cName, _FIELDTABLE_CNAME_LEN);
  strlcpy(fieldTable[fieldTableCount].type.cValue, sValue.c_str(), _FIELDTABLE_CVALUE_LEN);
  fieldTable[fieldTableCount].cType = 'c';
  fieldTableCount++;
  
} //  addToTable(char, String)

//=======================================================================
void addToTable(const char *cName, uint32_t uValue)
{
  strlcpy(fieldTable[fieldTableCount].cName, cName, _FIELDTABLE_CNAME_LEN);
  fieldTable[fieldTableCount].type.uValue = uValue;
  fieldTable[fieldTableCount].cType = 'u';
  fieldTableCount++;
  
} //  addToTable(char, uint)

//=======================================================================
void addToTable(const char *cName, int32_t iValue)
{
  strlcpy(fieldTable[fieldTableCount].cName, cName, _FIELDTABLE_CNAME_LEN);
  fieldTable[fieldTableCount].type.iValue = iValue;
  fieldTable[fieldTableCount].cType = 'i';
  fieldTableCount++;
  
} //  addToTable(char, int)

//=======================================================================
void addToTable(const char *cName, float fValue)
{
  strlcpy(fieldTable[fieldTableCount].cName, cName, _FIELDTABLE_CNAME_LEN);
  fieldTable[fieldTableCount].type.fValue = fValue;
  fieldTable[fieldTableCount].cType = 'f';
  fieldTableCount++;
  
} //  addToTable(char, float)


//=======================================================================
//======= actualStore functions =========================================
//=======================================================================

//=======================================================================
//=======================================================================
void pushToActualStore(const char *cName,  String sValue)
{
  if (strcmp(cName, "timestamp") == 0)
        strlcpy(actualStore[actualStoreSlot].timestamp, sValue.c_str(), _TIMESTAMP_LEN);
  
} //  addToTable(char, String)

//=======================================================================
void pushToActualStore(const char *cName, float fValue)
{
  if (strcmp(cName, "power_delivered_l1") == 0)
        actualStore[actualStoreSlot].power_delivered_l1 = round3(fValue);
  else if (strcmp(cName, "power_delivered_l2") == 0)
        actualStore[actualStoreSlot].power_delivered_l2 = round3(fValue);
  else if (strcmp(cName, "power_delivered_l3") == 0)
        actualStore[actualStoreSlot].power_delivered_l3 = round3(fValue);
  else if (strcmp(cName, "power_returned_l1") == 0)
        actualStore[actualStoreSlot].power_returned_l1  = round3(fValue);
  else if (strcmp(cName, "power_returned_l2") == 0)
        actualStore[actualStoreSlot].power_returned_l2  = round3(fValue);
  else if (strcmp(cName, "power_returned_l3") == 0)
        actualStore[actualStoreSlot].power_returned_l3  = round3(fValue);
  else if (strcmp(cName, "gas_delivered") == 0)
        actualStore[actualStoreSlot].gas_delivered      = round3(fValue);
  
} //  pushToActualStore(char, float)

//=======================================================================
void pushTlgrmToActualStore()
{
  memset(fieldTable, 0, (sizeof(fieldTableStruct) *100));
  fieldTableCount = 0;
  onlyIfPresent = true;
  copyToFieldsArray(actualTableArray, actualElements);

  actualStoreSlot = actualStoreCount % _MAX_ACTUAL_STORE;
  actualStore[actualStoreSlot].count = actualStoreCount;
  
  tlgrmData.applyEach(addSmToActualStore());
  pushToActualStore("gas_delivered", gasDelivered);

  actualStoreCount++;

  if ( (actualStoreCount % 1000) == 0 )
  {
    for (int i=0; i< _MAX_ACTUAL_STORE; i++)
    {
      int s = (i+actualStoreSlot+1) % _MAX_ACTUAL_STORE;
      Debugf("[%3d][%5d][%-12.12s] PwrDel[%10.3f] PwrRet[%10.3f] GasDel[%10.3f]\r\n"
              , i, actualStore[s].count, actualStore[s].timestamp
                                       , (actualStore[s].power_delivered_l1
                                          + actualStore[s].power_delivered_l2
                                          + actualStore[s].power_delivered_l3)
                                       , (actualStore[s].power_returned_l1
                                          + actualStore[s].power_returned_l2
                                          + actualStore[s].power_returned_l3)
                                       , actualStore[s].gas_delivered);
    }
  }
 
} //  pushTlgrmToActualStore()


//=======================================================================
unsigned int CRC16(unsigned int crc, unsigned char *buf, int len)
{
  for (int pos = 0; pos < len; pos++)
  {
    crc ^= (unsigned int)buf[pos];    // XOR byte into least sig. byte of crc

    for (int i = 8; i != 0; i--)      // Loop over each bit
    {
      if ((crc & 0x0001) != 0)        // If the LSB is set
      {
        crc >>= 1;                    // Shift right and XOR 0xA001
        crc ^= 0xA001;
      }
      else                            // Else LSB is not set
        crc >>= 1;                    // Just shift right
    }
  }

  return crc;
  
} //  CRC16()


//====================================================================
//-- for char *array use this syntax:
//-- char *<SpiRamAllocator> buff(500);
//====================================================================
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

using SpiRamJsonDocument = BasicJsonDocument<SpiRamAllocator>;


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
