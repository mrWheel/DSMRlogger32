/*
***************************************************************************
**  Program  : MQTTstuff, part of DSMRlogger32
**  Version  : v5.n
**
**  Copyright (c) 2020 .. 2024 Willem Aandewiel
**
**  TERMS OF USE: MIT License. See bottom of file.
***************************************************************************
*/
#include "MQTTstuff.h"

// Declare some variables within global scope

IPAddress  MQTTbrokerIP;
char       MQTTbrokerIPchar[20];

//-- MQTT client publish and subscribe functionality
//-- https://github.com/knolleary/pubsubclient
//-- #include <PubSubClient.h>

//-- static PubSubClient MQTTclient(wifiClient);
int8_t              reconnectAttempts = 0;
char                lastMQTTtimestamp[15] = "-";
char                mqttBuff[100];

/*				*** enum moved to arduinoGlue.h ***
enum states_of_MQTT { MQTT_STATE_INIT, MQTT_STATE_TRY_TO_CONNECT, MQTT_STATE_IS_CONNECTED, MQTT_STATE_ERROR };
*/
enum states_of_MQTT stateMQTT = MQTT_STATE_INIT;

String            MQTTclientId;

//===========================================================================================
void connectMQTT()
{
  if (Verbose2) DebugTf("MQTTclient.connected(%d), mqttIsConnected[%d], stateMQTT [%d]\r\n"
                          , MQTTclient.connected()
                          , mqttIsConnected, stateMQTT);

  if (devSetting->MQTTinterval == 0)
  {
    mqttIsConnected = false;
    return;
  }

  if (!MQTTclient.connected() || stateMQTT != MQTT_STATE_IS_CONNECTED)
  {
    mqttIsConnected = false;
    stateMQTT = MQTT_STATE_INIT;
  }

  mqttIsConnected = connectMQTT_FSM();

  if (Verbose1) DebugTf("connected()[%d], mqttIsConnected[%d], stateMQTT [%d]\r\n"
                          , MQTTclient.connected()
                          , mqttIsConnected, stateMQTT);

  CHANGE_INTERVAL_MIN(reconnectMQTTtimer,  5);

} //  connectMQTT()


//===========================================================================================
bool connectMQTT_FSM()
{
  switch(stateMQTT)
  {
    case MQTT_STATE_INIT:
      DebugTln(F("MQTT State: MQTT Initializing"));
      WiFi.hostByName(devSetting->MQTTbroker, MQTTbrokerIP);  // lookup the MQTTbroker convert to IP
      snprintf(MQTTbrokerIPchar, sizeof(MQTTbrokerIPchar), "%d.%d.%d.%d"
                                                            , MQTTbrokerIP[0]
                                                            , MQTTbrokerIP[1]
                                                            , MQTTbrokerIP[2]
                                                            , MQTTbrokerIP[3]);
      if (!isValidIP(MQTTbrokerIP))
      {
        //-- devSetting->MQTTinterval = 0;
        DebugTf("ERROR: [%s] => extend interval\r\n", devSetting->MQTTbroker);
        DebugTln(F("Next State: MQTT_STATE_ERROR"));
        stateMQTT = MQTT_STATE_ERROR;
        return false;
      }
      //MQTTclient.disconnect();
      //DebugTf("disconnect -> MQTT status, rc=%d \r\n", MQTTclient.state());
      DebugTf("[%s] => setServer(%s, %d) \r\n", devSetting->MQTTbroker, MQTTbrokerIPchar, devSetting->MQTTbrokerPort);
      MQTTclient.setServer(MQTTbrokerIPchar, devSetting->MQTTbrokerPort);
      DebugTf("setServer  -> MQTT status, rc=%d \r\n", MQTTclient.state());
      MQTTclientId  = String(devSetting->Hostname) + "-" + WiFi.macAddress();
      stateMQTT = MQTT_STATE_TRY_TO_CONNECT;
      DebugTln(F("Next State: MQTT_STATE_TRY_TO_CONNECT"));
      reconnectAttempts = 0;

    case MQTT_STATE_TRY_TO_CONNECT:
      DebugTln(F("MQTT State: MQTT try to connect"));
      DebugTf("MQTT server is [%s], IP[%s]\r\n", devSetting->MQTTbroker, MQTTbrokerIPchar);

      DebugTf("Attempting MQTT connection as [%s] .. \r\n", MQTTclientId.c_str());
      reconnectAttempts++;

      //--- If no username, then anonymous connection to broker, otherwise assume username/password.
      if (String(devSetting->MQTTuser).length() == 0)
      {
        DebugT(F("without a Username/Password "));
        MQTTclient.connect(MQTTclientId.c_str());
      }
      else
      {
        DebugTf("with Username [%s] and password ", devSetting->MQTTuser);
        MQTTclient.connect(MQTTclientId.c_str(), devSetting->MQTTuser, devSetting->MQTTpasswd);
      }
      //--- If connection was made succesful, move on to next state...
      if (MQTTclient.connected())
      {
        reconnectAttempts = 0;
        Debugf(" .. connected -> MQTT status, rc=%d\r\n", MQTTclient.state());
        MQTTclient.loop();
        stateMQTT = MQTT_STATE_IS_CONNECTED;
        return true;
      }
      Debugf(" -> MQTT status, rc=%d \r\n", MQTTclient.state());

      //--- After 3 attempts... go wait for a while.
      if (reconnectAttempts >= 3)
      {
        DebugTln(F("3 attempts have failed. Retry wait for next reconnect in 10 minutes\r"));
        stateMQTT = MQTT_STATE_ERROR;  // if the re-connect did not work, then return to wait for reconnect
        DebugTln(F("Next State: MQTT_STATE_ERROR"));
      }
      break;

    case MQTT_STATE_IS_CONNECTED:
      MQTTclient.loop();
      return true;

    case MQTT_STATE_ERROR:
      DebugTln(F("MQTT State: MQTT ERROR, wait for 10 minutes, before trying again"));
      //--- next retry in 10 minutes.
      CHANGE_INTERVAL_MIN(reconnectMQTTtimer, 10);
      break;

    default:
      DebugTln(F("MQTT State: default, this should NEVER happen!"));
      //--- do nothing, this state should not happen
      stateMQTT = MQTT_STATE_INIT;
      CHANGE_INTERVAL_MIN(reconnectMQTTtimer, 10);
      DebugTln(F("Next State: MQTT_STATE_INIT"));
      break;
  }

  return false;

} // connectMQTT_FSM()


