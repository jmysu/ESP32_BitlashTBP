
#include <Arduino.h>
#include <SimpleMap.h>
SimpleMap<uint64_t, uint32_t> *_myNodesTime, *_myNodesAlive;
SimpleMap<uint64_t, int8_t> *_myNodesRSSI;

#include "myEspNow.h"

void setupNodesMap()
{
 _myNodesTime = new SimpleMap<uint64_t, uint32_t>([](uint64_t &a, uint64_t &b) ->int {
        if (a == b) return 0;      // a and b are equal
        else if (a > b) return 1;  // a is bigger than b
        else return -1;            // a is smaller than b
    });
 _myNodesAlive = new SimpleMap<uint64_t, uint32_t>([](uint64_t &a, uint64_t &b) ->int {
        if (a == b) return 0;      // a and b are equal
        else if (a > b) return 1;  // a is bigger than b
        else return -1;            // a is smaller than b
    });
 _myNodesRSSI = new SimpleMap<uint64_t, int8_t>([](uint64_t &a, uint64_t &b) ->int {
        if (a == b) return 0;      // a and b are equal
        else if (a > b) return 1;  // a is bigger than b
        else return -1;            // a is smaller than b
    });   
 }

void printNodes()
{   /*
    Serial.printf("[%lu]\t===>%d[", millis(), _myNodesTime->size());
    for (int i=0;i<_myNodesTime->size();i++) {
        Serial.printf(" %0llX:%03lu",_myNodesTime->getKey(i), _myNodesTime->getData(i)/1000);
        }
    int32_t imsLeader= _myNodesTime->getData(_myNodesTime->size()-1)%1000;  
    Serial.println("]<===\n");
    //Serial.printf("]<=== %03lu(%03lu)\n", imsLeader, 1000-imsLeader);   
    */
    Serial.printf("[%6lu][Nodes     ]\t==>%d[", millis(), _myNodesTime->size());
    for (int i=0;i<_myNodesTime->size();i++) { //print macId:alive seconds
        uint64_t mac = _myNodesTime->getKey(i);
        char cMark = '-';
        if (mac == myMac) cMark='#'; 
        Serial.printf(" %c%04llX:%-6lu", cMark, mac&0x00FFFF, _myNodesTime->getData(i)/1000);
        }
    int32_t imsLeader= _myNodesTime->getData(_myNodesTime->size()-1)%1000;  
    Serial.printf("]<==\tSync@%03lu(%03lu)ms\n", imsLeader, 1000-imsLeader); //sync time   
/*
    //test Nodes functions
    Serial.printf("Total nodes:%d\n", getNodes());
    for (int i=0;i<getNodes();i++) {
        Serial.printf("<%llX>", getNodesMac(i));
        Serial.printf(":%d ", getNodesRSSI(i));
        }
    Serial.println();    
*/
}

int getNodes()
{
    return _myNodesTime->size();
}

uint64_t getNodesMac(int i) 
{
    if (i<_myNodesTime->size()){
        return _myNodesTime->getKey(i);
        }
    return 0;
}

int8_t getNodesRSSI(int i) 
{
    if (i<_myNodesRSSI->size()){
        return _myNodesRSSI->getData(i);
        }
    return 0;    
}

void printNodesOld()
{
    Serial.printf("[%6lu][NodesOld  ]\t--->%d(", millis(), _myNodesAlive->size());
    for (int i=0;i<_myNodesAlive->size();i++) {
        Serial.printf(" %0llX:%03lu",_myNodesAlive->getKey(i), _myNodesAlive->getData(i)/1000);
        }
    Serial.printf(")<---\n");    
}

/**
 * @brief print Nodes && KeepAlive time
 * 
 */
void printNodesAlive()
{
    Serial.printf("[%6lu][NodesAlive]\t>>>%d[", millis(), _myNodesTime->size());
    for (int i=0;i<_myNodesTime->size();i++) { //print macId:alive seconds
        uint64_t mac = _myNodesTime->getKey(i);
        char cMark = '-';
        if (mac == myMac) cMark='#'; 
        Serial.printf(" %c%04llX:%-6lu", cMark, mac&0x00FFFF, (_myNodesTime->getData(i)-_myNodesAlive->getData(i))/1000);
        }
    int32_t imsLeader= _myNodesTime->getData(_myNodesTime->size()-1)%1000;  
    Serial.printf("]<<<\tSync@%03lu(%03lu)ms\n", imsLeader, 1000-imsLeader); //sync time   
} 

void printNodesRSSI()
{
    Serial.printf("[%6lu][NodesRSSI ]\t>>>%d[", millis(), _myNodesTime->size());
    for (int i=0;i<_myNodesRSSI->size();i++) { //print macId:alive seconds
        uint64_t mac = _myNodesRSSI->getKey(i);

        char cBuf[16];
        sprintf(cBuf, "%-3ddB", _myNodesRSSI->getData(i));

        Serial.printf(" %c%04llX:%-6s", (mac==myMac)?'#':'-' , mac&0x00FFFF, (mac==myMac)?"0":cBuf );
        }
    int32_t imsLeader= _myNodesTime->getData(_myNodesTime->size()-1)%1000;  
    Serial.printf("]<<<\tSync@%03lu(%03lu)ms\n", imsLeader, 1000-imsLeader); //sync time
} 

uint64_t getNodesMacFromID(uint16_t uID16)
{
uint64_t uMac=0;
  
    for (int i=0;i<_myNodesTime->size();i++) {
        uint64_t mac = _myNodesTime->getKey(i);
        if ( (mac&0x0FFFF)==uID16 ) {
            uMac = mac;
            break;
            }
        }
    return uMac;    
}