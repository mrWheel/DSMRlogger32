/*
***************************************************************************
**  Program  : handleTestData - part of DSMRlogger32
**  Version  : v5.n
**
**  Copyright (c) 2020 .. 2023 Willem Aandewiel
**
**  TERMS OF USE: MIT License. See bottom of file.
***************************************************************************
*/


#ifdef _HAS_NO_SLIMMEMETER

#define   MAXLINELENGTH     500   // longest normal line is 47 char (+3 for \r\n\0)

enum runStates { SInit, SMonth, SDay, SHour, SNormal };
enum runStates runMode = SNormal;

char        telegramLine[MAXLINELENGTH] = "";
int16_t     testTlgrmLines = 2; // needs value > 0
uint16_t    currentCRC;
int16_t     calcCRC;
uint32_t    actInterval = 5, nextMinute = 0, nextESPcheck = 0, nextGuiUpdate;
int8_t      State;
int16_t     actSec, actMinute, actHour, actDay, actMonth, actYear, actSpeed;
char        actDSMR[3] = "40", savDSMR[3] = "40";
double      ED_T1=0, ED_T2=0, ER_T1=0, ER_T2=0, V_l1=0, V_l2=0, V_l3=0, C_l1=0, C_l2=0, C_l3=0;
uint8_t     ETariffInd=1;
float       PDelivered, PReturned;
float       IPD_l1, IPD_l2, IPD_l3, IPR_l1, IPR_l2, IPR_l3;
float       CUR_l1, CUR_l2, CUR_l3;
float       GDelivered = 321.123;
bool        forceBuildRingFiles = false;
int16_t     forceBuildRecs;

