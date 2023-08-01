#ifndef MYBITLASH_H
#define MYBITLASH_H
#include <Arduino.h>
#include <BitlashAPI.h>
#include "_BitlashScript.h"
#include "myTinyBasicPlus.h"
//-----------------------------------------------------------------------------
//Bitlash user function
//-----------------------------------------------------------------------------
numvar func_echo(void) {
    for (int i=1; i <= getarg(0); i++) {
        if (isstringarg(i)) sp((const char *) getstringarg(i));
        else printInteger(getarg(i), 0, ' ');
        speol();
    }
    return 0;
}

numvar func_tbp(void) {
    iMyScriptType = SCRIPT_TINYBASICPLUS; //for loop-processing
    setupTinyBasicPlus();
    processTinyBasicPlus();  //process some jobs after TinyBasicPlus init...
    return 0;
}

numvar func_neo() {
    neopixelRGB(getarg(1), getarg(2), getarg(3));
    log_i("chain neopixelRGB(%d,%d,%d)", getarg(1), getarg(2), getarg(3));
    return 0;
}

numvar func_temp() {
    setupTempSensor();
    log_w("ESP32C3:%5.1f°C", readTempSensor());  
    return 0;
}

numvar func_play() {
String sTune;
  if (isstringarg(1)){
    sTune = String((const char *)getstringarg(1));
    sTune.toLowerCase();
    if (sTune.startsWith("ding")) Play_DingDong();
    else if (sTune.startsWith("crazy")) Play_CrazyFrog();
    else if (sTune.startsWith("mario")) Play_MarioUW();
    else if (sTune.startsWith("titanic")) Play_Titanic();
    else if (sTune.startsWith("pirate")) Play_Pirates();
    else if (sTune.startsWith("135")) Play_135();
    else if (sTune.startsWith("531")) Play_531();
    else if (sTune.startsWith("up")) Play_RampUp();
    else if (sTune.startsWith("down")) Play_RampDown();
    else if (sTune.startsWith("over")) Play_MarioOver();
    else if (sTune.startsWith("pack")) Play_Pacman();
    }
  else {
    Play_DongDing(); //DingDong
    Serial.println("play([up|down|ding|dong|over|pack|135|531|crazy|mario|titanic|pirate])");
    }
  return 0;
}

#define S_To_uS_Factor 1000000ULL      //Conversion factor for micro seconds to seconds
numvar func_sleep() {
  int iTimerSeconds = (int)getarg(1);
  esp_sleep_enable_timer_wakeup(iTimerSeconds * S_To_uS_Factor);
  Serial.println("Going to sleep for " + String(iTimerSeconds) + " Seconds");
  delay(100);
  Serial.end();
  //Go to sleep now
  esp_deep_sleep_start();
  //=====================
  if (!Serial) Serial.begin(115200);
  Serial.println("I'am back!");

  return 0;
}

/*
  wifi, wifi(["scan|ntp]")
*/
numvar func_WiFi(){
  if (isstringarg(1)){
    String sCmd = String((const char *)getstringarg(1));
    sCmd.toLowerCase();
    if (sCmd.startsWith("scan")) 
      wifiScan();
    else if (sCmd.startsWith("ntp")) {
      bool isNTP = isSettingsNTP();
      if (isNTP) syncSNTP();
      Serial.printf("NTP is %s", (isNTP?"ON":"OFF"));
      }
    }
  else
    Serial.println("wifi(\"[scan|ntp]\")");

  return 0;
}

