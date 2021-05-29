/*
Implements a temperature/humidity sensor node with a
branding/configuration webpage and that broadcasts data over MQTT.

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
#include "aqi-sensor.h"
#include "arduino_secrets.h"
#include "bsp.h"
#include "webserver.h"
#include <ArduinoOTA.h>
#if defined(ESP32)
#include <WiFi.h>
#include <mdns.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#endif // ESP32 / ESP8266
#include <PubSubClient.h>

extern const char WIFI_SSID[];
extern const char WIFI_PASSWORD[];
extern const char OTA_UPDATE_PWD[];
extern const char MQTT_SERVER[];

AQISensor sensor;
Webserver webserver(&sensor);
WiFiClient wificlient;
PubSubClient mqttclient(wificlient);

void (*reset)(void) = 0;

unsigned long time_diff(unsigned long last_ms) {
  return abs((long long)millis() - (long long)last_ms);
}

// cppcheck-suppress unusedFunction
void setup() {
  Serial.begin(115200);
  delay(100);
  Serial.print("\r\nAQI Sensor Node Starting...");

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

  Serial.printf("\r\nQuery A: %s", MQTT_SERVER);

  struct ip4_addr addr;
  addr.addr = 0;

  esp_err_t err = mdns_query_a("printer", 2000, &addr);
  if (err) {
    if (err == ESP_ERR_NOT_FOUND) {
      printf("Host was not found!");
      return;
    }
    printf("Query Failed");
    return;
  }
  char addr_str[16];
  sprintf(addr_str, IPSTR, IP2STR(&addr));
  Serial.println(addr_str);

  sensor.Start();
  mqttclient.setServer(addr_str, 1883);
}

void loop() {
  static long last_sensor_ms = millis();
  if (!mqttclient.connected()) {
    mqttclient.connect(webserver.Address);
  } else {
    if (time_diff(last_sensor_ms) > 10000) {
      digitalWrite(BSP::LED_PIN, BSP::LED_ON);
      last_sensor_ms = millis();
      sensor.Loop();
      char topic[256] = {0};
      char value[16] = {0};
      sprintf(topic, "%s/aqi-pm2p5", webserver.Address);
      sprintf(value, "%4d", sensor.data.pm25_env);
      mqttclient.publish(topic, value);
      Serial.printf("\r\nPM2.5 %d\r\n", sensor.data.pm25_env);
    }
  }
  mqttclient.loop();

  static bool last_button_state = digitalRead(BSP::BUTTON_PIN);
  static long button_debounce_ms = millis();
  bool button_state = digitalRead(BSP::BUTTON_PIN);
  if (button_state != last_button_state) {
    button_debounce_ms = millis();
    last_button_state = button_state;
  } else if ((time_diff(button_debounce_ms) > 5000) &&
             (last_button_state == BSP::BUTTON_PRESSED)) {
    webserver.ChangeAddress(Webserver::DEFAULT_ADDRESS);
  }

  // Address has been changed, give a couple of seconds for webpage to be sent
  // and then reboot.
  static long address_timer_ms = 0;
  if (!webserver.AddressChanged)
    address_timer_ms = millis();
  if (time_diff(address_timer_ms) > 2000)
    reset();

  webserver.Loop();
  ArduinoOTA.handle();

  digitalWrite(BSP::LED_PIN, BSP::LED_OFF);

  yield(); // Make sure WiFi can do its thing.
}
