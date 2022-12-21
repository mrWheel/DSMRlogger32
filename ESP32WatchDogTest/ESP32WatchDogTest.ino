/*
 * neoPixelTest WatchDog for ESP8266/ESP32
 * 
 * Copyright 2022, 2023 Willem Aandewiel
 * Version 3.0  10-11-2022
 * 
 * 
 * Arduino IDE version 1.8.19
 * 
 *   - Board             : "ESP32 {Dev Module | Wrover Module}" 
 *   // Flash Size       : "4MB (32Mb)"
 *   - Flash Mode        : "QIO"
 *   - Partition Scheme  : "Default 4MB with spiffs (1.2MB APP/1.5MB SPIFFS)"
 *   - Core Debug Level  : "None" ??
 *   // PSRAM            : "Enabled"
 *   // Arduino Runs On  : "Core 1"
 *   // Events Run On    : "Core 0"
 *
 */

//-- Watchdog defines ---------
#define _HALF_A_SECOND      500       // half a second
#define _MAX_HALF_SECONDS    40       // Max 20 seconds elapse befoure WDT kicks in!
#define _STARTUP_TIME      2000       // 25 seconden
#define _MAX_NO_HARTBEAT  25000       // 60 seconds
#define _LAST_WARNING     15000 
#define _FIRST_WARNING    10000 

#include <Adafruit_NeoPixel.h>


//#define _PIN_NEO_LED         12       // GPIO-00 ==> DIL-6 ==> PB0
#define _PIN_NEO_LED         12       // ESP32
#define _PIN_DUMM1            1       // GPIO-01 ==> DIL-5 ==> PB1
//#define _PIN_INTERRUPT        2       // GPIO-02 ==> DIL-7 ==> PB2  / INT0
#define _PIN_INTERRUPT       27       // ESP32
#define _PIN_ESP32_EN         3       // GPIO-03 ==> DIL-2 ==> PB3
#define _PIN_DUMMY4           4       // GPIO-04 ==> DIL-3 ==> PB4

//-- How many NeoPixels are attached to the Arduino?
#define NEO_LED_COUNT         8

//-- NeoPixel brightness, 0 (min) to 255 (max)
#define NEO_BRIGHTNESS       50 // 

// Declare our NeoPixel strip object:
Adafruit_NeoPixel neoPixel(NEO_LED_COUNT, _PIN_NEO_LED, NEO_GRB + NEO_KHZ800);
// Argument 1 = Number of pixels in NeoPixel strip
// Argument 2 = Arduino pin number (most are valid)
// Argument 3 = Pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)

int       setBrightness, stepBrightness;

enum neoPixColor {
    neoPixWhite
  , neoPixRed
  , neoPixGreen
  , neoPixBlue
};

volatile  bool receivedInterrupt = false;
uint32_t  WDcounter;
uint8_t   feedsReceived = 0;
uint32_t  glowTimer, blinkWdtLedTimer, blinkRstLedTimer;
uint32_t  lastHartbeatTimer = 0;
uint32_t  loopTimer, waitTimer = millis();


//----------------------------------------------------------------
void IRAM_ATTR interruptSR(void) 
{
  receivedInterrupt = true;
    
}   // interruptSR()

//----------------------------------------------------------------
bool handleInterrupt()
{
  if (receivedInterrupt)
  {
    Serial.println("Received Interrupt!");
    receivedInterrupt = false;
    lastHartbeatTimer = millis();
    return true;
  }
  return false;
  
} //  handleInterrupt()


//----------------------------------------------------------------
void neoPixOff(int neoPixNr) 
{
  neoPixel.setPixelColor(neoPixNr, 0, 0, 0);
  neoPixel.show();
  
} //  neoPixOff()


//----------------------------------------------------------------
void neoPixOn(int neoPixNr, neoPixColor color) 
{
  switch(color)
  {
     //--  Set pixel's color (in RAM)
    case neoPixRed:
            neoPixel.setPixelColor(neoPixNr, 255, 0, 0);
            break;
    case neoPixGreen:
            neoPixel.setPixelColor(neoPixNr, 0, 255, 0);
            break;
    case neoPixBlue:
            neoPixel.setPixelColor(neoPixNr, 0, 0, 255);
            break;
    default:  //-- neoPixWhite
            neoPixel.setPixelColor(neoPixNr, 255, 255, 255);
  }
  neoPixel.show();                      //  Update strip to match
  
} //  neoPixOn()