//==================================================================================================
void handleTestdata()
{
  time_t nt;
  int16_t slot;

  DebugTf("Time for a new Telegram ..\r\n");
  if (forceBuildRingFiles)
  {
    switch(runMode)
    {
      case SInit:
        runMode = SMonth;
        forceBuildRecs = (devSetting->NoMonthSlots *2) +5;
        runMode = SMonth;
        nt = epoch(lastTlgrmTime.Timestamp, _TIMESTAMP_LEN, true);
        break;

      case SMonth:
        if (forceBuildRecs <= 0)
        {
          forceBuildRecs = (devSetting->NoDaySlots *2) +4;
          nt = epoch(lastTlgrmTime.Timestamp, _TIMESTAMP_LEN, true);
          runMode = SDay;
          break;
        }
        nt = now() + (15 * SECS_PER_DAY);
        DebugTf("Force build RING file for months -> rec[%2d]\r\n\n", forceBuildRecs);
        forceBuildRecs--;
        break;

      case SDay:
        if (forceBuildRecs <= 0)
        {
          forceBuildRecs = (devSetting->NoHourSlots * 2) +5;
          nt = epoch(lastTlgrmTime.Timestamp, _TIMESTAMP_LEN, true);
          runMode = SHour;
          break;
        }
        nt = now() + (SECS_PER_DAY / 2);
        DebugTf("Force build RING file for days -> rec[%2d]\r\n\n", forceBuildRecs);
        forceBuildRecs--;
        break;

      case SHour:
        if (forceBuildRecs <= 0)
        {
          forceBuildRingFiles= false;
          nt = epoch(lastTlgrmTime.Timestamp, _TIMESTAMP_LEN, true);
          DebugTln("Force build RING file back to normal operation\r\n\n");
          runMode = SNormal;
          break;
        }
        nt = now() + (SECS_PER_HOUR / 2);
        DebugTf("Force build RING file for hours -> rec[%2d]\r\n\n", forceBuildRecs);
        forceBuildRecs--;
        break;

      default:
        runMode = SNormal;
        forceBuildRingFiles = false;

    } // switch()
  }
  else     // normal operation mode
  {
    nt = now() + (SECS_PER_HOUR / 2);
    runMode = SNormal;
  }

  epochToTimestamp(nt, newTimestamp, sizeof(newTimestamp));

  updateMeterValues(runMode);

  testTlgrmLines = 0; // just to cickstart
  currentCRC     = 0;
  memset(tlgrmTmpData, 0, _TLGRM_LEN);
  DebugTln("Now build telegram ..");
  if (smSetting->PreDSMR40 == 1)
  {
    for (int16_t line = 0; line <= testTlgrmLines; line++)
    {
      yield();
      int16_t len = buildTelegram30(line, telegramLine);  // also: prints to DSMRsend
      //    calcCRC = decodeTelegram(len);  // why??
      httpServer.handleClient();
    }
    if (Verbose2) Debugf("!\r\n");
    strlcat(tlgrmTmpData, "!\r\n", _TLGRM_LEN);
  }
  else
  {
    for (int16_t line = 0; line <= testTlgrmLines; line++)
    {
      yield();
      int16_t len = buildTelegram(line, telegramLine);  // also: prints to DSMRsend
      calcCRC = decodeTelegram(len);
      httpServer.handleClient();
    }
    snprintf(gMsg,  _GMSG_LEN, "!%04X\r\n\r\n", (calcCRC & 0xFFFF));
    if (Verbose2) Debug(gMsg);
    strlcat(tlgrmTmpData, gMsg, _TLGRM_LEN);
  }

  DebugTf("telegramCount=[%d] telegramErrors=[%d]\r\n", telegramCount, telegramErrors);
  Debugln(F("\r\n[Time----][frHeap] Function------(line):\r"));
  //- Voorbeeld: [21:00:11][  9880] loop          ( 997): read telegram [28] => [140307210001S]
  telegramCount++;

  //--MyData
  tlgrmData = {};
  ParseResult<void> res;

  if (smSetting->PreDSMR40 == 1)
        res = P1Parser::parse(&tlgrmData, tlgrmTmpData, _TLGRM_LEN, false, false);
  else  res = P1Parser::parse(&tlgrmData, tlgrmTmpData, _TLGRM_LEN, false, true);

  if (res.err)
  {
    // Parsing error, show it
    //Debugln(res.fullError(tlgrmTmpData, tlgrmTmpData + lengthof(tlgrmTmpData)));
    Debugln(res.fullError(tlgrmTmpData, tlgrmTmpData + strlen(tlgrmTmpData)));
    neoPixOn(1, neoPixRed);
    glowTimer1 = millis() + 2000;
  }
  else
  {
    if (millis() > ( glowTimer1 + _GLOW_TIME) )
    {
      neoPixOn(1, neoPixGreen);
      glowTimer1 = millis();
    }
  }

  if (!tlgrmData.all_present())
  {
    if (Verbose2) DebugTln("DSMR: Some fields are missing");
  }
  // Succesfully parsed, now process the data!
  DebugTln("Processing tlgrmTmpData ..");
  // Succesfully parsed, now process the data!
  if (!tlgrmData.timestamp_present)
  {
    snprintf(gMsg, _GMSG_LEN, "%02d%02d%02d%02d%02d%02dW\0\0"
          //, (year(nt) - 2000), month(nt), day(nt)
            , tzEurope.year(), tzEurope.month(), tzEurope.day()
            , hour(nt), minute(nt), second(nt));
    tlgrmData.timestamp         = gMsg;
    tlgrmData.timestamp_present = true;
  }

  gasDelivered = modifyMbusDelivered();
  modifySmFaseInfo();

  processTelegram();

} // handleTestdata()


