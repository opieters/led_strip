#include "website_handling.h"

extern ESP8266WebServer server;

void configure_website_pages(){
    server.serveStatic("/", SPIFFS, "/index.html");
    server.serveStatic("/", SPIFFS, "/index.html");

    server.on("/leds", HTTP_POST, handleLEDs);
}


void handleLEDs(){
    if(!(server.hasArg("red") || server.hasArg("blue") || server.hasArg("green") || server.hasArg("white"))){
        server.send(400, "text/plain", "400: Invalid Request");
        return;
    }
    
}