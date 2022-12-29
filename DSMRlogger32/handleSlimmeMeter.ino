/*
***************************************************************************
**  Program  : handleSlimmeMeter - part of DSMRlogger32
**  Version  : v5.n
**
**  Copyright (c) 2020 .. 2023 Willem Aandewiel
**
**  TERMS OF USE: MIT License. See bottom of file.
***************************************************************************
*/

#ifndef _HAS_NO_SLIMMEMETER
//==================================================================================
void handleSlimmemeter()
{
  //DebugTf("showRaw (%s)\r\n", showRaw ?"true":"false");
  if (showRaw)
  {
    //-- process telegrams in raw mode
    processSlimmemeterRaw();
  }
  else
  {
    processSlimmemeter();
  }

} // handleSlimmemeter()


//==================================================================================
void processSlimmemeterRaw()
{
  DebugTf("handleSlimmerMeter RawCount=[%4d]\r\n", showRawCount);
  showRawCount++;
  showRaw = (showRawCount <= 20);
  if (!showRaw)
  {
    showRawCount  = 0;
    return;
  }

  if (sysSetting->OledType > 0)
  {
    oled_Print_Msg(0, "<DSMRlogger32>", 0);
    oled_Print_Msg(1, "-------------------------", 0);
    oled_Print_Msg(2, "Raw Format", 0);
    snprintf(gMsg,  _GMSG_LEN, "Raw Count %4d", showRawCount);
    oled_Print_Msg(3, gMsg, 0);
  }

  slimmeMeter.enable(true);
  slimmeMeter.loop();

  if (slimmeMeter.available()) 
  {
    if (millis() > ( glowTimer1 + _GLOW_TIME) )
    {
      neoPixOn(1, neoPixGreen);
      glowTimer1 = millis();
    }
    tlgrmData = {};
    String tlgrmError;
    snprintf(tlgrmRaw, _TLGRM_LEN, slimmeMeter.raw().c_str());
    int16_t thisCRC=CRC16(0x0000, (unsigned char *) tlgrmRaw, strlen(tlgrmRaw));
    char crcChar[10] = {};
    snprintf(crcChar, sizeof(crcChar), "!%04x\r\n", (0xffff & thisCRC));

    if (slimmeMeter.parse(&tlgrmData, &tlgrmError)) 
    {
      Debug(tlgrmRaw);
      Debugln(crcChar);
      Debugln();
      Debugf("Telegram has %d chars\r\n\n", strlen(tlgrmRaw));
    }
  }

} // processSlimmemeterRaw()


//==================================================================================
void processSlimmemeter()
{
  slimmeMeter.loop();
  if (slimmeMeter.available())
  {
    DebugTf("telegramCount=[%d] telegramErrors=[%d]\r\n", telegramCount, telegramErrors);
    Debugln(F("\r\n[Time----][frHeap] Function------(line):\r"));
    //- Voorbeeld: [21:00:11][  9880] loop          ( 997): read telegram [28] => [140307210001S]
    telegramCount++;

    tlgrmData = {};
    String    tlgrmError;

    snprintf(tlgrmRaw, _TLGRM_LEN, slimmeMeter.raw().c_str());
    int telegramBytes = strlen(tlgrmRaw);

    if (slimmeMeter.parse(&tlgrmData, &tlgrmError)) 
    {
      //-- Parse succesful, print result
      if (telegramCount > (UINT32_MAX - 10))
      {
        writeToSysLog("Max. number of telegrams [%d] .. restart", telegramCount);

        delay(1000);
        ESP.restart();
        delay(1000);
      }
      if ( millis() > (glowTimer1 + _GLOW_TIME) )
      {
        neoPixOn(1, neoPixGreen);
        glowTimer1 = millis();
      }
      digitalWrite(LED_BUILTIN, LED_OFF);
      if (tlgrmData.identification_present)
      {
        //--- this is a hack! The identification can have a backslash in it
        //--- that will ruin javascript processing :-(
        for(int i=0; i<tlgrmData.identification.length(); i++)
        {
          if (tlgrmData.identification[i] == '\\') tlgrmData.identification[i] = '=';
          yield();
        }
      }

      if (tlgrmData.p1_version_be_present)
      {
        tlgrmData.p1_version = tlgrmData.p1_version_be;
        tlgrmData.p1_version_be_present  = false;
        tlgrmData.p1_version_present     = true;
      }

      modifySmFaseInfo();

      if (!tlgrmData.timestamp_present)
      {
        snprintf(gMsg,  _GMSG_LEN, "%02d%02d%02d%02d%02d%02d\0\0"
                      , (tzEurope.year() - 2000), tzEurope.month(), tzEurope.day()
                      , tzEurope.hour(), tzEurope.minute(), tzEurope.second());
        if (tzEurope.isDST())
              strlcat(gMsg, "S", _GMSG_LEN);
        else  strlcat(gMsg, "W", _GMSG_LEN);
        tlgrmData.timestamp         = gMsg;
        tlgrmData.timestamp_present = true;
      }

      //-- handle mbus delivered values
      gasDelivered = modifyMbusDelivered();

      processTelegram();
      if (Verbose2)
      {
        DebugTln("showValues: ");
        tlgrmData.applyEach(showValues());
      }

    }
    else                  // Parser error, print error
    {
      neoPixOn(1, neoPixRed);
      glowTimer1 = millis() + 5000;
      delay(1000);
      telegramErrors++;
      DebugTf("Parse error\r\n%s\r\n\r\n", tlgrmError.c_str());
      //--- set DTR to get a new telegram as soon as possible
      slimmeMeter.enable(true); 
      //slimmeMeter.loop();
    }

    if ( DUE(updateTlgrmCount) )
    {
      DebugTf("Processed [%d] telegrams ([%d] errors)\r\n", telegramCount, telegramErrors);
      writeToSysLog("Processed [%d] telegrams ([%d] errors)", telegramCount, telegramErrors);
    }

  } // if (slimmeMeter.available())

} // handleSlimmeMeter()

