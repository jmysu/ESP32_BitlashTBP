/**
 * @file _QEspNowSync.cpp
 * @author jimmy.su
 * @brief  QEspNow LED Sync
 *         Dependancy library:QuickEspNow, SimpleMap, Esp32Ticker
 * @version 0.1
 * @date 2023-01-06
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "_BitlashScript.h"
#include <Arduino.h>

#include <myTinyBasicPlus.h>
int iMyScriptType;

// #if defined(BOARDNAME)
//  #pragma message "Board:" BOARDNAME
// #endif

// PeerListClass myPeers;

// #define LED_ON  0
// #define LED_OFF 1
#define DEST_ADDR ESPNOW_BROADCAST_ADDRESS
#include "espnow/myEspNow.h" //struct && enum

bool isEspNowRXed = false; // flag for loop processing
bool _isRxInfo = false; // for RX debug
uint64_t myMac, macLeader;

//-----------------------------------------------------------------------------
void espSendHelloCB()
{
    quickEspNow.onDataSent(NULL); // No sent callback
    broadcastHello();
}

extern void prompt(void); // bitlash prompt
extern void setupSD();
extern void setupESPUI();

auto& usbSerial = Serial;
//=============================================================================
void setup()
{
    setupBoardPins();
    // Blink LED1 @1Hz
    // tickBlinkLED1.attach_ms(1000, toggleLED1);

#if (ARDUINO_USB_CDC_ON_BOOT)
#pragma message "USB_CDC_ON_BOOT=1"
    if (isUSBCDC()) {
        if (!Serial) {
            Serial.begin(115200);
        }
        // Workaround for when USB CDC is unplugged #7583
        Serial.setTxTimeoutMs(10); // <<<====== solves USBCDC issue of delay and ANSI serial corruption
        log_w("USB_CDC_ON_BOOT enabled!");
        Serial.println("USBCDC:Starting CDC message...");
    }
#else
#pragma message "USB_CDC_ON_BOOT=0"
    if (!Serial) {
        Serial.begin(115200);
    }
    log_w("USB_CDC_ON_BOOT disabled!");
    Serial.println("USB-Bridge:Starting Serial message...");
#endif

    // Check NVS settigns
    if (!isSettingExist())
        createSettingsDummy();
    listSettings();
    
    printESP32info();
//-------------------------------------------------------------------------
#if (ENABLE_WIFI_SNTP)
#pragma message "ENABLE_WIFI_SNTP"
    WiFi.disconnect(false, true);
    WiFi.mode(WIFI_AP_STA); // Set the device as a Station and Soft Access Point simultaneously
    // if (!WiFi.mode (WIFI_MODE_AP)) {
    //     Serial.println ("WiFi mode AP not supported");
    //     }
    delay(200);

    wifiScanHomeMobile(); // 100ms per channel scan
    if (_isHomeAPexist || _isMobileAPexist) { // Yes, we are at home
        Serial.print("WiFi AP: ");
        if (_isMobileAPexist)
            Serial.println("MobileAP");
        else
            Serial.println("HomeAP");

        // WiFi.mode(WIFI_AP_STA); // default AP_STA mode for OTA
        // setupWiFiHome();
        // if (WiFi.status() == WL_CONNECTED)

        setupOTA();     //8.8.4.4:8080
        #if (ENABLE_ESPUI)
         setupESPUI();   //8.8.4.4:80
        #endif
    } else
        WiFi.disconnect(true); // wifi-off
#else
#pragma message "!!! NO WiFi_SNTP !!!"
#endif

//-------------------------------------------------------------------------
#if (ENABLE_QESPNOW)
#pragma message "ENABLE_QESPNOW"
    // if (WiFi.SSID()) //STA mode
    //     Serial.printf("[WiFi SSID@CH] %s@%d\n", WiFi.SSID().c_str(), WiFi.channel());
    if (WiFi.softAPIP()) {
        Serial.printf("[WiFi SOFTAP@CH] %s@%d\n", WiFi.softAPSSID().c_str(), WiFi.channel());
        Serial.printf("     IP address: %s\n", WiFi.softAPIP().toString().c_str());
        Serial.printf("     MAC address: %s\n", WiFi.softAPmacAddress().c_str());
    }
    uint8_t mac[6];
    WiFi.macAddress(mac);
    Serial.printf("[WiFi MAC] " MACSTR "\n", MAC2STR(mac));
    uint32_t uOui = mac[0] << 16 | mac[1] << 8 | mac[2];
    uint32_t uId = mac[3] << 16 | mac[4] << 8 | mac[5];
    myMac = ((uint64_t)uOui) << 24 | uId;
    macLeader = myMac;
    // Serial.printf("uOui Id:%X:%X", uOui, uId);
    Serial.printf("[QEspNOW MAC] %llX\n", myMac);

    // Setup Nodes Maps---------------------
    setupNodesMap();
    _myNodesAlive->clear();
    _myNodesTime->put(myMac, millis());
    printNodesAlive();
    //=====================================
    // quickEspNow.onDataSent(dataSentCB);
    quickEspNow.onDataRcvd(dataReceivedCB);

#if (ENABLE_ESPNOW_LR)
#pragma message "ESPNOW_LR"
    /* Config LR Mode
       For LR-enabled AP of ESP32, it is incompatible with traditional 802.11 mode, because the beacon is sent in LR mode.
       For LR-enabled station of ESP32 whose mode is NOT LR-only mode, it is compatible with traditional 802.11 mode.
       If both station and AP are ESP32 devices and both of them have enabled LR mode, the negotiated mode supports LR.*/
    if (ESP_OK == esp_wifi_set_protocol(WIFI_IF_STA, WIFI_PROTOCOL_LR)) { // 11B/11G/11N/LR
        Serial.println("[802.11 LongRange]");
    }
