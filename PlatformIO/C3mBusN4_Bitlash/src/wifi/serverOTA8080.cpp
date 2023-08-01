/*
ElegantOTA w/ AsyncWebServer && LittleFS

[HTTP ERROR] Bad Request fix, add delay(1) in AsyncElegantOTA.h line88
https://github.com/ayushsharma82/AsyncElegantOTA/issues/49

To update the progress percentagle, add this to AsyncElegantOTA.h line 120
 if(len){
      if (Update.write(data, len) != len) {
          return request->send(400, "text/plain", "OTA could not begin");
          }
      else {
          static int last_percent = 0;
          int percent = (100 * Update.progress()) / Update.size();
          if (percent != last_percent) {
            last_percent = percent;
            Serial.printf("\r%02d%%", percent);
            }
          }    
      }
*/
#include <Arduino.h>
#include "_BitlashScript.h"

#pragma message "ELEGANT_OTA"

//#include <ESPmDNS.h>
//#include <DNSServer.h>
//#ifdef ESP32
 #include <WiFi.h>
 #include <AsyncTCP.h>
//#endif
#include "ESPAsyncWebServer.h"
#include <AsyncElegantOTA.h>
#include <esp_wifi.h>
#include <LittleFS.h>

IPAddress _APIP(8,8,4,4);
IPAddress _APSUBNET(255, 255, 255, 0);
//const uint8_t _DNS_PORT = 53;

//DNSServer dnsServer;
AsyncWebServer serverOTA(8080);

extern bool handleFileRead(String path, AsyncWebServerRequest *request);
/*
class CaptiveRequestHandler : public AsyncWebHandler {
public:
  CaptiveRequestHandler() {}
  virtual ~CaptiveRequestHandler() {}

  bool canHandle(AsyncWebServerRequest *request){
    //request->addInterestingHeader("ANY");
    return true;
  }

  void handleRequest(AsyncWebServerRequest *request) {
    AsyncResponseStream *response = request->beginResponseStream("text/html");
    response->printf("<!DOCTYPE html><html><head><title>%s Captive Portal</title></head><body>", MDNS.hostname(0).c_str());
    response->print("<p>This is captive portal front page.</p>");
    response->printf("<p>You were trying to reach: http://%s%s</p>", request->host().c_str(), request->url().c_str());
    //response->printf("<p>Try opening <a href='http://%s'>this link</a> instead</p>", WiFi.softAPIP().toString().c_str());
    response->printf("<p>Try opening http://%s instead</p>", WiFi.softAPIP().toString().c_str());
    
    response->print("</body></html>");
    request->send(response);
    log_w("Target:%s Redirect:%s", request->url().c_str(), WiFi.softAPIP().toString().c_str());
    
  }
}; */

String getContentType(String filename, AsyncWebServerRequest *request) {
    if (request->hasArg("download")) return "application/octet-stream";
    else if (filename.endsWith(".htm")) return "text/html";
    else if (filename.endsWith(".html")) return "text/html";
    else if (filename.endsWith(".css")) return "text/css";
    else if (filename.endsWith(".js")) return "application/javascript";
    else if (filename.endsWith(".json")) return "application/json";
    else if (filename.endsWith(".png")) return "image/png";
    else if (filename.endsWith(".gif")) return "image/gif";
    else if (filename.endsWith(".jpg")) return "image/jpeg";
    else if (filename.endsWith(".ico")) return "image/x-icon";
    else if (filename.endsWith(".xml")) return "text/xml";
    else if (filename.endsWith(".pdf")) return "application/x-pdf";
    else if (filename.endsWith(".zip")) return "application/x-zip";
    else if (filename.endsWith(".gz")) return "application/x-gzip";
    return "text/plain";
}
bool handleFileRead(String path, AsyncWebServerRequest *request) {
    log_d("handleFileRead: %s", path.c_str());
    if (path.endsWith("/")) path += "index.html";
    String contentType = getContentType(path, request);
    log_d("ContentType: %s", contentType.c_str());
    String pathWithGz = path + ".gz";
    log_d("Content type: %s", contentType.c_str());
        
    File file;
    AsyncWebServerResponse *response = request->beginResponse(200);
    if (LittleFS.exists(path)) {
          file = LittleFS.open(path, "r");
          log_d("LittleFS %s exist", path.c_str());
      response = request->beginResponse(LittleFS, path, contentType); 
      }
    //else if (SD.exists(path))  {
    //  file = SD.open(path, "r");
    //      log_d("SD %s exist", path.c_str());
    //  response = request->beginResponse(SD, path, contentType); 
    //  }

    if (path.endsWith(".gz"))
            response->addHeader("Content-Encoding", "gzip");
        request->send(response);
        log_d("File %s Sent", path.c_str());

        return true;
}