//==================================================================================================
int16_t buildTelegram(int16_t line, char telegramLine[])
{
  int16_t len = 0;

  float val;

  switch (line)
  {
    //XMX5LGBBLB2410065887
    case 0:
      snprintf(telegramLine, MAXLINELENGTH, "/XMX5LGBBLB2410065887\r\n");
      break;
    case 1:
      snprintf(telegramLine, MAXLINELENGTH, "\r\n");
      break;
    case 2:
      snprintf(telegramLine, MAXLINELENGTH, "1-3:0.2.8(50)\r\n");
      break;
    case 3:
      snprintf(telegramLine, MAXLINELENGTH, "0-0:1.0.0(%12.12sS)\r\n", newTimestamp);
      break;
    case 4:
      snprintf(telegramLine, MAXLINELENGTH, "0-0:96.1.1(4530303336303000000000000000000040)\r\n", val);
      break;
    case 5:   // Energy Delivered
      snprintf(telegramLine, MAXLINELENGTH, "1-0:1.8.1(%s*kWh)\r\n", Format(ED_T1, 10, 3).c_str());
      break;
    case 6:
      snprintf(telegramLine, MAXLINELENGTH, "1-0:1.8.2(%s*kWh)\r\n", Format(ED_T2, 10, 3).c_str());
      break;
    case 7:   // Energy Returned
      snprintf(telegramLine, MAXLINELENGTH, "1-0:2.8.1(%s*kWh)\r\n", Format(ER_T1, 10, 3).c_str());
      break;
    case 8:
      snprintf(telegramLine, MAXLINELENGTH, "1-0:2.8.2(%s*kWh)\r\n", Format(ER_T2, 10, 3).c_str());
      break;
    case 9:   // Tariff indicator electricity
      snprintf(telegramLine, MAXLINELENGTH, "0-0:96.14.0(%04d)\r\n", ETariffInd);
      break;
    case 10:  // Actual electricity power delivered (+P) in 1 Watt resolution
      snprintf(telegramLine, MAXLINELENGTH, "1-0:1.7.0(%s*kW)\r\n", Format(PDelivered, 6, 2).c_str());
      break;
    case 11:  // Actual electricity power received (-P) in 1 Watt resolution
      snprintf(telegramLine, MAXLINELENGTH, "1-0:2.7.0(%s*kW)\r\n", Format(PReturned, 6, 2).c_str());
      break;
    case 12:  // Number of power failures in any phase
      snprintf(telegramLine, MAXLINELENGTH, "0-0:96.7.21(00010)\r\n", val);
      break;
    case 13:  // Number of long power failures in any phase
      snprintf(telegramLine, MAXLINELENGTH, "0-0:96.7.9(00000)\r\n", val);
      break;
    case 14:  // Power Failure Event Log (long power failures)
      snprintf(telegramLine, MAXLINELENGTH, "1-0:99.97.0(10)(0-0:96.7.19)"
              "(190508094303S)(0000055374*s)"
              "(190507165813S)(0000007991*s)(190507141021S)(0000000274*s)"
              "(190507135954S)(0000000649*s)(190507134811S)(0000083213*s)"
              "(190506143928S)(0000090080*s)(190505123501S)(0000073433*s)"
              "(190504152603S)(0000003719*s)(190504120844S)(0000337236*s)"
              "(190430142638S)(0000165493*s)\r\n", val);
      break;
    case 15:  // Number of voltage sags in phase L1
      snprintf(telegramLine, MAXLINELENGTH, "1-0:32.32.0(00002)\r\n", val);
      break;
    case 16:  // Number of voltage sags in phase L2 (polyphase meters only)
      snprintf(telegramLine, MAXLINELENGTH, "1-0:52.32.0(00003)\r\n", val);
      break;
    case 17:  // Number of voltage sags in phase L3 (polyphase meters only)
      snprintf(telegramLine, MAXLINELENGTH, "1-0:72.32.0(00003)\r\n", val);
      break;
    case 18:  // Number of voltage swells in phase L1
      snprintf(telegramLine, MAXLINELENGTH, "1-0:32.36.0(00000)\r\n", val);
      break;
    case 19:  // Number of voltage swells in phase L2
      snprintf(telegramLine, MAXLINELENGTH, "1-0:52.36.0(00000)\r\n", val);
      break;
    case 20:  // Number of voltage swells in phase L3
      snprintf(telegramLine, MAXLINELENGTH, "1-0:72.36.0(00000)\r\n", val);
      break;
    case 21:  // Text message max 2048 characters
      snprintf(telegramLine, MAXLINELENGTH, "0-0:96.13.0()\r\n", val);
      break;
    case 22:  // Instantaneous voltage L1 in 0.1V resolution
      snprintf(telegramLine, MAXLINELENGTH, "1-0:32.7.0(%03d.0*V)\r\n", (240 + random(-3, 3)));
      break;
    case 23:  // Instantaneous voltage L1 in 0.1V resolution
      snprintf(telegramLine, MAXLINELENGTH, "1-0:52.7.0(%03d.0*V)\r\n", (238 + random(-3, 3)));
      break;
    case 24:  // Instantaneous voltage L1 in 0.1V resolution
      snprintf(telegramLine, MAXLINELENGTH, "1-0:72.7.0(%03d.0*V)\r\n", (236 + random(-3, 3)));
      break;
    case 25:  // Instantaneous current L1 in A resolution
      snprintf(telegramLine, MAXLINELENGTH, "1-0:31.7.0(%s*A)\r\n", Format(CUR_l1, 6, 2).c_str());
      break;
    case 26:  // Instantaneous current L2 in A resolution
      snprintf(telegramLine, MAXLINELENGTH, "1-0:51.7.0(%03d*A)\r\n",  random(0, 4));
      break;
    case 27:  // Instantaneous current L3 in A resolution
      snprintf(telegramLine, MAXLINELENGTH, "1-0:71.7.0(%s*A)\r\n", Format(CUR_l3, 6, 2).c_str());
      break;
    case 28:  // Instantaneous active power L1 (+P) in W resolution
      snprintf(telegramLine, MAXLINELENGTH, "1-0:21.7.0(%s*kW)\r\n", Format(IPD_l1, 6, 3).c_str());
      break;
    case 29:  // Instantaneous active power L2 (+P) in W resolution
      snprintf(telegramLine, MAXLINELENGTH, "1-0:41.7.0(%s*kW)\r\n", Format(IPD_l2, 6, 3).c_str());
      break;
    case 30:  // Instantaneous active power L3 (+P) in W resolution
      snprintf(telegramLine, MAXLINELENGTH, "1-0:61.7.0(%s*kW)\r\n", Format(IPD_l3, 6, 3).c_str());
      break;
    case 31:  // Instantaneous active power L1 (-P) in W resolution
      snprintf(telegramLine, MAXLINELENGTH, "1-0:22.7.0(%s*kW)\r\n", Format(IPR_l1, 6, 3).c_str());
      break;
    case 32:  // Instantaneous active power L2 (-P) in W resolution
      snprintf(telegramLine, MAXLINELENGTH, "1-0:42.7.0(%s*kW)\r\n", Format(IPR_l2, 6, 3).c_str());
      break;
    case 33:  // Instantaneous active power L3 (-P) in W resolution
      snprintf(telegramLine, MAXLINELENGTH, "1-0:62.7.0(%s*kW)\r\n", Format(IPR_l3, 6, 3).c_str());
      break;

    case 34:  // [1] Gas Device-Type
      snprintf(telegramLine, MAXLINELENGTH, "0-1:24.1.0(003)\r\n", val);
      break;
    case 35:  // Equipment identifier (Gas)
      snprintf(telegramLine, MAXLINELENGTH, "0-1:96.1.0(4730303339303031363532303530323136)\r\n", val);
      break;
    case 36:  // Last 5-minute value (temperature converted), gas delivered to client
      // in m3, including decimal values and capture time (Note: 4.x spec has
      snprintf(telegramLine, MAXLINELENGTH, "0-1:24.2.1(%02d%02d%02d%02d%02d01S)(%s*m3)\r\n", (year() - 2000), month(), day(), hour(), minute(),
              Format(GDelivered, 9, 3).c_str());
      break;

    case 37:  // [2] Device-Type
      snprintf(telegramLine, MAXLINELENGTH, "0-2:24.1.0(005)\r\n", val);
      break;
    case 38:  // Equipment identifier [4]
      snprintf(telegramLine, MAXLINELENGTH, "0-2:96.1.0(4730303339303031322222222222222222)\r\n", val);
      break;
    case 39:  //  0-2:24.4.0(1) - spare [4] valve_position
      snprintf(telegramLine, MAXLINELENGTH, "0-2:24.4.0(%1d)\r\n", (telegramCount%2));
      break;
    case 40:  // [2]
      snprintf(telegramLine, MAXLINELENGTH, "0-2:24.2.1(%02d%02d%02d%02d%02d01S)(%s*GJ)\r\n", (year() - 2000), month(), day(), hour(), minute(),
              Format((GDelivered / 5), 9, 3).c_str());
      break;

    case 41:  // [4] Device-Type
      snprintf(telegramLine, MAXLINELENGTH, "0-4:24.1.0(003)\r\n", val);
      break;
    case 42:  // Equipment identifier [4]
      snprintf(telegramLine, MAXLINELENGTH, "0-4:96.1.0(4730303339303031344444444444444444)\r\n", val);
      break;
    case 43:  //  0-4:24.4.0(1) - spare [4] valve_position
      snprintf(telegramLine, MAXLINELENGTH, "0-4:24.4.0(%1d)\r\n", (telegramCount%2));
      break;
    case 44:  // [4]
      snprintf(telegramLine, MAXLINELENGTH, "0-4:24.2.1(%02d%02d%02d%02d%02d01S)(%s*GJ)\r\n", (year() - 2000), month(), day(), hour(), minute(),
              Format((GDelivered / 3), 9, 3).c_str());
      break;

    case 45:
      snprintf(telegramLine, MAXLINELENGTH, "!xxxx\r\n");
      break;

  } // switch(line)

  testTlgrmLines   = 45;
  if (line < testTlgrmLines)
  {
    if (Verbose2) Debug(telegramLine);
    strlcat(tlgrmTmpData, telegramLine, _TLGRM_LEN);
  }

  for(len = 0; len < MAXLINELENGTH, telegramLine[len] != '\0'; len++) {}

  //return len;
  return strlen(telegramLine);

} // buildTelegram()