#endif
    // quickEspNow.begin (); // Use no parameters to start ESP-NOW on same channel as WiFi, in STA mode
    // quickEspNow.begin (CURRENT_WIFI_CHANNEL, WIFI_IF_AP); // Same channel must be used for both AP and ESP-NOW
    if (_channelSTA_AP)
        quickEspNow.begin(_channelSTA_AP, WIFI_IF_STA); // Use known channel when available
    else
        quickEspNow.begin(1);

    tickSendHello.attach_ms(1000, espSendHelloCB); // Send HELLO every 1000ms
#endif

    //=========================================================
    showBanner();
    // delay(500);
    //  Bitlash start here!!!----------------------------------
    initBitlash();
    // addBitlashFunction("echo",  (bitlash_function) func_echo);
    addBitlashFunctionHint("tbp", (bitlash_function)func_tbp, "\tTinyBasicPlus V0.15j");
    addBitlashFunctionHint("neo", (bitlash_function)func_neo, "\tNeopixel RGB, neo(r,g,b)");
    addBitlashFunctionHint("temp", (bitlash_function)func_temp, "\tESP32C3 temperature sensor");
    addBitlashFunctionHint("play", (bitlash_function)func_play, "\tplay([\"up|down|ding|dong|over|pack|135|531|crazy|mario|titanic|pirate\"])");
    addBitlashFunctionHint("sleep", (bitlash_function)func_sleep, "\tDeepSleep seconds");
    addBitlashFunctionHint("settings", (bitlash_function)func_SETTINGS, "\tsettings([\"list|ssid|pass|ntp|ota|espnow|RESET\"])");
    addBitlashFunctionHint("cq", (bitlash_function)func_CQ, "\tcq([\"msg\"])");

#if ENABLE_WIFI_SNTP
    addBitlashFunctionHint("wifi", (bitlash_function)func_WiFi, "\twifi([\"scan|ssid|pass\"])");
#endif
#if ENABLE_QESPNOW
    addBitlashFunctionHint("now", (bitlash_function)func_Now, "\tnow([\"nodes|rxinfo|time|ping\"])");
    addBitlashFunctionHint("nowmsg", (bitlash_function)func_nowMsg, "\tnowmsg(<\"id\">, <\"msg\">)");
#endif

    speol(); // bitlash EOL
    //=========================================================
    if (_pinNeopixel) {
        neopixelRGB(0, 10, 0);
        neopixelSetupBlink(1000);
    }
    if (_pinBuzzer) {
        setupPwmBuzzer();
        Play_RampUp();
    }
    //==========================================================
    prompt(); // bitlash Prompt
    setupSD();

}

/*
 *   _EsoNowRx stores the    msgType;    //msgType: 0x10:time-report, 0x18:echo time-report
 *                           macAddr64;  //mac address for the sender
 *                           ums;        //time in milliseconds sent
 *                           rssi;       //RSSI sent
 *
 *    *_myNodesTime, *_myNodesAlive; *_myNodesRSSI;
 *      Three simpleMap nodes store MAC/time, Alive time and RSSI
 *
 *    The EspNow message is packed in MsgPack
 */
