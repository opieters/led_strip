#include "website_handling.h"
#include <NeoPixelBrightnessBus.h> 

extern ESP8266WebServer server;

void set_leds(RgbwColor color);

void configure_website_pages(){
    server.serveStatic("/", SPIFFS, "/index.html");

    server.on("/", HTTP_POST, handleLEDs);
}


void handleLEDs(){
    if(!(server.hasArg("red") || server.hasArg("blue") || server.hasArg("green") || server.hasArg("white"))){
        server.send(400, "text/plain", "400: Invalid Request");
        return;
    }

    // parse LED arguments
    String red_s = server.arg("red"), green_s = server.arg("green"), blue_s = server.arg("blue"), white_s = server.arg("white");

    uint8_t red = red_s.toInt(), green = green_s.toInt(), blue = blue_s.toInt(), white = white_s.toInt();

    set_leds(RgbwColor(red, green, blue, white)); 
    
    // redirect to main page
    server.sendHeader("Location", String("/"), true);
    server.send( 302, "text/plain", "");
}