//==================================================================================================
int16_t buildTelegram30(int16_t line, char telegramLine[])
{
  /*
  **  /KMP5 KA6U001585575011                - Telegram begin-marker + manufacturer + serial number
  **
  **  0-0:96.1.1(204B413655303031353835353735303131)    -  Serial number in hex
  **  1-0:1.8.1(08153.408*kWh)                          -  +T1: Energy input, low tariff (kWh)
  **  1-0:1.8.2(05504.779*kWh)                          -  +T2: Energy input, normal tariff (kWh)
  **  1-0:2.8.1(00000.000*kWh)                          -  -T3: Energy output, low tariff (kWh)
  **  1-0:2.8.2(00000.000*kWh)                          -  -T4: Energy output, normal tariff (kWh)
  **  0-0:96.14.0(0002)                                 -  Current tariff (1=low, 2=normal)
  **  1-0:1.7.0(0000.30*kW)                             -  Actual power input (kW)
  **  1-0:2.7.0(0000.00*kW)                             -  Actual power output (kW)
  **  0-0:17.0.0(999*A)                                 -  Max current per phase (999=no max)
  **  0-0:96.3.10(1)                                    -  Switch position
  **  0-0:96.13.1()                                     -  Message code
  **  0-0:96.13.0()                                     -  Message text
  **  0-1:24.1.0(3)                                     -  Attached device type identifier
  **  0-1:96.1.0(3238313031353431303031333733353131)    -  Serial number of gas meter
  **  0-1:24.3.0(190718190000)(08)(60)(1)(0-1:24.2.1)(m3) -  Time of last gas meter update
  **  (04295.190)                                       -  Gas meter value (m³)
  **  0-1:24.4.0(1)                                     -  Gas valve position
  **  !                                                 -  Telegram end-marker
  **
  */
  //==================================================================================================
  int16_t len = 0;

  float val;

  switch (line)
  {
    //KMP5 KA6U001585575011
    case 0:
      snprintf(telegramLine, MAXLINELENGTH, "/KMP5 KA6U001585575011\r\n");
      break;
    case 1:
      snprintf(telegramLine, MAXLINELENGTH, "\r\n");
      break;
    case 2:
      snprintf(telegramLine, MAXLINELENGTH, "0-0:96.1.1(4530303336303000000000000000000000)\r\n", val);
      break;
    case 3:   // Energy Delivered
      snprintf(telegramLine, MAXLINELENGTH, "1-0:1.8.1(%s*kWh)\r\n", Format(ED_T1, 10, 3).c_str());
      break;
    case 4:
      snprintf(telegramLine, MAXLINELENGTH, "1-0:1.8.2(%s*kWh)\r\n", Format(ED_T2, 10, 3).c_str());
      break;
    case 5:   // Energy Returned
      snprintf(telegramLine, MAXLINELENGTH, "1-0:2.8.1(%s*kWh)\r\n", Format(ER_T1, 10, 3).c_str());
      break;
    case 6:
      snprintf(telegramLine, MAXLINELENGTH, "1-0:2.8.2(%s*kWh)\r\n", Format(ER_T2, 10, 3).c_str());
      break;
    case 7:   // Tariff indicator electricity
      snprintf(telegramLine, MAXLINELENGTH, "0-0:96.14.0(%04d)\r\n", ETariffInd);
      break;
    case 8:   // Actual electricity power delivered (+P) in 1 Watt resolution
      snprintf(telegramLine, MAXLINELENGTH, "1-0:1.7.0(%s*kW)\r\n", Format(PDelivered, 6, 2).c_str());
      break;
    case 9:   // Actual electricity power received (-P) in 1 Watt resolution
      snprintf(telegramLine, MAXLINELENGTH, "1-0:2.7.0(%s*kW)\r\n", Format(PReturned, 6, 2).c_str());
      break;
    case 10:  // Max current per phase (999=no max)
      snprintf(telegramLine, MAXLINELENGTH, "0-0:17.0.0(016*A)\r\n", val);
      break;
    case 11:  // Switch position (?)
      snprintf(telegramLine, MAXLINELENGTH, "0-0:96.3.10(1)\r\n", val);
      break;
    case 12:  // Text message code
      snprintf(telegramLine, MAXLINELENGTH, "0-0:96.13.1()\r\n", val);
      break;
    case 13:  // Text message text
      snprintf(telegramLine, MAXLINELENGTH, "0-0:96.13.0()\r\n", val);
      break;
    case 14:  // Gas Device-Type
      snprintf(telegramLine, MAXLINELENGTH, "0-1:24.1.0(3)\r\n", val);
      break;
    case 15:  // Equipment identifier (Gas)
      snprintf(telegramLine, MAXLINELENGTH, "0-1:96.1.0(4730303339303031363500000000000000)\r\n", val);
      break;
    case 16:  // Last 5-minute value (temperature converted), gas delivered to client
      // in m3, including decimal values and capture time
      snprintf(telegramLine, MAXLINELENGTH, "0-1:24.3.0(%02d%02d%02d%02d%02d00)(08)(60)(1)(0-1:24.2.1)(m3)\r\n", (year() - 2000), month(), day(), hour(), minute());
      break;
    case 17:
      snprintf(telegramLine, MAXLINELENGTH, "(%s)\r\n", Format(GDelivered, 9, 3).c_str());
      break;
    case 18:  // Gas valve position
      snprintf(telegramLine, MAXLINELENGTH, "0-1:24.4.0(1)\r\n", val);
      break;
    case 19:
      snprintf(telegramLine, MAXLINELENGTH, "!\r\n\r\n");     // just for documentation
      break;

  } // switch(line)
  testTlgrmLines = 19;
  if (line < testTlgrmLines)
  {
    if (Verbose2) Debug(telegramLine);
    strlcat(tlgrmTmpData, telegramLine, _TLGRM_LEN);
  }

  for(len = 0; len < MAXLINELENGTH, telegramLine[len] != '\0'; len++) {}

  return len;

} // buildTelegram30()