//----------------------------------------------------------------
void neoPixHeartBeat(int neoPixNr, int16_t heartRate, neoPixColor color=neoPixGreen) 
{
  int16_t moduloHeartRate = (millis() % heartRate);
  int16_t brightness      = abs(map(moduloHeartRate, 0, heartRate, -255, 255));
  
  switch(color)
  {
     //--  Set pixel's color (in RAM)
    case neoPixRed:
            neoPixel.setPixelColor(neoPixNr, brightness, 0, 0);
            break;
    case neoPixGreen:
            neoPixel.setPixelColor(neoPixNr, 0, brightness, 0);
            break;
    case neoPixBlue:
            neoPixel.setPixelColor(neoPixNr, 0, 0, brightness);
            break;
    default:  //-- neoPixWhite
            neoPixel.setPixelColor(neoPixNr, brightness, brightness,brightness);
  }
  neoPixel.show();                      //  Update strip to match

} //  neoPixHeartBeat()


//----------------------------------------------------------------
void setup() 
{
  Serial.begin(115200);
  while (!Serial) { delay(10); }
  Serial.println("\r\nAnd than it all starts ....\r\n");
  
  //  pinMode(_PIN_NEO_LED,  OUTPUT);
  neoPixel.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  neoPixel.show();            // Turn OFF all pixels ASAP
  neoPixel.setBrightness(NEO_BRIGHTNESS);

  Serial.printf("AttachInterrupt to GPIO%d..\r\n", _PIN_INTERRUPT);
  attachInterrupt(_PIN_INTERRUPT, interruptSR, CHANGE);
  
  Serial.printf("Start %d seconds start time\r\n", _STARTUP_TIME/1000);
  waitTimer = millis();
  while((millis() - waitTimer) < _STARTUP_TIME)
  {
    neoPixHeartBeat(0, 500, neoPixBlue);
  }
  Serial.println("Start Normal Operation ..");
  neoPixOff(0);
  receivedInterrupt = false;
  WDcounter         = 0;
  feedsReceived     = 0;
  lastHartbeatTimer = millis();

} // setup()


//----------------------------------------------------------------
void loop() 
{
  //neoPixHeartBeat(0, 1000, neoPixGreen);
  //neoPixHeartBeat(1, 2000);
  //neoPixHeartBeat(2, 2000, neoPixRed);
  //neoPixHeartBeat(4, 4000, neoPixBlue);
  //neoPixHeartBeat(6, 8000, neoPixWhite);
  
  handleInterrupt();
  loopTimer = (millis() - lastHartbeatTimer);
  
  if ((loopTimer > _LAST_WARNING) && (loopTimer < _MAX_NO_HARTBEAT))
  {
    //Serial.printf("Last warning ...! [%d > %d]\r\n", loopTimer,  _LAST_WARNING);
    neoPixHeartBeat(0, 500, neoPixRed); 
  }
  
  else if ((loopTimer > _FIRST_WARNING) && (loopTimer < _MAX_NO_HARTBEAT))
  {
    //Serial.printf("First warning ...[%d > %d]\r\n", loopTimer, _FIRST_WARNING);
    //Serial.flush();
    neoPixHeartBeat(0, 2000, neoPixRed);
  }

  else if (loopTimer < _MAX_NO_HARTBEAT)
  {
    neoPixHeartBeat(0, 2000, neoPixGreen);
    if (handleInterrupt()) return;
  }
       //-- no heartbeats for too long!
       //-- initiate reset sequence ..
  else if (loopTimer > _MAX_NO_HARTBEAT)
  {
    Serial.printf("No heartbeat received in %d mS\r\n", loopTimer);
    Serial.println("RED heartbeat for 5 seconds..");
    uint32_t waitTimer = millis();
    while((millis() - waitTimer) < 5000)
    {
      neoPixHeartBeat(0, 200, neoPixRed);
      if (handleInterrupt()) return;
    }
    digitalWrite(_PIN_ESP32_EN,  HIGH);
    neoPixOn(0, neoPixRed);
    Serial.println("DetachInterrupt..");
    detachInterrupt(_PIN_INTERRUPT);
    //-- reset ESP32 ------------------
    pinMode(_PIN_ESP32_EN, OUTPUT);
    digitalWrite(_PIN_ESP32_EN, HIGH);
    delay(500);
    digitalWrite(_PIN_ESP32_EN,  LOW);
    delay(500);
    digitalWrite(_PIN_ESP32_EN, HIGH);

    pinMode(_PIN_ESP32_EN, INPUT);
    //-- now give ESP time (60 seconds) to startup!
    Serial.println("ESP32 reset, now wait for 60 seconds..");
    waitTimer = millis();
    while((millis() - waitTimer) < 60000)
    {
      neoPixOn(0, neoPixRed);
      delay(250);
      neoPixOn(0, neoPixWhite);
      delay(250);
    }
  }
     
} // loop()

/* eof */
