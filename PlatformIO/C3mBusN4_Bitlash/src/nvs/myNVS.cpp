/*
   Preferences (nvs) to store a structure.  
   Note that the maximum size of a putBytes is 496K
or 97% of the nvs partition size.  
*/ 
#include <Arduino.h>
#include "_BitlashScript.h" 

#include <Preferences.h>
Preferences prefs;

typedef struct {
  uint8_t cSSID[16];
  uint8_t cPASS[16];
  bool    bWiFiNTP;
  bool    bEspNOW;
  bool    bRsv1; //Reserved1
  //bool    bRsv2; //Reserved2
  //bool    bRsv3; //Reserved3
} MYSETTING_t;
MYSETTING_t _strucMySetting;

bool isSettingExist()
{
  prefs.begin("mySettings");  
  size_t prefsLen = prefs.getBytesLength("mySettings");
  prefs.end();
  return (prefsLen>0);
}
void createSettingsDummy()
{
  prefs.begin("mySettings");  // use "mySettings" namespace
  prefs.clear();              // clear opened preferences
  //             "123456789ABCDEF" 
  String sSSID = "SSIDSSID-------";
  String sPASS = "PassPass-------";
  memcpy(_strucMySetting.cSSID, sSSID.c_str(), sSSID.length());
  memcpy(_strucMySetting.cPASS, sPASS.c_str(), sPASS.length());
  _strucMySetting.bWiFiNTP= false;
  _strucMySetting.bEspNOW = false;
  prefs.putBytes("mySettings", &_strucMySetting, sizeof(MYSETTING_t));
  prefs.end();  
}

void listSettings()
{ char buffer[64];

  prefs.begin("mySettings"); // use "mySettings" namespace
  size_t prefsLen = prefs.getBytesLength("mySettings");
  prefs.getBytes("mySettings", buffer, prefsLen);
  if (prefsLen % sizeof(MYSETTING_t)) { // simple check that data fits
    log_e("SETTING Data is not correct size!");
    return;
  }
  //MYSETTING_t *data = (MYSETTING_t *) buffer; // cast the bytes into a struct ptr

  memcpy(&_strucMySetting, buffer, prefsLen);
  Serial.println("[mySettings]");
  Serial.printf("\tWiFiSSID=\"%s\"\n",  _strucMySetting.cSSID);
  Serial.printf("\tWiFiPASS=\"%s\"\n",  _strucMySetting.cPASS);
  Serial.printf("\tWiFiNTP =%d\n",      _strucMySetting.bWiFiNTP);
  Serial.printf("\tEspNow  =%d\n",      _strucMySetting.bEspNOW);
  prefs.end(); 
}

void putSettingsSSID(uint8_t* ptrSSID)
{
  prefs.begin("mySettings"); // use "mySettings" namespace
  size_t prefsLen = prefs.getBytesLength("mySettings");
  
  prefs.getBytes("mySettings", &_strucMySetting, prefsLen);
  if (prefsLen % sizeof(MYSETTING_t)) { // simple check that data fits
    log_e("SETTING Data is not correct size!");
    }
  sprintf((char*)_strucMySetting.cSSID, "%s", (char*)ptrSSID);
  prefs.putBytes("mySettings", &_strucMySetting, sizeof(MYSETTING_t));   
  prefs.end(); 
}
void putSettingsPASS(uint8_t* ptrPASS)
{
  prefs.begin("mySettings"); // use "mySettings" namespace
  size_t prefsLen = prefs.getBytesLength("mySettings");
  
  prefs.getBytes("mySettings", &_strucMySetting, prefsLen);
  if (prefsLen % sizeof(MYSETTING_t)) { // simple check that data fits
    log_e("Data is not correct size!");
    }
  sprintf((char*)_strucMySetting.cPASS, "%s", (char*)ptrPASS);
  prefs.putBytes("mySettings", &_strucMySetting, sizeof(MYSETTING_t));   
  prefs.end();  
}
void putSettingsNTP(bool isNTP)
{
  prefs.begin("mySettings"); // use "mySettings" namespace
  size_t prefsLen = prefs.getBytesLength("mySettings");
  
  prefs.getBytes("mySettings", &_strucMySetting, prefsLen);
  if (prefsLen % sizeof(MYSETTING_t)) { // simple check that data fits
    log_e("Data is not correct size!");
    }
  _strucMySetting.bWiFiNTP = isNTP;
  prefs.putBytes("mySettings", &_strucMySetting, sizeof(MYSETTING_t));   
  prefs.end();  
}
void putSettingsEspNOW(bool isNOW)
{
  prefs.begin("mySettings"); // use "mySettings" namespace
  size_t prefsLen = prefs.getBytesLength("mySettings");
  
  prefs.getBytes("mySettings", &_strucMySetting, prefsLen);
  if (prefsLen % sizeof(MYSETTING_t)) { // simple check that data fits
    log_e("Data is not correct size!");
    }
  _strucMySetting.bEspNOW = isNOW;
  prefs.putBytes("mySettings", &_strucMySetting, sizeof(MYSETTING_t));   
  prefs.end();  
}

//-----------------------------------------------------------------------------
uint8_t* getSettingsSSID()
{/*
 prefs.begin("mySettings"); // use "mySettings" namespace
 size_t prefsLen = prefs.getBytesLength("mySettings");
 prefs.getBytes("mySettings", &_strucMySetting, prefsLen);
 if (prefsLen % sizeof(MYSETTING_t)) { // simple check that data fits
    log_e("SETTING Data is not correct size!");
    }
 prefs.end();  
 */  
 return _strucMySetting.cSSID;
}
uint8_t* getSettingsPASS()
{/*
 prefs.begin("mySettings"); // use "mySettings" namespace
 size_t prefsLen = prefs.getBytesLength("mySettings");
 prefs.getBytes("mySettings", &_strucMySetting, prefsLen);
 if (prefsLen % sizeof(MYSETTING_t)) { // simple check that data fits
    log_e("SETTING Data is not correct size!");
    }
 prefs.end();    
 */
 return _strucMySetting.cPASS;
}
bool isSettingsNTP()
{/*
 prefs.begin("mySettings"); // use "mySettings" namespace
 size_t prefsLen = prefs.getBytesLength("mySettings");
 prefs.getBytes("mySettings", &_strucMySetting, prefsLen);
 if (prefsLen % sizeof(MYSETTING_t)) { // simple check that data fits
    log_e("SETTING Data is not correct size!");
    }
 prefs.end(); 
 */   
 return _strucMySetting.bWiFiNTP;
}
bool isSettingsEspNOW()
{/*
 prefs.begin("mySettings"); // use "mySettings" namespace
 size_t prefsLen = prefs.getBytesLength("mySettings");
 prefs.getBytes("mySettings", &_strucMySetting, prefsLen);
 if (prefsLen % sizeof(MYSETTING_t)) { // simple check that data fits
    log_e("SETTING Data is not correct size!");
    }
 prefs.end();    
 */
 return _strucMySetting.bEspNOW;
}
