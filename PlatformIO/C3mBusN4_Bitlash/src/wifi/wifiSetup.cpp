#include <Arduino.h>
#include "_BitlashScript.h"

#include <WiFi.h>
#include <AsyncTCP.h>
 
#include <esp_wifi.h>
#include <LittleFS.h>

bool _isSTA_GotIP=false;
bool _isAPconnected=false;
bool _isAPclosed=false;


uint32_t ipClientAddress;

void APclientIPassigned(WiFiEvent_t event, WiFiEventInfo_t info)
{   
    //ledColor(1,0,0);
    ipClientAddress = info.wifi_ap_staipassigned.ip.addr; 
    log_d("Client ip:%s", IPAddress(ipClientAddress).toString().c_str());

    _isAPconnected=true;
    #if (PIN_C3Pico)||(PIN_C3mBUS0)||(PIN_C3mBusN4)
     Play_RampUp();
    #endif
}
void APclientConnected(WiFiEvent_t event, WiFiEventInfo_t info)
{   
    //ledColor(1,0,0);
    ipClientAddress = info.wifi_ap_staipassigned.ip.addr; 
    log_d("AP client:%s", IPAddress(ipClientAddress).toString().c_str());

    _isAPconnected=true;
    #if (PIN_C3Pico)||(PIN_C3mBUS0)||(PIN_C3mBusN4)
     neopixelRGB(15,0,15); //pink
     neopixelSetupBlink(1000); //60 breaths per minutes
     Play_135();
    #endif
}
void APclientLeave(WiFiEvent_t event, WiFiEventInfo_t info)
{    
    //ledColor(0,0,0);
    log_d("AP client leave.");

    #if (PIN_C3Pico)||(PIN_C3mBUS0)||(PIN_C3mBusN4)
     neopixelRGB(0,0,15);
     neopixelSetupBlink(3750); //16 breaths per minutes
     Play_531();
    #endif
    _isAPconnected=false;
}
void APstarted(WiFiEvent_t event, WiFiEventInfo_t info)
{
  _isAPclosed=false;
  //log_w("AP Started!");
 
}
void APclosed(WiFiEvent_t event, WiFiEventInfo_t info)
{
  _isAPclosed=true;
  //log_w("AP Closed!");
}
void STAgotIP(WiFiEvent_t event, WiFiEventInfo_t info)
{
  _isSTA_GotIP=true;
  log_w("Got STA IP");
  //After got STA IP, start AsyncFSWeb and wsTerminal
  //TODO: Make CapativePortal compatible
  //setupFSWeb();
  //setupWsTerminal();
  Play_DongDing();   //Conected && NTP Sync starts...     
} 

void setupWiFiEvents(){
  WiFi.onEvent(APclientIPassigned, WiFiEvent_t::ARDUINO_EVENT_WIFI_AP_STAIPASSIGNED);
  WiFi.onEvent(APclientConnected, WiFiEvent_t::ARDUINO_EVENT_WIFI_AP_STACONNECTED);
  WiFi.onEvent(APclientLeave, WiFiEvent_t::ARDUINO_EVENT_WIFI_AP_STADISCONNECTED);
  WiFi.onEvent(APstarted, WiFiEvent_t::ARDUINO_EVENT_WIFI_AP_START);
  WiFi.onEvent(APclosed, WiFiEvent_t::ARDUINO_EVENT_WIFI_AP_STOP);
  WiFi.onEvent(STAgotIP, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_GOT_IP); //STA_GOT_IP
}

/**
 * @brief setupWiFiHome
 *        setup WiFi for Home environment, connect AP only found my prefered AP!
 * 
 */
void setupWiFiHome()
{
    //setupWiFiEvents();
    //WiFi.begin("BreezeHill_50", "0932xxxxxx");

    String sMySSID = (char*)getSettingsSSID();
    if (_foundSSID.equalsIgnoreCase(sMySSID)) {
        log_w("Match mySettings AP:%s", _foundSSID.c_str());
        String sPASS = (char*)getSettingsPASS();
        String sPASSxxx = sPASS.substring(0,4)+"-XXX";
        log_w("SSID:PASS=%s:%s", _foundSSID.c_str(), sPASSxxx.c_str());
        if (_channelSTA_AP) {
            log_d("Connect to known AP:%s@CH:%d", _foundSSID.c_str(),  _channelSTA_AP);
            WiFi.begin(sMySSID.c_str(), sPASS.c_str(), _channelSTA_AP); //direct connect to known channel
            }
        else {
            log_d("Connecting AP:%s", _foundSSID.c_str());
            WiFi.begin(sMySSID.c_str(), sPASS.c_str());    
            }           
        int count=0;               
        while (WiFi.status()!=WL_CONNECTED) {
            Serial.print(">");
            delay(250);
            count++;
            if (count>20) break;
            } 
        //if (WiFi.status()==WL_CONNECTED) syncSNTP();
        if (isSettingsNTP()) syncSNTP();
        
        else Serial.println("WiFi NOT connected!"); 
        }        
    else
      Serial.printf("No matching mySettings AP(\"%s\")! Skip connect!", sMySSID.c_str());
}

 