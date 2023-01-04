/*
***************************************************************************  
**  Program  : readDSMR-logger (DSMRlogger32)
**
**  This program call's the DSMR-logger "actual" restAPI
**  and presents the value of the fields.
*/
#define _FW_VERSION "v5.0 (04-01-2023)"
/*
**  Copyright (c) 2023 Willem Aandewiel
**
**  TERMS OF USE: MIT License. See bottom of file.                                                            
***************************************************************************      
*/

//======= define type of board ===========
//==== only define one (1) board type ====
// #define _IS_ARDUINO_MEGA
// #define _IS_ESP8266 
// #define _IS_ESP32     
//========================================

//==== edit "myCredentials_org.h" ========
//==== and save it as "myCredentials.h" ==
#include "myCredentials.h"

//=====================================================
//======= leave everything below this line unchanged ==
//=====================================================

#if !defined(_IS_ARDUINO_MEGA) && !defined(_IS_ESP8266) && !defined(_IS_ESP32)
  #error Select one (and only one) board -- abort
#endif

#ifdef _IS_ARDUINO_MEGA 
  #include <ArduinoHttpClient.h>    // tested with version 0.4.0
  #include <Ethernet.h>
  #include <SPI.h>
#endif

#ifdef _IS_ESP8266
  #include <ESP8266WiFi.h>

  const char *ssid      = _WIFI_SSID;
  const char *password  = _WIFI_PASSWRD;
#endif

#ifdef _IS_ESP32
  #include <WiFi.h>
  #include <HTTPClient.h>

  const char *ssid      = _WIFI_SSID;
  const char *password  = _WIFI_PASSWRD;
#endif

//------ [ArduinoJson@6.19.3]
#include <ArduinoJson.h>
//-- if you need more fields make this JsonDocument bigger
StaticJsonDocument<350> dsmrDoc;
//-- if you need more fields make this JsonDocument bigger
StaticJsonDocument<150> filter;

#define _READINTERVAL     30000

const char *DSMRprotocol  = "http://";
const char *DSMRserverIP  = _DSMR_IP_ADDRESS;
const char *DSMRrestAPI   = "/api/v2/sm/actual";
String      payload;
int         httpResponseCode;
uint32_t    lastRead = 0;

//--- catch specific fields for further processing -------
//--- these are just an example! see readDsmrLogger() ----
String  timeStamp;
float   energyDelivered, pwrDelivered; 
float   energyReturned,  pwrReturned;
float   gasDelivered;
bool    firstCall = true;

#ifdef _IS_ARDUINO_MEGA 
//--------------------------------------------------------------------------
// Include in the main program:
//    #include <ArduinoHttpClient.h>    // version 0.4.0
//    #include <Ethernet.h>
//    #include <SPI.h>
//
// and in Setup() do something like:
//    // Initialize Ethernet library
//    byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
//
//    if (!Ethernet.begin(mac)) {
//      Serial.println("Failed to configure Ethernet");
//      return;
//    }
//    delay(1000);
//
//--------------------------------------------------------------------------
bool dsmrGETrequest() //-- for Arduino Mega
{
  EthernetClient ETHclient;
  HttpClient DSMRclient = HttpClient(ETHclient, DSMRserverIP, 80);

  payload = ""; 
   
  Serial.println(F("making GET request"));
  DSMRclient.get(DSMRrestAPI);

  // read the response code and body of the response
  httpResponseCode = DSMRclient.responseStatusCode();
  //-debug-Serial.print(F("http Response Code: "));
  //-debug-Serial.println(httpResponseCode);

  if (httpResponseCode <= 0)
  {
    Serial.print(F("http Response Code: "));
    Serial.println(httpResponseCode);
    return false;
  }

  payload    = DSMRclient.responseBody();

  //-debug-Serial.print(F("payload: "));
  //-debug-Serial.println(payload);

  deserializeJson(dsmrDoc, payload, DeserializationOption::Filter(filter));

  // Free resources
  DSMRclient.stop();

  if (firstCall)
  {
    Serial.print("dsmrDoc::");
    serializeJsonPretty(dsmrDoc, Serial);
    Serial.println();
  }
  firstCall = false;

  return true;
  
} // dsmrGETrequest()
#endif

