#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino
#include "website_handling.h"
#include <NeoPixelBrightnessBus.h> // instead of NeoPixelBus.h

//needed for library
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>         //https://github.com/tzapu/WiFiManager

#include <ArduinoOTA.h>

const uint16_t pixelCount = 120;
const uint8_t pixelPin = 2;

// LED strip
NeoPixelBrightnessBus<NeoRgbwFeature, NeoEsp8266Uart800KbpsMethod> strip(pixelCount, pixelPin);

// Set web server port number to 80
ESP8266WebServer server(80);

// Variable to store the HTTP request
String header;

void set_leds(RgbwColor color){
    for(int i=0;i<pixelCount;i++){
        strip.SetPixelColor(i, color);
    }
    strip.Show();
}

void setup() {
    // put your setup code here, to run once:
    Serial.begin(115200);

    while (!Serial); // wait for serial attach

    // set all LEDs to the off state
    strip.Begin();
    strip.Show();

    // set the hostname
    wifi_station_set_hostname((char*) "ledstrip");

    // WiFiManager
    WiFiManager wifiManager;

    //fetches ssid and pass from eeprom and tries to connect
    //if it does not connect it starts an access point with the specified name
    //here  "AutoConnectAP"
    //and goes into a blocking loop awaiting configuration
    wifiManager.autoConnect("AutoConnectAP");
    //or use this for auto generated name ESP + ChipID
    //wifiManager.autoConnect();

    
    //if you get here you have connected to the WiFi
    Serial.println("connected...yeey :)");

    // configure the webserver
    configure_website_pages();

    ArduinoOTA.setPort(4133);
    ArduinoOTA.setPassword("admin");
    // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

    ArduinoOTA.onStart([]() {
        String type;
        if (ArduinoOTA.getCommand() == U_FLASH)
        type = "sketch";
        else // U_SPIFFS
        type = "filesystem";

        // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
        Serial.println("Start updating " + type);
    });
    ArduinoOTA.onEnd([]() {
        Serial.println("\nEnd");
    });
    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
        Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    });
    ArduinoOTA.onError([](ota_error_t error) {
        Serial.printf("Error[%u]: ", error);
        if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
        else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
        else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
        else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
        else if (error == OTA_END_ERROR) Serial.println("End Failed");
    });
    ArduinoOTA.begin();
}

void loop() {
    server.handleClient();
    ArduinoOTA.handle();
}