numvar func_Now(){
  if (isstringarg(1)){
    String sCmd = String((const char *)getstringarg(1));
    sCmd.toLowerCase();
    if (sCmd.startsWith("nodes")) {
      printNodes();
      printNodesRSSI();
      printNodesAlive();
      return _myNodesTime->size();
      }
    else if (sCmd.startsWith("rxinfo")) {
      _isRxInfo = (bool) getarg(2);
      Serial.printf("EspNOW RXinfo is %s", (_isRxInfo?"ON":"OFF"));
      }
    else if (sCmd.startsWith("time")) {
      //struct timeval tv;
      //gettimeofday(&tv, NULL); //retrieve original tv
      //tv.tv_sec = _EspNowLeaderUms/1000L; //get tv.sec from Leader time
      //tv.tv_usec= 0;                      //clear usec
      //settimeofday(&tv, NULL);            //set 
      //----------------------------------
      //Log synced time in HH:MM:SS.ZZZ
      //log_w("%02d:%02d:%02d.%03lu",hour(), minute(), second(), tv.tv_usec/1000L);
      unsigned long ulH,ulM,ulS,ulZ;
      ulH = (_EspNowLeaderUms / (1000*60*60)) %60;
      ulM = (_EspNowLeaderUms / (1000*60)) %60;
      ulS = (_EspNowLeaderUms / (1000)) %60;
      ulZ = _EspNowLeaderUms % 1000;
      Serial.printf("Nodes' time:%lums >>> %02lu:%02lu:%02lu.%03lu\n",_EspNowLeaderUms, ulH, ulM, ulS, ulZ);
      //Serial.printf("Nodes' diff:%ldms", millis()-_EspNowLeaderUms);
      }
    else if (sCmd.startsWith("ping")) {
      //#pragma message "TODO:ping"
      broadcastPing();
      _lastPingMs = millis(); //mark time for PONG response time
      }  
    }
  else {
    printNodes();
    Serial.println("now(\"[nodes|rxinfo|time|ping]\")");
    }
  return 0;
}

numvar func_SETTINGS(){
  if (!isSettingExist()) createSettingsDummy();

  if (isstringarg(1)){
    String sCmd = String((const char *)getstringarg(1));
    if (sCmd.startsWith("RESET")) { //NEED CAPTICAL RESET to initialize settings
      createSettingsDummy();
      listSettings();
      Serial.println("[mySettings] Initialized!!!\n");
      return 0;
      }

    sCmd.toLowerCase();
    if (sCmd.startsWith("list")) { //list Settings
      listSettings();
      }
    else if (sCmd.startsWith("ssid")) {
      if (isstringarg(2)) { //got ssid string
        String sSSID = String((const char *)getstringarg(2));
        putSettingsSSID((unsigned char*)sSSID.c_str());
        }
      Serial.printf("[mySettings] SSID: \"%s\"\n", getSettingsSSID());
      }
    else if (sCmd.startsWith("pass")) {
      if (isstringarg(2)) { //got pass string
        String sPASS = String((const char *)getstringarg(2));
        putSettingsPASS((unsigned char*)sPASS.c_str());
        }
      Serial.printf("[mySettings] PASS: \"%s\"\n", getSettingsPASS());
      }  
    else if (sCmd.startsWith("ntp")) {
      bool isNTP = (bool)getarg(2);
      putSettingsNTP(isNTP);
      Serial.printf("[mySettings] NTP:%d\n", isNTP);
      }  
    else if (sCmd.startsWith("espnow")) {
      bool isNOW = (bool)getarg(2);
      putSettingsEspNOW(isNOW);
      Serial.printf("[mySettings] EspNOW:%d\n", isNOW);
      }    
  
    }
  else
    Serial.println("settings(\"[list|ssid|pass|ntp|espnow|RESET]\")");

  return 0;
}

/* 
 *
 *
 * 
 * 
 */
numvar func_CQ(){
  if (isstringarg(1)){
    String sCQ = String((const char *)getstringarg(1));
    sendMorseCodes((char*)sCQ.c_str()); 
    }
  else
    sendMorseCodes();

  return 0;
}
/*
 *  nowmsg([*|id],"msg")
 *
 *  {"echohlo",     heloEchoHandler,  " echohlo <id>; request <id> send back an HELLO"},
 *  {"echomsg",     msgpackEchoHandler," echomsg <id> <msg>; request <id> send back msgPack"},
 *  {"msgcmd",      msgcmdHandler,    " msgcmd <id> <cmd>; send cmd to <id> w/ msgPack"},
 *
 */
numvar func_nowMsg() {
  String sID= String((const char *)getstringarg(1));
  uint16_t uID16 = strtol(sID.c_str(), 0, 16);
  //log_d("uAddr:%04X", uAddr);
  uint64_t uMAC = getNodesMacFromID(uID16);
  if (uMAC) { //Got uID from nodes
    if (isstringarg(2)){
      String sMsg= String((const char *)getstringarg(2));
      Serial.printf("Sending msg:\"%s\" to node:[%llX]\n",  sMsg.c_str(), uMAC);
      unicastMsgPack(uMAC, sMsg.c_str());
      }
    else
      Serial.println("nowmsg(<id>, <msg>)");
    }
  else 
    Serial.printf("node:[%04X] not found!\n", uID16);

  return 0;
}
#endif