#ifdef _IS_ESP8266
//--------------------------------------------------------------------------
// Include in the main program:
//    #include <ESP8266WiFi.h>
//--------------------------------------------------------------------------
bool dsmrGETrequest() //-- for ESP8266
{
  WiFiClient  DSMRclient;

  payload = ""; 

  Serial.print("DSMRclient.connect("); Serial.print(DSMRserverIP);
  Serial.println(", 80)");
  if (!DSMRclient.connect(DSMRserverIP, 80))
  {
    Serial.println(F("error connecting to DSMRlogger "));
    payload = "{\"actual\":[{\"name\":\"httpresponse\", \"value\":\"error connecting\"}]}";
    return false;
  }

  //-- send request
  DSMRclient.print(F("GET "));
  DSMRclient.print(DSMRrestAPI);
  DSMRclient.println(" HTTP/1.1");
  DSMRclient.print(F("Host: "));
  DSMRclient.println(DSMRserverIP);
  DSMRclient.println(F("Connection: close"));
  DSMRclient.println();
  delay(500);
  
  DSMRclient.setTimeout(900);

  DSMRclient.find("\r\n\r\n");  // skip everything up-until data seperator

  deserializeJson(dsmrDoc, DSMRclient, DeserializationOption::Filter(filter));
  // Free resources
  DSMRclient.stop();

  if (firstCall)
  {
    Serial.print("dsmrDoc::");
    serializeJsonPretty(dsmrDoc, Serial);
    Serial.println();
  }
  firstCall = false;
  
  return true;
  
} // dsmrGETrequest()
#endif

#ifdef _IS_ESP32
//--------------------------------------------------------------------------
// Include in the main program:
//    #include <WiFi.h>
//    #include <HTTPClient.h>
//--------------------------------------------------------------------------
bool dsmrGETrequest() //-- for ESP32
{
  HTTPClient DSMRclient;
    
  // Your IP address with path or Domain name with URL path 
  DSMRclient.begin(String(DSMRprotocol) + String(DSMRserverIP)+String(DSMRrestAPI));
  
  // Send HTTP GET request
  httpResponseCode = DSMRclient.GET();

  //-debug-Serial.print("HTTP Response code: ");
  //-debug-Serial.println(httpResponseCode);
  
  payload = ""; 
  
  if (httpResponseCode > 0) 
  {
    deserializeJson(dsmrDoc, DSMRclient.getString(), DeserializationOption::Filter(filter));
    if (firstCall)
    {
      Serial.print("dsmrDoc::");
      serializeJsonPretty(dsmrDoc, Serial);
      Serial.println();
    }
    firstCall = false;
  }
  else 
  {
    payload = "{\"actual\":[{\"name\":\"httpresponse\", \"value\": "+String(httpResponseCode)+"}]}";
    // Free resources
    DSMRclient.end();
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    return false;
  }

  // Free resources
  DSMRclient.end();

  return true;
  
} // dsmrGETrequest()
#endif


