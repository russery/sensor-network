/*
  ESP8266 mDNS responder sample

  This is an example of an HTTP server that is accessible
  via http://esp8266.local URL thanks to mDNS responder.

  Instructions:
  - Update WiFi SSID and password as necessary.
  - Flash the sketch to the ESP8266 board
  - Install host software:
    - For Linux, install Avahi (http://avahi.org/).
    - For Windows, install Bonjour (http://www.apple.com/support/bonjour/).
    - For Mac OSX and iOS support is built in through Bonjour already.
  - Point your browser to http://esp8266.local, you should see a response.

*/

#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiClient.h>

#ifndef STASSID
#define STASSID "I Love Lamp"
#define STAPSK  "bananabread"
#endif

const char* ssid = STASSID;
const char* password = STAPSK;


// TCP server at port 80 will respond to HTTP requests
WiFiServer server(80);

void setup(void) {
  Serial.begin(115200);

  // Connect to WiFi network
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Set up mDNS responder:
  // - first argument is the domain name, in this example
  //   the fully-qualified domain name is "esp8266.local"
  // - second argument is the IP address to advertise
  //   we send our IP address on the WiFi network
  // if (!MDNS.begin("esp8266")) {
  //   Serial.println("Error setting up MDNS responder!");
  //   while (1) {
  //     delay(1000);
  //   }
  // }
  // Serial.println("mDNS responder started");

  // // Start TCP (HTTP) server
  // server.begin();
  // Serial.println("TCP server started");

  // // Add service to MDNS-SD
  // MDNS.addService("http", "tcp", 80);
}

void loop(void) {
Serial.println("*fart*");
delay(1000);
}



// /*
// Implements a network of temperature sensors, viewable from the web.

// Copyright (C) 2021  Robert Ussery

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
// */


// #define ESP8266
// #define HARDWARE_PROTO_ESP8266

// #include "arduino_secrets.h"
// // #include "bsp.h"
// // #include "webserver.h"
// // #include <ArduinoOTA.h>
// #if defined(ESP32)
// #include <WiFi.h>
// #elif defined(ESP8266)
// #include <ESP8266WiFi.h>
// #endif


// //Webserver webserver;

// // void (*resetFunc)(void) = 0;

// // cppcheck-suppress unusedFunction
// void setup() {
//   Serial.begin(115200);
//     delay(100);
//   Serial.println("\r\nTemp Network Starting\r\n\n");

//   // pinMode(BSP::LED_PIN, OUTPUT);
//   // digitalWrite(BSP::LED_PIN, BSP::LED_OFF);
//   WiFi.mode(WIFI_STA);
//   WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
//   while (true){//WiFi.status() != WL_CONNECTED) {
//     Serial.print(".");
//     delay(250);
//   }


//   Serial.println("");
//   Serial.print("Connected to ");
//   Serial.println(WIFI_SSID);
//   Serial.print("IP address: ");
//   Serial.println(WiFi.localIP());

// // digitalWrite(BSP::LED_PIN, BSP::LED_ON);
//   // webserver.Start();
//   // webserver.StartMdns();

//   // ArduinoOTA.setHostname(Webserver::MDNS_ADDRESS);
//   // if (strlen(OTA_UPDATE_PWD) > 0)
//   //   ArduinoOTA.setPassword(OTA_UPDATE_PWD);

//   // ArduinoOTA.onStart([]() { Serial.println("Start updating sketch"); });
//   // ArduinoOTA.onEnd([]() { Serial.println("\nFinished Updating"); });
//   // ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
//   //   Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
//   // });
//   // ArduinoOTA.onError([](ota_error_t error) {
//   //   Serial.printf("Error[%u]: ", error);
//   //   if (error == OTA_AUTH_ERROR)
//   //     Serial.println("Auth Failed");
//   //   else if (error == OTA_BEGIN_ERROR)
//   //     Serial.println("Begin Failed");
//   //   else if (error == OTA_CONNECT_ERROR)
//   //     Serial.println("Connect Failed");
//   //   else if (error == OTA_RECEIVE_ERROR)
//   //     Serial.println("Receive Failed");
//   //   else if (error == OTA_END_ERROR)
//   //     Serial.println("End Failed");
//   // });
//   // ArduinoOTA.begin();
// }

// // cppcheck-suppress unusedFunction
// void loop() {
//   // webserver.Loop();
//   // ArduinoOTA.handle();
//   yield(); // Make sure WiFi can do its thing.
// }
