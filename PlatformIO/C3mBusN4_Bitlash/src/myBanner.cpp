#include <Arduino.h>
#include "_BitlashScript.h"
#include "ansi.h"

extern ANSI ansi;

 void showBanner() {
    while(!Serial){;}
       Serial.println();/*
       Serial.println("       ██╗██╗███╗   ███╗███╗   ███╗██╗   ██╗    ██████╗██████╗  █████╗ ███████╗████████╗");
       Serial.println("       ██║   ████╗ ████║████╗ ████║╚██╗ ██╔╝   ██╔════╝██╔══██╗██╔══██╗██╔════╝╚══██╔══╝");
       Serial.println("       ██║██║██╔████╔██║██╔████╔██║ ╚████╔╝    ██║     ██████╔╝███████║█████╗     ██║   ");
       Serial.println("  ██   ██║██║██║╚██╔╝██║██║╚██╔╝██║  ╚██╔╝     ██║     ██╔══██╗██╔══██║██╔══╝     ██║   ");
       Serial.println("  ╚█████╔╝██║██║ ╚═╝ ██║██║ ╚═╝ ██║   ██║      ╚██████╗██║  ██║██║  ██║██║        ██║   ");
       Serial.println("   ╚════╝ ╚═╝╚═╝     ╚═╝╚═╝     ╚═╝   ╚═╝       ╚═════╝╚═╝  ╚═╝╚═╝  ╚═╝╚═╝        ╚═╝   ");*/
       #if CONFIG_ARDUHAL_LOG_COLORS
        Serial.println( _grn"                             " _GRN" #####                             "     "   "     "####### #     # "); 
        Serial.println( _grn"     # # #    # #    # #   # " _GRN"#     # #####    ##   ###### ##### "     "   "     "   #    #     # "); 
        Serial.println( _grn"     #   ##  ## ##  ##  # #  " _GRN"#       #    #  #  #  #        #   "     "   "     "   #    #  #  # "); 
        Serial.println( _grn"     # # # ## # # ## #   #   " _GRN"#       #    # #    # #####    #   "     "   "     "   #    #  #  # "); 
        Serial.println( _grn"     # # #    # #    #   #   " _GRN"#       #####  ###### #        #   "     "   "     "   #    #  #  # "); 
        Serial.println( _grn"#    # # #    # #    #   #   " _GRN"#     # #   #  #    # #        #   " _grn"## " _GRN"   #    #  #  # "); 
        Serial.print  ( _grn" ####  # #    # #    #   #   " _GRN" #####  #    # #    # #        #   " _grn"## " _GRN"   #     ## ##  ");     
        Serial.print( _grn"  V:" _RED);Serial.print(FW_MAJOR);Serial.print(".");Serial.print(FW_MID);Serial.print(".");Serial.println(FW_MINOR);
        Serial.print(_WHT); 
        for (int i=0;i<100;i++) {
            ansi.color(ansi.white, ansi.grey2color(i));
            Serial.print("\u203E"); //overline
            }
         ansi.normal();Serial.println();
         ansi.color(ansi.black, ansi.white); //Reverse 
         Serial.printf("%s %s", BOARDNAME, DEVICEINFO); //print DeviceInfo w/ Reverse color
         #if (ENABLE_WIFI_SNTP)
          Serial.print("WiFi_SNTP ");
         #endif
         #if (ENABLE_OTA)
           //if (WiFi.status()==WL_CONNECTED) Serial.print("ElegantOTA<8.8.4.4> ");
           Serial.print("ElegantOTA<8.8.4.4>:8080 ");
         #endif
         #if (ENABLE_ESPUI)
            Serial.print("ESPUI<8.8.4.4>:80 ");
         #endif
         #if (ENABLE_QESPNOW)
          Serial.print("QEspNOW ");
         #endif
         ansi.normal();  
         Serial.println();
       #else
        Serial.println("                              #####                            "); 
        Serial.println("     # # #    # #    # #   # #     # #####    ##   ###### #####"); 
        Serial.println("     #   ##  ## ##  ##  # #  #       #    #  #  #  #        #  "); 
        Serial.println("     # # # ## # # ## #   #   #       #    # #    # #####    #  "); 
        Serial.println("     # # #    # #    #   #   #       #####  ###### #        #  "); 
        Serial.println("#    # # #    # #    #   #   #     # #   #  #    # #        #  "); 
        Serial.print  (" ####  # #    # #    #   #    #####  #    # #    # #        #  ");     
        Serial.print("V:");Serial.print(FW_MAJOR);Serial.print(".");Serial.print(FW_MID);Serial.print(".");Serial.println(FW_MINOR);
       #endif
 }