//--------------------------------------------------------------------------
void readDsmrLogger()
{
  int fieldNr = 0;

  dsmrGETrequest();
    
  // This is how the "actual" JSON object looks like:
  //  {
  //    "actual": {
  //      "timestamp": "230104092951W",
  //      "energy_delivered_tariff1": 6381.384,
  //      "energy_delivered_tariff2": 9852.761,
  //      "energy_returned_tariff1": 1183.1,
  //      "energy_returned_tariff2": 2600.5,
  //      "power_delivered": 1.53,
  //      "power_returned": 0,
  //      "voltage_l1": 221.4,
  //      "voltage_l2": 221.3,
  //      "voltage_l3": 227.7,
  //      "current_l1": 2,
  //      "current_l2": 0,
  //      "current_l3": 1,
  //      "power_delivered_l1": 0.911,
  //      "power_delivered_l2": 0.581,
  //      "power_delivered_l3": 0.034,
  //      "power_returned_l1": 0,
  //      "power_returned_l2": 0,
  //      "power_returned_l3": 0,
  //      "mbus1_delivered": 5403.24,
  //      "mbus2_delivered": 0,
  //      "mbus3_delivered": 0,
  //      "mbus4_delivered": 1080.65,
  //      "gas_delivered": 5403.239
  //    }
  //  }

  //-- store the fields you are interested in ----------
  timeStamp         = dsmrDoc["actual"]["timestamp"].as<const char*>();
  energyDelivered   = dsmrDoc["actual"]["energy_delivered_tariff1"].as<float>()
                    + dsmrDoc["actual"]["energy_delivered_tariff2"].as<float>();
  pwrDelivered      = dsmrDoc["actual"]["power_delivered"].as<float>();
  energyReturned    = dsmrDoc["actual"]["energy_returned_tariff1"].as<float>()
                    + dsmrDoc["actual"]["energy_returned_tariff2"].as<float>();
  pwrReturned       = dsmrDoc["actual"]["power_returned"].as<float>();
  gasDelivered      = dsmrDoc["actual"]["gas_delivered"].as<float>();
//voltageL1    = dsmrDoc["actual"]["voltage_l1"].as<int>();
//currentL1    = dsmrDoc["actual"]["current_l1"].as<int>();

  if (firstCall)
  {
    Serial.println();
    Serial.println(F("==== Start parsing payload ======================="));

    for (JsonPair p : dsmrDoc["actual"].as<JsonObject>())
    {
      Serial.print(p.key().c_str()); Serial.print("\t: ");
      if (p.value().is<int>())              Serial.println(p.value().as<int>());
      else if (p.value().is<float>())       Serial.println(p.value().as<float>());
      else if (p.value().is<const char*>()) Serial.println(p.value().as<const char*>());
      else Serial.println("unknown type");
    }
    Serial.println(F("=================================================="));
  }
      
} // readDsmrLogger()


//--------------------------------------------------------------------------
void setup() 
{
  Serial.begin(115200);
  while(!Serial) { /* wait a bit */ }

  Serial.println("");
  Serial.println(F("\r\n***************************************"));
  Serial.println(F("And then it all begins ..."));
  Serial.println(F("***************************************\r\n"));

  //-- The filter: it contains "true" for each value we want to capture
  filter["actual"]["timestamp"] = true;
  filter["actual"]["power_delivered"] = true;
  filter["actual"]["power_returned"] = true;
  filter["actual"]["gas_delivered"] = true;
  filter["actual"]["energy_delivered_tariff1"] = true;
  filter["actual"]["energy_delivered_tariff2"] = true;
  filter["actual"]["energy_returned_tariff1"] = true;
  filter["actual"]["energy_returned_tariff2"] = true;
  Serial.print("filterDoc::");
  serializeJsonPretty(filter, Serial);
  Serial.println();

#ifdef _IS_ARDUINO_MEGA
  // Initialize Ethernet library
  byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};

  if (!Ethernet.begin(mac)) 
  {
    Serial.println(F("Failed to configure Ethernet"));
    return;
  }
  delay(1000);
#endif

#if defined(_IS_ESP8266) || defined(_IS_ESP32)
  WiFi.begin(ssid, password);
  Serial.println(F("Connecting"));
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print(F("Connected to WiFi network with IP Address: "));
  Serial.println(WiFi.localIP());
#endif

  lastRead = millis() + _READINTERVAL;

  Serial.println(F("\r\nStart reading ..."));

} // setup()


//--------------------------------------------------------------------------
void loop() 
{
  if ((millis() - lastRead) > _READINTERVAL)
  {
    lastRead = millis();
    Serial.print(F("\r\nread API/v2 from DSMR-logger @"));
    Serial.println(_DSMR_IP_ADDRESS);
    readDsmrLogger();
    Serial.println(F("\r\nCaptured fields ==============================="));
    Serial.print(F("timestamp       : ")); Serial.println(timeStamp);
    Serial.print(F("energyDelivered : ")); Serial.println(energyDelivered);
    Serial.print(F("pwrDelivered    : ")); Serial.println(pwrDelivered);
    Serial.print(F("energyReturned  : ")); Serial.println(energyReturned);
    Serial.print(F("pwrReturned     : ")); Serial.println(pwrReturned);
    Serial.print(F("gasDelivered    : ")); Serial.println(gasDelivered);
    Serial.println(F("\r\n"));
  }
  
} // loop()

/*
****************************************************************************
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
