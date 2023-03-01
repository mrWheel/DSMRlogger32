/*
** espressif/arduino-esp32
** https://github.com/espressif/arduino-esp32/blob/master/libraries/WiFi/examples/WiFiClientEvents/WiFiClientEvents.ino
** 
*/

bool        lostWiFiConnection  = true;
bool        firstConnectionLost = true;
uint32_t    disconnectWiFiStart = 0;
static int  lostWiFiCount       = -1;
//----------------------------------------------------------------------------
void WiFiEvent(WiFiEvent_t event)
{
    Debugln();
    DebugTf("[WiFi-event] event: [%d] ", event);

    switch (event) 
    {
        case ARDUINO_EVENT_WIFI_READY: 
            Debugln("WiFi interface ready");
            break;
        case ARDUINO_EVENT_WIFI_SCAN_DONE:
            Debugln("Completed scan for access points");
            break;
        case ARDUINO_EVENT_WIFI_STA_START:
            Debugln("WiFi client started");
            break;
        case ARDUINO_EVENT_WIFI_STA_STOP:
            Debugln("WiFi clients stopped");
            break;
        case ARDUINO_EVENT_WIFI_STA_CONNECTED:
            Debugln("Connected to access point");
            if ((lostWiFiCount % 10) == 0)
            {
              writeToSysLog("Connected to AP successfully!");    
            }                    
            //-- wait for IP address - lostWiFiConnection = false;
            break;
        case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
            lostWiFiCount++;
            if (disconnectWiFiStart == 0) { disconnectWiFiStart = millis(); }
            if ((lostWiFiCount % 10) == 0)
            {
              Debugln("Disconnected from WiFi access point");
              writeToSysLog("Disconnected from WiFi access point"); 
            }                        
            lostWiFiConnection = true;
            break;
        case ARDUINO_EVENT_WIFI_STA_AUTHMODE_CHANGE:
            Debugln("Authentication mode of access point has changed");
            writeToSysLog("Authentication mode of access point has changed");                         
            lostWiFiConnection = true;
            break;
        case ARDUINO_EVENT_WIFI_STA_GOT_IP:
            Debug("Obtained IP address: ");
            Debugln(WiFi.localIP());
            writeToSysLog("Obtained IP address [%s]; reconnect took [%.1f] seconds"
                                              , WiFi.localIP().toString().c_str()
                                              , (float)((millis() - disconnectWiFiStart) / 1000.0));                         
            lostWiFiConnection  = false;
            firstConnectionLost = true;
            disconnectWiFiStart = 0;
            lostWiFiCount       = -1;
            break;
        case ARDUINO_EVENT_WIFI_STA_LOST_IP:
            Debugln("Lost IP address and IP address is reset to 0");
            writeToSysLog("Lost IP address and IP address is reset to 0");                         
            lostWiFiConnection = true;
            break;
        case ARDUINO_EVENT_WPS_ER_SUCCESS:
            Debugln("WiFi Protected Setup (WPS): succeeded in enrollee mode");
            break;
        case ARDUINO_EVENT_WPS_ER_FAILED:
            Debugln("WiFi Protected Setup (WPS): failed in enrollee mode");
            break;
        case ARDUINO_EVENT_WPS_ER_TIMEOUT:
            Debugln("WiFi Protected Setup (WPS): timeout in enrollee mode");
            break;
        case ARDUINO_EVENT_WPS_ER_PIN:
            Debugln("WiFi Protected Setup (WPS): pin code in enrollee mode");
            break;
        case ARDUINO_EVENT_WIFI_AP_START:
            Debugln("WiFi access point started");
            break;
        case ARDUINO_EVENT_WIFI_AP_STOP:
            Debugln("WiFi access point  stopped");
            break;
        case ARDUINO_EVENT_WIFI_AP_STACONNECTED:
            Debugln("Client connected");
            break;
        case ARDUINO_EVENT_WIFI_AP_STADISCONNECTED:
            Debugln("Client disconnected");
            break;
        case ARDUINO_EVENT_WIFI_AP_STAIPASSIGNED:
            Debugln("Assigned IP address to client");
            break;
        case ARDUINO_EVENT_WIFI_AP_PROBEREQRECVED:
            Debugln("Received probe request");
            break;
        case ARDUINO_EVENT_WIFI_AP_GOT_IP6:
            Debugln("AP IPv6 is preferred");
            break;
        case ARDUINO_EVENT_WIFI_STA_GOT_IP6:
            Debugln("STA IPv6 is preferred");
            break;
        case ARDUINO_EVENT_ETH_GOT_IP6:
            Debugln("Ethernet IPv6 is preferred");
            break;
        case ARDUINO_EVENT_ETH_START:
            Debugln("Ethernet started");
            break;
        case ARDUINO_EVENT_ETH_STOP:
            Debugln("Ethernet stopped");
            break;
        case ARDUINO_EVENT_ETH_CONNECTED:
            Debugln("Ethernet connected");
            break;
        case ARDUINO_EVENT_ETH_DISCONNECTED:
            Debugln("Ethernet disconnected");
            break;
        case ARDUINO_EVENT_ETH_GOT_IP:
            Debugln("Obtained IP address");
            break;
        default: 
            Debugln();
            break;
    } //  switch ..
    
} //  WiFiEvent()

/*eof*/
