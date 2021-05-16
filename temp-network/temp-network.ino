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
#include "arduino_secrets.h"
#include "bsp.h"
#include "temp-sensor.h"
#include "webserver.h"
#include <ArduinoOTA.h>
#include <ESP8266WiFi.h>
#include <FS.h>
#include <PubSubClient.h>

extern const char WIFI_SSID[];
extern const char WIFI_PASSWORD[];
extern const char OTA_UPDATE_PWD[];

TempSensor sensor;
Webserver webserver(&sensor);
WiFiClient wificlient;
PubSubClient mqttclient(wificlient);

void (*reset)(void) = 0;

// cppcheck-suppress unusedFunction
void setup() {
  Serial.begin(115200);
  delay(100);
  Serial.print("\r\nTemp Network Starting...");

  pinMode(BSP::BUTTON_PIN, INPUT);
  pinMode(BSP::LED_PIN, OUTPUT);
  digitalWrite(BSP::LED_PIN, BSP::LED_OFF);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(250);
  }

  webserver.Start();

  ArduinoOTA.setHostname(webserver.Address);
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
  mqttclient.setServer("printer.local", 1883);
}

void loop() {
  static long last_temp_ms = millis();
  static long address_timer_ms = 0;
  static bool last_button_state = digitalRead(BSP::BUTTON_PIN);
  static long button_debounce_ms = millis();

  if (mqttclient.connected()) {
    if (millis() - last_temp_ms > 1000) {
      digitalWrite(BSP::LED_PIN, BSP::LED_ON);
      last_temp_ms = millis();
      sensor.Loop();
      char topic[256] = {0};
      char value[16] = {0};
      sprintf(topic, "%s/temperature", webserver.Address);
      sprintf(value, "%4.1f", sensor.TemperatureFahrenheit);
      mqttclient.publish(topic, value);
      sprintf(topic, "%s/humidity", webserver.Address);
      sprintf(value, "%4.1f", sensor.HumidityPercent);
      mqttclient.publish(topic, value);
    }
    mqttclient.loop();
  } else {
    mqttclient.connect(webserver.Address);
  }

  bool button_state = digitalRead(BSP::BUTTON_PIN);
  if (button_state != last_button_state) {
    button_debounce_ms = millis();
    last_button_state = button_state;
  } else if ((millis() - button_debounce_ms > 5000) &&
             (last_button_state == BSP::BUTTON_PRESSED)) {
    webserver.ChangeAddress(Webserver::DEFAULT_ADDRESS);
  }

  // Address has been changed, give a couple of seconds for webpage to be sent
  // and then reboot.
  if (!webserver.AddressChanged)
    address_timer_ms = millis();
  if (millis() - address_timer_ms > 2000)
    reset();

  webserver.Loop();
  ArduinoOTA.handle();

  digitalWrite(BSP::LED_PIN, BSP::LED_OFF);

  yield(); // Make sure WiFi can do its thing.
}
