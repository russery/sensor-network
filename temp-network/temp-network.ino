/*
Implements a network of temperature sensors, viewable from the web.

Copyright (C) 2021  Robert Ussery

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#define ESP8266
#define HARDWARE_PROTO_ESP8266

#include "arduino_secrets.h"
#include "bsp.h"
#include "webserver.h"
#include <ArduinoOTA.h>
#if defined(ESP32)
#include <WiFi.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#endif

extern const char WIFI_SSID[];
extern const char WIFI_PASSWORD[];
extern const char OTA_UPDATE_PWD[];

TempSensor sensor;
Webserver webserver(&sensor);

void (*resetFunc)(void) = 0;

// cppcheck-suppress unusedFunction
void setup() {
  Serial.begin(115200);
  delay(100);
  Serial.print("\r\nTemp Network Starting...");

  pinMode(BSP::LED_PIN, OUTPUT);
  digitalWrite(BSP::LED_PIN, BSP::LED_OFF);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(250);
  }

  digitalWrite(BSP::LED_PIN, BSP::LED_ON);
  webserver.Start();
  webserver.StartMdns();

  ArduinoOTA.setHostname(Webserver::MDNS_ADDRESS);
  if (strlen(OTA_UPDATE_PWD) > 0)
    ArduinoOTA.setPassword(OTA_UPDATE_PWD);

  ArduinoOTA.onStart([]() { Serial.println("Start updating sketch"); });
  ArduinoOTA.onEnd([]() { Serial.println("\nFinished Updating"); });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR)
      Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR)
      Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR)
      Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR)
      Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR)
      Serial.println("End Failed");
  });
  ArduinoOTA.begin();

  sensor.Start();
}

// cppcheck-suppress unusedFunction
void loop() {
  static long last_temp_ms = millis();
  webserver.Loop();
  ArduinoOTA.handle();
  if (millis() - last_temp_ms > 1000) {
    last_temp_ms = millis();
    sensor.Loop();
  }
  yield(); // Make sure WiFi can do its thing.
}
