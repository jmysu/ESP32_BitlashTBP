#include <Arduino.h>
#include "_BitlashScript.h"

#include <timeLib.h> 
#include <WiFi.h>
#include "esp_sntp.h"

long timezone = 8; //UTC+8
uint8_t daysavetime = 0;
bool _isNTPsynced=false;

void printLocalTime()
{
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
}

bool syncNTP(){
  //Serial.println("Contacting Time Server");
  log_d("Polling NTP...");  
  //configTime(3600*timezone, daysavetime*3600, "time.nist.gov", "0.pool.ntp.org", "1.pool.ntp.org");
  //configTime(3600*timezone, daysavetime*3600, "pool.ntp.org", "0.pool.ntp.org", "1.pool.ntp.org");
  configTime(3600*timezone, daysavetime*3600, "time.nist.gov");
  struct tm tmstruct ;
  tmstruct.tm_year = 0;
  if ( getLocalTime(&tmstruct, 5000) ) {
        //printLocalTime();
        setTime(mktime(&tmstruct));
        adjustTime(3600*timezone);
        log_w("Set Local Time:%02d:%02d:%02d", hour(), minute(), second());
        return true;
        }
  else    
      return false;    
}

void cbSyncSNTP(struct timeval *tv)
{
  struct tm tmstruct ;
  if ( getLocalTime(&tmstruct, 5000) ) {
        setTime(mktime(&tmstruct));
        adjustTime(3600*timezone);
        //log_i("---Time Sync---%02d:%02d:%02d", hour(), minute(), second());
        if (year()>2020) {
          if (sntp_get_sync_interval() <= 15000) { //one time only
            log_i("%04d-%02d-%02d %02d:%02d:%02d.%03ld", year(), month(), day(), hour(), minute(), second(), tv->tv_usec/1000L);
            sntp_set_sync_interval(300000); //5*60
            sntp_restart();
            }
          _isNTPsynced=true;  
          }
          
    }
  //Serial.println("\n----Time Sync-----");
  //Serial.println(tv->tv_sec);
  //Serial.println(ctime(&tv->tv_sec));
}
 
void syncSNTP()
{/*
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.println("Connecting...");
  }
   
  Serial.println("Connected with success");
 */
  sntp_set_time_sync_notification_cb(cbSyncSNTP);
 
  sntp_set_sync_interval(15000);
  configTime(3600*timezone, 3600, "time.google.com");
}