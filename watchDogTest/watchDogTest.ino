/*
 * WatchDog tester for ESP8266/ESP32
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

#define _PIN_WD_RESET    0  //-- GPIO00
#define _PIN_HEARTBEAT   4
#define _RESET_TIME     10000
#define _PULSE_TIME     1000

uint32_t  resetTimer, newDelay =1000;


//-------------------------------------------------------------------
void pulseHeart()
{
  static uint32_t pulseTimer;
  
  if ((millis()-pulseTimer) > _PULSE_TIME)
  {
    pulseTimer = millis();
    digitalWrite(_PIN_HEARTBEAT, !digitalRead(_PIN_HEARTBEAT));
  }
  
} //  pulseHeart()


//-------------------------------------------------------------------
void setup() 
{
  Serial.begin(115200);
  while(!Serial) { delay(10); }
  Serial.println("\r\nAnd than it begins ..\r\n");

  pinMode(_PIN_WD_RESET, OUTPUT);
  //-- Hold WatchDog
  Serial.println("Hold Watchdog ..");
  digitalWrite(_PIN_WD_RESET, LOW);

  Serial.println("Wait a bit ...");
  delay(5000);
  
  //-- Free WatchDog
  Serial.println("Free Watchdog ..");
  pinMode(_PIN_HEARTBEAT, OUTPUT);
  digitalWrite(_PIN_WD_RESET, HIGH);
  resetTimer = millis();

  Serial.println("Enter the loop() ..");
  Serial.flush();
  
} //  setup()


//-------------------------------------------------------------------
void loop() 
{
  pulseHeart();
  
  if ((millis() - resetTimer) > _RESET_TIME)
  {
    Serial.printf("\r\nTime [%d seconds] is up!\r\n", (_RESET_TIME / 1000));
    resetTimer = millis();
    Serial.println("Stoped sending HeartBeats ..");
    Serial.flush();
    while(1) { delay(100); }
  }

} //  loop()
