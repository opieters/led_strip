// NeoPixelBrightness
// This example will cycle brightness from high to low of
// three pixels colored Red, Green, Blue.  
// This demonstrates the use of the NeoPixelBrightnessBus 
// with integrated brightness support
//
// There is serial output of the current state so you can 
// confirm and follow along
//

#include <NeoPixelBrightnessBus.h> // instead of NeoPixelBus.h
#include <ESP8266WiFi.h>
#include <ArduinoOTA.h>
#include <ESP8266WebServer.h>
#include <FS.h>
#include "website_handling.h"

const uint16_t PixelCount = 3; // this example assumes 3 pixels, making it smaller will cause a failure
const uint8_t PixelPin = 14;  // make sure to set this to the correct pin, ignored for Esp8266

#define colorSaturation 255 // saturation of color constants
RgbwColor red(colorSaturation, 0, 0, 0);
RgbwColor green(0, colorSaturation, 0, 0);
RgbwColor blue(0, 0, colorSaturation, 0);
RgbwColor white(0, 0, 0, colorSaturation);

const char* hostname = "ledstrip";
const char* ssid     = "yourssid";
const char* password = "yourpassword";

// Make sure to provide the correct color order feature
// for your NeoPixels
NeoPixelBrightnessBus<NeoRgbwFeature, Neo800KbpsMethod> strip(PixelCount, PixelPin);

ESP8266WebServer server(80);

// you loose the original color the lower the dim value used
// here due to quantization
const uint8_t c_MinBrightness = 8; 
const uint8_t c_MaxBrightness = 255;

int8_t direction; // current direction of dimming

void setup()
{
    Serial.begin(115200);
    while (!Serial); // wait for serial attach

    Serial.println();
    Serial.println("Initializing...");
    Serial.flush();

    // this resets all the neopixels to an off state
    strip.Begin();
    strip.Show();

    direction = -1; // default to dim first
    
    Serial.println();
    Serial.println("Running...");

    // set our three original colors
    strip.SetPixelColor(0, red);
    strip.SetPixelColor(1, green);
    strip.SetPixelColor(2, blue);

    // start WiFi
    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.hostname(hostname);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("Ready");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

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
    
    strip.Show();

    // set server pages
    configure_website_pages();
}

void loop()
{
    uint8_t brightness = strip.GetBrightness();
    Serial.println(brightness);
    
    delay(100);

    // swap diection of dim when limits are reached
    //
    if (direction < 0 && brightness <= c_MinBrightness)
    {
      direction = 1;
    }
    else if (direction > 0 && brightness >= c_MaxBrightness)
    {
      direction = -1;
    }
    // apply dimming
    brightness += direction;
    strip.SetBrightness(brightness);

    // show the results
    strip.Show();

    ArduinoOTA.handle();
}