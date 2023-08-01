#include <Arduino.h>

#if ENABLE_WIFI_SNTP
 #include <WiFi.h>
 #include "_BitlashScript.h"

/*
 *  This sketch demonstrates how to scan WiFi networks.
 *  The API is almost the same as with the WiFi Shield library,
 *  the most obvious difference being the different file you need to include:
 */

 bool _isHomeAPexist=false;
 bool _isMobileAPexist=false;
 uint8_t _channelSTA_AP=0;
 String _foundSSID="";
void wifiScan()
{

    Serial.println("WiFi scan start...");
    // WiFi.scanNetworks will return the number of networks found
    int n = WiFi.scanNetworks(false, false, false, 100); 
    Serial.println("Scan done.");
    if (n == 0) {
        Serial.println("no networks found");
    } else {
        Serial.print(n);
        Serial.println(" networks found");
        for (int i = 0; i < n; ++i) {
            // Print SSID and RSSI for each network found
            Serial.print(i + 1);
            Serial.print(": ");
            Serial.print(WiFi.SSID(i));
            Serial.print(" rssi(");
            Serial.print(WiFi.RSSI(i));
            Serial.printf(") CH:%d Auth:", WiFi.channel());
            Serial.print((WiFi.encryptionType(i) == WIFI_AUTH_OPEN)?" ":"*");
            Serial.println(WiFi.encryptionType(i));
            //delay(10);
        }
    }
    Serial.println("");
}

/**
 * @brief wifiScanHome()
 * 
 *      scan the home AP, isJimmyAP/isBreezeAP will be changed after scan!
 *  
 */
void wifiScanHomeMobile()
{
    //scan channels 1-11, for BreezeHill or JimmyAP
    _isHomeAPexist=false;
    _isMobileAPexist=false;
    for (int iChannel=1;iChannel<12;iChannel++) {
        int n=WiFi.scanNetworks(false, false, false, 200, iChannel); //fast scan at channels
        if (n>0) {
            for (uint8_t i=0;i<n;i++) {
                String sSSID =  WiFi.SSID(i);
                int iRSSI = WiFi.RSSI(i);
                Serial.printf("%s(rssi:%d)@CH:%d\n", sSSID.c_str(), iRSSI, iChannel);
                //Looking for known AP
                if (sSSID.startsWith("Breeze")) {
                    log_w("Found KNOWN %s(rssi:%d)@CH:%d", sSSID.c_str(), iRSSI, iChannel);
                    _isHomeAPexist=true;
                    _channelSTA_AP= iChannel;
                    _foundSSID = sSSID;
                    break;
                    }
                if (sSSID.startsWith("Jimmy"))   {
                    log_w("Found KNOWN %s(rssi:%d)@CH:%d", sSSID.c_str(), iRSSI, iChannel);
                    _isMobileAPexist=true;
                    _channelSTA_AP= iChannel;
                    _foundSSID = sSSID;
                    break;
                    }
                }
            }
        if (_isHomeAPexist || _isMobileAPexist) break;    
        }
    log_d("");    
}
#endif