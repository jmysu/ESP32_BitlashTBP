/*

=============================================================================
                       Buzzer:2  Neopixel:3
                        ┌──- C3mBusN4  ──┐
         LynxRx/AD0 | 01│●AN0       1PWM●│16 | IO1  | LynxTx
                    | 02│●RST       3INT●│15 | IO3  | Neopixel 
             IO7/10 | 03│●SS7/10     RX ●│14 | Rx20 
                      04│●SK4        TX ●│13 | Tx21 
       TouchRx6 IO6 | 05│●SO6       9SCL●│12 | IO9  | I2C SCL |BOOT
                      06│●SI5       8SDA■│11 | IO8  | I2C SDA 
                      07│●3v3        +5V■│10
                      08│●GND        GND■│09  
                        └────────────────┘                             
Booting Mode 
Pin   Default         SPI Boot      Download Boot
--------------------------------------------------
GPIO2 N/A               1           1
GPIO8 N/A               Don’t care  1
GPIO9 Internal-pull-up  1           0

*/
#ifndef _BitlashScript_H
#define _BitlashScript_H
#include <Arduino.h>
 


//Bitlash
#include <BitlashAPI.h>
#define SCRIPT_BITLASH 0
#define SCRIPT_TINYBASICPLUS 1

extern numvar func_echo();
extern numvar func_tbp();
extern numvar func_neo();
extern numvar func_temp();
extern numvar func_play();
extern numvar func_sleep();
extern numvar func_WiFi();
extern numvar func_Now();
extern numvar func_SETTINGS();
extern numvar func_CQ();
extern numvar func_nowMsg();

extern int iMyScriptType;

#include <TimeLib.h>
#include <Ticker.h>

#if (ENABLE_WIFI_SNTP)
 //#pragma message "ENABLE_WIFI_SNTP"
 #include <WiFi.h>
 #include <esp_wifi.h>
 extern void setupWiFiEvents();
 extern void setupWiFiHome();
 extern void wifiScan();
 extern void wifiScanHomeMobile();
 extern void setupOTA();
 extern bool _isHomeAPexist;
 extern bool _isMobileAPexist;
 extern uint8_t _channelSTA_AP;
 extern String _foundSSID;
#endif

//OTA NEED ENABLE_WIFI_SNTP
#if (ENABLE_OTA)
 
#endif

//QuickESPNOW NEED ENABLE_WIFI_SNTP
#if (ENABLE_QESPNOW)
 //#pragma message "ENABLE_QESPNOW"
 #include <QuickEspNow.h>
 #include "espnow/myEspNow.h"
 extern unsigned long _lastPingMs;
#endif


//Firmware Version Major.mid.minor
#define FW_MAJOR 0
#define FW_MID   9
#define FW_MINOR "0604"

#if CONFIG_ARDUHAL_LOG_COLORS
#define _BLK "\e[0;30m"
#define _red "\e[0;31m"
#define _RED "\e[1;31m"
#define _grn "\e[0;32m"
#define _GRN "\e[1;32m"
#define _YEL "\e[0;33m"
#define _blu "\e[0;34m"
#define _BLU "\e[1;34m"
#define _MAG "\e[0;35m"
#define _CYN "\e[0;36m"
#define _WHT "\e[0;37m"
#endif
extern void showBanner();
 
extern int8_t _LEDs; 
extern int8_t _pinLED[];
extern int8_t _LEDon[];

#include <Ticker.h>
extern Ticker tickSendHello;
extern Ticker tickBlinkLED1;
extern Ticker tickBlinkLED20;
extern Ticker tickBlinkLED21;

extern void toggleLED1();
extern void blinkLED20(uint8_t);
extern void blinkLED21(uint8_t);
extern void blinkLED1(uint8_t);

//NVS
#include <Preferences.h>

//extern int8_t _pinBlink;  
//extern bool _pinBlinkOn; 
extern int8_t _pinNeopixel;
extern int8_t _pinBuzzer; 
extern String _BoardName;
extern Preferences _myPref;

extern void putBoardNVS();
extern void getBoardNVS();
extern signed char getPinKeyValue(const char *key);
extern void putPinKeyValue(const char *key, signed char v);
extern void setupBoardPins();

extern void nvsPrintStates();
extern void nvsList();

//Settings
extern void createSettingsDummy();
extern void listSettings();
extern bool isSettingExist();
extern void putSettingsSSID(uint8_t*);
extern void putSettingsPASS(uint8_t*);
extern uint8_t* getSettingsSSID();
extern uint8_t* getSettingsPASS();
extern void putSettingsNTP(bool);
extern void putSettingsEspNOW(bool);
extern bool isSettingsNTP();
extern bool isSettingsEspNOW();


//Commander
//extern void setupCmdSerial();
//extern Commander cmd;
//extern void msgpackCommander(unsigned char * data);

//ESP32
extern bool isUSBCDC();
extern uint32_t getESP32ChipID24();
extern void neopixelRGB(uint8_t r, uint8_t g, uint8_t b);
extern void neopixelSetupBlink(int msBlink);
extern void printESP32info();
extern float readTempSensor();
extern void setupTempSensor();

//Neopixel
extern int _msBlinkInterval; 
extern int8_t _cNeopixelColors;
extern uint8_t _neoR, _neoG, _neoB;

extern void neopixelRGB(uint8_t r, uint8_t g, uint8_t b);
extern void neopixelSetupBlink(int msBlink);
extern void neopixelRotateColors(uint8_t c);
extern void neopixelBlink();
extern void neopixelOff();

//Buzzer
extern void setupPwmBuzzer();
extern void pwmBuzzer(int8_t duty, int freq, int durationMills);
extern void Play_DingDong();
extern void Play_DongDing();
extern void Play_Pacman();
extern void Play_CrazyFrog();
extern void Play_Titanic();
extern void Play_Pirates();
extern void Play_MarioUW();
extern void Play_MarioOver(); 
extern void Play_135();
extern void Play_531();
extern void Beep(uint8_t duty, int duration);
extern void Play_RampUp();
extern void Play_RampDown();
extern void sendMorseCodes();
extern void sendMorseCodes(char *msg);
extern void setupEspPWMBuzzer(int ms);

//FS
extern void setupListFS();
extern void LittleFSlistFS();
extern String readScriptFile(const char* path);

//WiFi ralated
extern void wifiScan();
extern void wifiScanHomeMobile();
extern void syncSNTP();

//Nodes related
extern void printNodes();
extern void printNodesAlive();
extern void printNodesRSSI();

extern void print_wakeup_reason();

#endif