//==================================================================================================
void updateMeterValues(uint8_t period)
{
  float  Factor = 1.098;
  String wsString = "";
  DebugTf("period[%d]\r\n", period);
  /*
  switch(period) {
    case SMonth:  Factor = 30.0 * 24.0; break;
    case SDay:    Factor = 24.0;        break;
    case SHour:   Factor = 1.0;         break;
    default:      Factor = 1.0;
  }
  **/
  ED_T1      += (float)((random(200, 2200) / 3600000.0) * actInterval) * Factor;
  ED_T2      += (float)((random(100, 9000) / 3600000.0) * actInterval) * Factor;
  if (hour(actT) >= 4 && hour(actT) <= 20)
  {
    ER_T1      += (float)((random(0, 400)  / 3600000.0) * actInterval) * Factor;
    ER_T2      += (float)((random(0, 200)  / 3600000.0) * actInterval) * Factor;
    ETariffInd  = 1;
  }
  else
  {
    ETariffInd  = 2;
  }
  GDelivered += (float)(random(2, 25) / 10000.0) * Factor;    // Gas Delevered
  V_l1        = (float)(random(220, 240) * 1.01);     // Voltages
  V_l2        = (float)(random(220, 240) * 1.02);
  V_l3        = (float)(random(220, 240) * 1.03);
  C_l1        = (float)(random(1, 20) * 1.01);         // Current
  C_l2        = (float)(random(1, 15) * 1.02);
  C_l3        = (float)(random(1, 10) * 1.03);
  IPD_l1      = (float)(random(1, 1111) * 0.001102);
  IPD_l2      = (float)(random(1, 892)  * 0.001015);
  IPD_l3      = (float)(random(1, 773)  * 0.001062);
  if (hour(actT) >= 4 && hour(actT) <= 20)
  {
    IPR_l1    = (float)(random(1, 975) * 0.01109);
    IPR_l2    = (float)(random(1, 754) * 0.01031);
    IPR_l3    = (float)(random(1, 613) * 0.01092);

  }
  else        // 's-nachts geen opwekking van energy!
  {
    IPR_l1    = 0.0;
    IPR_l2    = 0.0;
    IPR_l3    = 0.0;
  }
  PDelivered  = (float)(IPD_l1 + IPD_l2 + IPD_l3) / 1.0;       // Power Delivered
  PReturned   = (float)(IPR_l1 + IPR_l2 + IPR_l3) / 1.0;       // Power Returned
  CUR_l1      = (float)(random(0, 3000) * 0.001);
  CUR_l2      = (float)(random(0, 25000) * 0.001);
  CUR_l3      = (float)(random(500, 3000) * 0.001);

  if (Verbose2) Debugf("l1[%5d], l2[%5d], l3[%5d] ==> l1+l2+l3[%9.3f]\n"
                         , (int)(IPD_l1 * 1000)
                         , (int)(IPD_l2 * 1000)
                         , (int)(IPD_l3 * 1000)
                         , PDelivered);

} // updateMeterValues()