#endif  // ifndef _HAS_NO_SLIMMEMETER


//==================================================================================
void modifySmFaseInfo()
{
  if (!setting->SmHasFaseInfo)
  {
    if (tlgrmData.power_delivered_present && !tlgrmData.power_delivered_l1_present)
    {
      tlgrmData.power_delivered_l1 = tlgrmData.power_delivered;
      tlgrmData.power_delivered_l1_present = true;
      tlgrmData.power_delivered_l2_present = true;
      tlgrmData.power_delivered_l3_present = true;
    }
    if (tlgrmData.power_returned_present && !tlgrmData.power_returned_l1_present)
    {
      tlgrmData.power_returned_l1 = tlgrmData.power_returned;
      tlgrmData.power_returned_l1_present = true;
      tlgrmData.power_returned_l2_present = true;
      tlgrmData.power_returned_l3_present = true;
    }
  } // No Fase Info

} //  modifySmFaseInfo()


//==================================================================================
float modifyMbusDelivered()
{
  float tmpGasDelivered = 0;

  if (tlgrmData.mbus1_delivered_ntc_present)
    tlgrmData.mbus1_delivered = tlgrmData.mbus1_delivered_ntc;
  else if (tlgrmData.mbus1_delivered_dbl_present)
    tlgrmData.mbus1_delivered = tlgrmData.mbus1_delivered_dbl;
  tlgrmData.mbus1_delivered_present     = true;
  tlgrmData.mbus1_delivered_ntc_present = false;
  tlgrmData.mbus1_delivered_dbl_present = false;
  //dbg if (setting->Mbus1Type > 0) DebugTf("mbus1_delivered [%.3f]\r\n", (float)tlgrmData.mbus1_delivered);
  if ( (setting->Mbus1Type == 3) && (tlgrmData.mbus1_device_type == 3) )
  {
    tmpGasDelivered = (float)(tlgrmData.mbus1_delivered * 1.0);
    //dbg DebugTf("gasDelivered .. [%.3f]\r\n", tmpGasDelivered);
  }

  if (tlgrmData.mbus2_delivered_ntc_present)
    tlgrmData.mbus2_delivered = tlgrmData.mbus2_delivered_ntc;
  else if (tlgrmData.mbus2_delivered_dbl_present)
    tlgrmData.mbus2_delivered = tlgrmData.mbus2_delivered_dbl;
  tlgrmData.mbus2_delivered_present     = true;
  tlgrmData.mbus2_delivered_ntc_present = false;
  tlgrmData.mbus2_delivered_dbl_present = false;
  if (setting->Mbus2Type > 0) DebugTf("mbus2_delivered [%.3f]\r\n", (float)tlgrmData.mbus2_delivered);
  if ( (setting->Mbus2Type == 3) && (tlgrmData.mbus2_device_type == 3) )
  {
    tmpGasDelivered = (float)(tlgrmData.mbus2_delivered * 1.0);
    //dbg DebugTf("gasDelivered .. [%.3f]\r\n", tmpGasDelivered);
  }

  if (tlgrmData.mbus3_delivered_ntc_present)
    tlgrmData.mbus3_delivered = tlgrmData.mbus3_delivered_ntc;
  else if (tlgrmData.mbus3_delivered_dbl_present)
    tlgrmData.mbus3_delivered = tlgrmData.mbus3_delivered_dbl;
  tlgrmData.mbus3_delivered_present     = true;
  tlgrmData.mbus3_delivered_ntc_present = false;
  tlgrmData.mbus3_delivered_dbl_present = false;
  if (setting->Mbus3Type > 0) DebugTf("mbus3_delivered [%.3f]\r\n", (float)tlgrmData.mbus3_delivered);
  if ( (setting->Mbus3Type == 3) && (tlgrmData.mbus3_device_type == 3) )
  {
    tmpGasDelivered = (float)(tlgrmData.mbus3_delivered * 1.0);
    //dbg DebugTf("gasDelivered .. [%.3f]\r\n", tmpGasDelivered);
  }

  if (tlgrmData.mbus4_delivered_ntc_present)
    tlgrmData.mbus4_delivered = tlgrmData.mbus4_delivered_ntc;
  else if (tlgrmData.mbus4_delivered_dbl_present)
    tlgrmData.mbus4_delivered = tlgrmData.mbus4_delivered_dbl;
  tlgrmData.mbus4_delivered_present     = true;
  tlgrmData.mbus4_delivered_ntc_present = false;
  tlgrmData.mbus4_delivered_dbl_present = false;
  if (setting->Mbus4Type > 0) DebugTf("mbus4_delivered [%.3f]\r\n", (float)tlgrmData.mbus4_delivered);
  if ( (setting->Mbus4Type == 3) && (tlgrmData.mbus4_device_type == 3) )
  {
    tmpGasDelivered = (float)(tlgrmData.mbus4_delivered * 1.0);
    //dbg DebugTf("gasDelivered .. [%.3f]\r\n", tmpGasDelivered);
  }

  return tmpGasDelivered;

} //  modifyMbusDelivered()

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
/*eof*/