//===========================================================================================
void sendMQTTData()
{
  DebugTf("MQTTinterval [%d]\r\n", devSetting->MQTTinterval);
  if (devSetting->MQTTinterval == 0) return;

  if (!MQTTclient.connected() || ! mqttIsConnected)
  {
    DebugTf("MQTTclient.connected(%d), mqttIsConnected[%d], stateMQTT [%d]\r\n"
            , MQTTclient.connected()
            , mqttIsConnected, stateMQTT);
  }
  if (!MQTTclient.connected())
  {
    if ( DUE( reconnectMQTTtimer) || mqttIsConnected)
    {
      mqttIsConnected = false;
      connectMQTT();
    }
    else
    {
      DebugTf("trying to reconnect in less than %d minutes\r\n", (TIME_LEFT_MIN(reconnectMQTTtimer) +1) );
    }
    if ( !mqttIsConnected )
    {
      DebugTln("no connection with a MQTT broker ..");
      return;
    }
  }

  memset(fieldTable,  0, (sizeof(fieldTableStruct) *100));
  memset(fieldsArray, 0, sizeof(fieldsArray));
  fieldTableCount = 0;

  onlyIfPresent   = false;
  tlgrmData.applyEach(buildJsonV2ApiSm());
  addToTable("gas_delivered", gasDelivered);

  for(int i=0; i<fieldTableCount; i++)
  {
    memset(jsonBuff, 0, _JSONBUFF_LEN);
    //-- Allocate the JsonDocument
    DynamicJsonDocument doc(3000);

    switch (fieldTable[i].cType)
    {
      case 'i': //Debugf("val(%c)[%d]\r\n", fieldTable[i].cType, fieldTable[i].type.iValue);
                doc[fieldTable[i].cName] = fieldTable[i].type.iValue;
                break;
      case 'u': //Debugf("val(%c)[%d]\r\n", fieldTable[i].cType, fieldTable[i].type.uValue);
                doc[fieldTable[i].cName] = fieldTable[i].type.uValue;
                break;
      case 'f': //Debugf("val(%c)[%.3f]\r\n", fieldTable[i].cType, fieldTable[i].type.fValue);
                if (strncmp(fieldTable[i].cName, "voltage", 7) == 0)
                      doc[fieldTable[i].cName] = round1(fieldTable[i].type.fValue);
                else  doc[fieldTable[i].cName] = round3(fieldTable[i].type.fValue);
                break;
      default:  //Debugf("val(%c)[%s]\r\n", fieldTable[i].cType, fieldTable[i].type.cValue);
                doc[fieldTable[i].cName] = fieldTable[i].type.cValue;
    }
    serializeJson(doc, jsonBuff, _JSONBUFF_LEN);

    //-- calculate maxPayload:
    //-- https://arduino.stackexchange.com/questions/76840/pubsubclient-mqtt-max-packet-size-how-is-it-calculated
    int maxBuff = MQTT_MAX_PACKET_SIZE - MQTT_MAX_HEADER_SIZE - 2  - strlen(devSetting->MQTTtopTopic);
    if (jsonBuff[maxBuff] != 0)
    {
      //-dbg-Debugln(jsonBuff);
      jsonBuff[maxBuff-5] = '+';
      jsonBuff[maxBuff-4] = '+';
      jsonBuff[maxBuff-3] = '+';
      jsonBuff[maxBuff-2] = '"';
      jsonBuff[maxBuff-1] = '}';
      jsonBuff[maxBuff]   =  0;
    }

    if (!MQTTclient.publish(devSetting->MQTTtopTopic, jsonBuff) )
    {
      DebugTf("Error publish(%s) [%s] [%d bytes]\r\n", devSetting->MQTTtopTopic
                                                     , jsonBuff
                                                     , (strlen(devSetting->MQTTtopTopic) + strlen(jsonBuff)));
    }

  } //-- for all enttries in table

} // sendMQTTData()

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
****************************************************************************
*/