//==================================================================================================
String Format(double x, int len, int d)
{
  String r;
  int rl;

  r = String(x, d);
  //Debugf("Format(%s, %d, %d)\n", r.c_str(), len, d);
  while (r.length() < len) r = "0" + r;
  rl = r.length();
  if (rl > len)
  {
    return r.substring((rl - len));
  }
  return r;

} // Format()



//==================================================================================================
int FindCharInArrayRev(unsigned char array[], char c, int len)
{
  for (int16_t i = len - 1; i >= 0; i--)
  {
    if (array[i] == c)
    {
      return i;
    }
  }
  return -1;
}


//==================================================================================================
int16_t decodeTelegram(int len)
{
  //need to check for start
  int startChar = FindCharInArrayRev((unsigned char *)telegramLine, '/', len);
  int endChar   = FindCharInArrayRev((unsigned char *)telegramLine, '!', len);

  bool validCRCFound = false;
  if(startChar>=0)
  {
    //start found. Reset CRC calculation
    currentCRC=CRC16(0x0000, (unsigned char *) telegramLine+startChar, len-startChar);

  }
  else if(endChar>=0)
  {
    //add to crc calc
    currentCRC=CRC16(currentCRC, (unsigned char *)telegramLine+endChar, 1);

  }
  else
  {
    currentCRC=CRC16(currentCRC, (unsigned char *)telegramLine, len);
  }

  return currentCRC;

} // decodeTelegram()

#endif

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
