#ifndef MY_ESPNOW_H
#define MY_ESPNOW_H
#include <Arduino.h>
#include <esp_now.h>
#include <QuickEspNow.h>
#include <SimpleMap.h>

#include <MessagePack.h>
extern MessagePack _EspNowMsgPackRx;
extern uint32_t _EspNowLeaderUms;

enum myEspNowMsgType {
  HELLO       =0x10, // ID/millis    
  HELLO_ECHO_REQ,    // HELLO ECHO request, 掛號請求！
  HELLO_ECHO,        // HELLO ECHO back
  //HELLO_ECHO_ACK,  // HELLO ECHO ackowledge! 雙掛號！
  PING        =0x40,
  PONG,
  MSGPACK     =0x80, // MsgPack Data
  MSGPACK_ECHO_REQ,  // MsgPack ECHO request!
  MSGPACK_ECHO,      // MsgPack ECHO back
  MSGPACK_CMD,       // MsgPack CMD
   
  Reserved   // Reserved
};

struct espnow_struct {
    uint8_t   msgType;   
    uint64_t  macAddr64;
    //uint16_t  frames; // Not used! removed!
    uint32_t  ums;
    int8_t    rssi;
    //int8_t    dummy;
};



//extern SimpleList<uint32_t>* listID;
//extern void printListID();
//extern void setupListID();

extern uint64_t myMac; 
extern SimpleMap<uint64_t, uint32_t> *_myNodesTime, *_myNodesAlive;
extern SimpleMap<uint64_t, int8_t> *_myNodesRSSI;

extern void setupNodesMap();
extern void printNodes();
extern void printNodesOld();
extern void printNodesAlive();
extern void printNodesRSSI();
extern uint64_t getNodesMacFromID(uint16_t uID16);

extern int getNodes();
extern uint64_t getNodesMac(int i);
extern int8_t getNodesRSSI(int i);

extern void addNodesName(unsigned char*);
extern void printNodesName();
extern uint64_t getNodesMacFromName(unsigned char*);

//WiFi Promiscuous Sniff
//extern void setupWiFiSniffer();

extern void parseMessagePack(unsigned char * data);

//myEspNow
extern espnow_struct _EspNowRx;
extern bool isEspNowRXed;
extern bool _isRxInfo; 
extern uint64_t myMac, macLeader;

extern void dataSentCB (uint8_t* address,  uint8_t status );
extern void dataReceivedCB (uint8_t* address, uint8_t* data, uint8_t len, signed int rssi, bool broadcast); 
extern void broadcastHello();
extern void unicastHelloEchoReq(uint64_t mac);
extern void unicastHelloEcho();
extern void broadcastPing();
extern void unicastPong();
extern void unicastMsgPack(uint64_t mac, const char *msg);
extern void unicastMsgPackEchoReq(uint64_t mac, const char *msg);
extern void unicastMsgPackEcho();
extern void unicastMsgPackCmd(uint64_t mac, const char *cmd);

//_QEspNowCmdr
//extern void blinkLED20_5ms();  //For nodes
//extern void blinkLED21_500ms(); //For leader

//ESP32
extern float readTempSensor();

#endif