uint32_t lastStatusMs = 0;
unsigned long _lastPingMs = 0;
uint8_t counter = 0;
extern void loopESPUI();
void loop()
{

#if (ENABLE_QESPNOW)
    if (isSettingsEspNOW()) {
        // Check dataReceivedCB() got EspNow data?
        if (isEspNowRXed) { // Got _EspNowRx RX data?
            isEspNowRXed = false;
            switch (_EspNowRx.msgType) {
            case myEspNowMsgType::HELLO:
                // Check if there is new nodes not in list
                if (!_myNodesTime->has(_EspNowRx.macAddr64)) { // New memeber
                    _myNodesAlive->put(_EspNowRx.macAddr64, 1); // Add 1 to nodesAlive
                    _myNodesRSSI->put(_EspNowRx.macAddr64, _EspNowRx.rssi); // Add rssi to nodesRSSI
                    _myNodesTime->put(_EspNowRx.macAddr64, _EspNowRx.ums);
                    Serial.printf("\n[%6lu][Nodes] <%0llX> Added!", millis(), _EspNowRx.macAddr64);
                    // printNodesOld();
                    // printNodes();
                } else { // Old nodes, update time mark
                    _myNodesTime->put(_EspNowRx.macAddr64, _EspNowRx.ums);
                    // Average RSSI for old nodes
                    int8_t rssiOld = _myNodesRSSI->getData(_myNodesRSSI->getIndex(_EspNowRx.macAddr64));
                    // Serial.printf("<%0llX>RSSI:%d/%d\n",  EspNow.macAddr64, rssiOld, EspNow.rssi);
                    _myNodesRSSI->put(_EspNowRx.macAddr64, (_EspNowRx.rssi / 2 + rssiOld / 2));
                }
                // Update Leader's time mark here
                // Leader is the last one in _myNodesTime
                macLeader = _myNodesTime->getKey(_myNodesTime->size() - 1);
                if (_EspNowRx.macAddr64 == macLeader) { // Got Leader's HELLO
                    unsigned long ulSync = 1000 - (_EspNowRx.ums % 1000);
                    // tickBlinkLED21.once_ms(ulSync, blinkLED21_100ms); // Scheduling for next blink and try to sync w/ Leader
                    // log_d("[%lu]%lu", millis(), ulSync);
                    _EspNowLeaderUms = _EspNowRx.ums; // Update Leader's time
                    if (((_EspNowLeaderUms / 1000) % 6) == 0) // every 6 times
                        tickBlinkLED1.once_ms(ulSync, blinkLED1, (uint8_t)250);
                }
                break;
            case myEspNowMsgType::HELLO_ECHO_REQ: // Got an Hello echo request
                unicastHelloEcho(); // Echo the REQ, send back HELLO
                break;
            case myEspNowMsgType::HELLO_ECHO: // Echo acked! calculate RTTI
                // Serial.printf("myMac:%0llX peerMac:%0llX\n", myMac, _EspNowRx.macAddr64);
                if (myMac == _EspNowRx.macAddr64) {
                    uint32_t uRTTI = micros() - _EspNowRx.ums; //_EspNowRx.ums is the timestamp REQ fired!
                    Serial.printf("!!! ECHO RTTI=%luus from:%llX (%d)dBm\n", uRTTI, _EspNowRx.macAddr64, _EspNowRx.rssi);
                    // Serial.printf(" From mac:%02x:%02x:%02x:%02x:%02x:%02x\n", MAC2STR(EspNow.mac));
                }
                break;
            case myEspNowMsgType::PING: // Got Ping
                log_i("!PING! from [%0llX]", _EspNowRx.macAddr64);
                unicastPong(); // send pong to sender
                break;
            case myEspNowMsgType::PONG: // Got Pong, actions delay might affect dataReceivedCB()
                // log_d("!PONG! from [%0llX]", _EspNowRx.macAddr64);
                //Serial.printf("\n[%lu]\tPONG@%0X", millis(), (uint16_t)_EspNowRx.macAddr64); // chop to 16bit
                Serial.printf("\n[%lums]\tPONG@%0X% 02ddB", (_EspNowRx.ums - _lastPingMs), (uint16_t)_EspNowRx.macAddr64, _EspNowRx.rssi); // chop to 16bit
                break;
            case myEspNowMsgType::MSGPACK: // Got msgPack
            case myEspNowMsgType::MSGPACK_ECHO: // Got msgPack echo
                parseMessagePack(_EspNowMsgPackRx.pack());
                break;
            case myEspNowMsgType::MSGPACK_ECHO_REQ: // Peer request to send back msgPack
                unicastMsgPackEcho();
                break;
            case myEspNowMsgType::MSGPACK_CMD: // Got msgPack to Commander cmd
                // msgpackCommander(_EspNowMsgPackRx.pack());
                break;
            }
        } else { // Not isEspNowRXed
            // print Nodes & check KeepAlive every 10seconds
            if ((millis() - lastStatusMs) >= 10000) {
                lastStatusMs = millis(); // update lastStatus counter
                // printNodesOld();
                // printNodesAlive();
                // Check KeepAlive, drop node when timeDiff<1000ms
                for (int i = 0; i < _myNodesTime->size(); i++) {
                    uint64_t mac = _myNodesTime->getKey(i); // get key
                    uint32_t timeOld = _myNodesAlive->getData(_myNodesAlive->getIndex(mac)); // find key/value in alive nodes
                    uint32_t timeNew = _myNodesTime->getData(i);
                    uint32_t timeDiff = timeNew - timeOld; // time diff with last time mark

                    _myNodesAlive->put(mac, timeNew); // update NodesAlive w/ new time
                    // Serial.printf(" %0llX:%lu\n",  mac, timeDiff);
                    if (timeDiff < 1000) {
                        _myNodesTime->remove(i);
                        _myNodesAlive->remove(i);
                        Serial.printf("[Nodes] <%0llX> Dropped!\n", mac);
                        // TODO: Might need to check/update Leader's time when Leader changes
                    }
                }
                // printNodes();
                // printNodesRSSI();
            }
        }
    }
#endif
    if (SCRIPT_BITLASH == iMyScriptType) {
        runBitlash();
    } else if (SCRIPT_TINYBASICPLUS == iMyScriptType) {
        while (Serial.available()) {
            processTinyBasicPlus();
        }
    }
    
#if (ENABLE_ESPUI)
    loopESPUI();
#endif
}