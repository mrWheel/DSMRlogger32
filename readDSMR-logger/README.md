# readDSMR-logger

Dit is een raamwerk om met verschillende MCU's de DSMRlogger32 uit te lezen en
de gegevens verder te verwerken.

Per board-type moet je één van onderstaande #define's actief maken door de twee 
slashes (//) ervoor weg te halen.
<pre>
//======= define type of board ===========
//==== only define one (1) board type ====
// #define _IS_ARDUINO_MEGA
// #define _IS_ESP8266
// #define _IS_ESP32
</pre>

### "myCredentials_org.h" 
Kopieer deze tab naar "myCredentials.h"

### In de tab "myCredentials.h" 
Define het IP address van jouw DSMR-logger:
<pre>
#define _DSMR_IP_ADDRESS    "IP_ADDRESS_OF_YOUR_DSMR_LOGGER"
</pre>

Als je een bordje met WiFi gebruikt voer dan hier de WiFi credentials van jouw AccessPoint in:
<pre>
#define _WIFI_SSID          "YOUR_WIFI_SSID"
#define _WIFI_PASSWRD       "YOUR_WIFI_PASSWRD"
</pre>

Dit programma vergt het maximale van een Arduino UNO met Ethernet shield!

Meer informatie over de DSMRlogger32 kun je 
<a href="https://willem.aandewiel.nl/index.php/2022/11/15/crisis-what-crisis/">hier</a>
vinden.