void setupOTA(){

  //Get 32bit chip id from reversed MAC address
  uint32_t ChipID32 = getESP32ChipID24();
  char cAP[32];
  //String sOTAname=getBoardName()+"-%04X";
  String sOTAname=String(BOARDNAME)+"-%04X";
  
  sprintf(cAP, sOTAname.c_str(), (uint16_t)ChipID32);    //for 4 nibbles 
  WiFi.softAP(cAP);
  WiFi.softAPConfig(_APIP, _APIP, _APSUBNET);
/*
  //DNS && mDNS
  dnsServer.start(_DNS_PORT, "*", WiFi.softAPIP());
  //if (!MDNS.begin(cAP)) {
  if (!MDNS.begin("c3mbus")) {
     Serial.println("Error starting mDNS");
     return;
     }
  // Add service to MDNS-SD
  MDNS.addService("http", "tcp", 80);
*/
/*
  if(!LittleFS.begin()){ 
    log_e("ERROR: LittleFS Mount Failed!");
    Play_MarioOver();
    return;
    }
  Serial.printf("OTA: LittleFS Used/Total:%lu/%lu KB\n", LittleFS.usedBytes()/1024L, LittleFS.totalBytes()/1024L);
*/
  //For SD file serving
  //server.serveStatic("/sd", SD, "/").setDefaultFile("index.htm");  

  // !!!The web handlers order matter!!!
  // AsyncWeb Request Life Cycle
  //  -TCP connection is received by the server
  //  -The connection is wrapped inside Request object
  //  -When the request head is received (type, url, get params, http version and host), 
  //   the server goes through all Rewrites (in the order they were added) 
  //   to rewrite the url and inject query parameters, next, it goes through all attached Handlers(in the order they were added)
  //   trying to find one that canHandle the given request. If none are found, the default(catch-all) handler is attached.

  //!!!Important for LittleFS folder serving!!!
  //serverOTA.serveStatic("/favicon.ico", LittleFS, "/favicon.ico"); //file not found fix
  //serverOTA.serveStatic("/", LittleFS, "/");  //Enable this will make "file not exist, no permits to create" issue!~~
 
  //Redirect to /update when /
  serverOTA.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->redirect("/update"); //to AsyncElegantOTA
    });

  //!!! Important for captive portal !!!
  // redirect any page like "hotspot-detect.html" "canonical.html" "msftconnecttest.com" "detectportal.firefox.com" "generate_204" 
  // to /update.html
  serverOTA.onNotFound([](AsyncWebServerRequest *request){
    log_w("Notfound: %s", request->url().c_str());       //This gives some insight into whatever was being requested
    request->redirect("/update"); //Redirect all unknown to /update
    });  
  


  //Required
  //serverOTA.on("/connecttest.txt",[](AsyncWebServerRequest *request){request->redirect("http://logout.net");}); //windows 11 captive portal workaround
  //Probably not all are Required, but some are. Others might speed things up?
  //serverOTA.on("http://captive.apple.com/hotspot-detect.html",[](AsyncWebServerRequest *request){request->redirect("/update");}); //firefox captive portal call home
  //serverOTA.on("/canonical.html",[](AsyncWebServerRequest *request){request->redirect("/update");}); //firefox captive portal call home
  //serverOTA.on("/chrome-variations/seed",[](AsyncWebServerRequest *request){request->send(200);}); //chrome captive portal call home
  //serverOTA.on("/redirect",[](AsyncWebServerRequest *request){request->redirect("/");}); //microsoft redirect
  //serverOTA.on("/success.txt",[](AsyncWebServerRequest *request){request->send(200);}); //firefox captive portal call home

  //---------------------------------------------------------------------------  
  //called when the url is not defined here
  //use it to load content from SPIFFS or SD
  /*
  server.onNotFound([](AsyncWebServerRequest *request) {
        log_e("Not found: %s\r\n", request->url().c_str());
        //if (!this->checkAuth(request))
        //  return request->requestAuthentication();
        AsyncWebServerResponse *response = request->beginResponse(200);
        response->addHeader("Connection", "close");
        response->addHeader("Access-Control-Allow-Origin", "*");
        if (!handleFileRead(request->url(), request))
            request->send(404, "text/plain", "FileNotFound");
        delete response; // Free up memory!
    });
  */
  //---------------------------------------------------------------------------
  //String user= "c3pico";
  String user= "c3mBus";
  String pass= "0806449";
  AsyncElegantOTA.setID(cAP);
  AsyncElegantOTA.begin(&serverOTA, user.c_str(), pass.c_str());    // Start ElegantOTA, login with c3pico, password
  
  //serverOTA.addHandler(new CaptiveRequestHandler()).setFilter(ON_AP_FILTER);//only when requested from AP
  serverOTA.begin();

  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);
  Serial.printf("serverOTA:8080 %s started! <%s:%s>\n", cAP, user.c_str() , pass.c_str()  );
  log_d("");
}

//void loopCaptivePortalDNS(){
//#if ENABLE_WIFI_AP_OTA
//  if (!_isAPconnected) return; //OTA client not connected!
//
//  dnsServer.processNextRequest();  
//#endif  
//}

