/*
Implements an AQI sensor node with a
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
#include "display.h"
#include "environment-sensor.h"
#include "loop-timer.h"
#include "webserver.h"
#include <ArduinoOTA.h>
#include <Wire.h>
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

Display display(&Wire);
EnvSensor envsensor(&Wire);
AQISensor aqi_sensor;
Webserver webserver(&aqi_sensor);
WiFiClient wificlient;
PubSubClient mqttclient(wificlient);

void (*reset)(void) = 0;

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

  // Get IP address of MQTT server:
  struct ip4_addr addr;
  addr.addr = 0;
  char server_copy[sizeof(MQTT_SERVER)];
  memcpy(server_copy, MQTT_SERVER, sizeof(MQTT_SERVER));
  // Get part of server name before TLD, assumes no subdomain:
  char *server_no_tld = strtok(server_copy, ".");
  esp_err_t err = mdns_query_a(server_no_tld, 2000, &addr);
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

  Wire.begin(BSP::I2C_SDA_PIN, BSP::I2C_SCL_PIN);
  display.Start();
  display.WriteText("Warming Up..........");

  envsensor.Start();
  // HACK: apply offset to temp sensor to account for self-heating of unit. This
  // doesn't fix humidity or pressure, but will make temperature agree better
  // with other sensors.
  envsensor.SetTempOffset(-6.0);

  aqi_sensor.Start();
  mqttclient.setServer(addr_str, 1883);
}

void loop() {
  aqi_sensor.Loop();
  envsensor.Loop();

  static LoopTimer mqtt_update_timer;
  if (!mqttclient.connected()) {
    Serial.println("\r\nNo MQTT connection... connecting");
    if (!mqttclient.connect(webserver.Address)) {
      Serial.print("\r\nConnection failed, State = ");
      Serial.print(mqttclient.state());
      Serial.print("\r\n");
      // HACK: Client reconnect never seems to work without a reboot, so just
      // reboot:
      reset();
    }
  } else if (mqtt_update_timer.CheckIntervalExceeded(5000)) {
    char topic[256] = {0};
    char value[16] = {0};
    if (!aqi_sensor.AreDataStale()) {
      display.Update(aqi_sensor.data.pm25_env);

      sprintf(topic, "%s/aqi-pm1p0", webserver.Address);
      sprintf(value, "%4d", aqi_sensor.data.pm10_env);
      mqttclient.publish(topic, value);
      sprintf(topic, "%s/aqi-pm2p5", webserver.Address);
      sprintf(value, "%4d", aqi_sensor.data.pm25_env);
      mqttclient.publish(topic, value);
      sprintf(topic, "%s/aqi-pm10", webserver.Address);
      sprintf(value, "%4d", aqi_sensor.data.pm100_env);
      mqttclient.publish(topic, value);
      sprintf(topic, "%s/aqi-particles_0p3um", webserver.Address);
      sprintf(value, "%4d", aqi_sensor.data.particles_03um);
      mqttclient.publish(topic, value);
      sprintf(topic, "%s/aqi-particles_0p5um", webserver.Address);
      sprintf(value, "%4d", aqi_sensor.data.particles_05um);
      mqttclient.publish(topic, value);
      sprintf(topic, "%s/aqi-particles_1p0um", webserver.Address);
      sprintf(value, "%4d", aqi_sensor.data.particles_10um);
      mqttclient.publish(topic, value);
      sprintf(topic, "%s/aqi-particles_2p5um", webserver.Address);
      sprintf(value, "%4d", aqi_sensor.data.particles_25um);
      mqttclient.publish(topic, value);
      sprintf(topic, "%s/aqi-particles_5p0um", webserver.Address);
      sprintf(value, "%4d", aqi_sensor.data.particles_50um);
      mqttclient.publish(topic, value);
      sprintf(topic, "%s/aqi-particles_10um", webserver.Address);
      sprintf(value, "%4d", aqi_sensor.data.particles_100um);
      mqttclient.publish(topic, value);
      Serial.printf(
          "---------------------------------------------------------\r\n");
      Serial.printf("PMS5003 Concentration Units (environmental)\r\n");
      Serial.printf("PM 1.0: %d\tPM 2.5: %d\tPM 10: \r\n",
                    aqi_sensor.data.pm10_env, aqi_sensor.data.pm25_env,
                    aqi_sensor.data.pm100_env);
    }
    if (envsensor.data.valid) {
      sprintf(topic, "%s/temperature", webserver.Address);
      sprintf(value, "%4.1f", envsensor.data.temperature_F);
      mqttclient.publish(topic, value);
      sprintf(topic, "%s/humidity", webserver.Address);
      sprintf(value, "%4.1f", envsensor.data.humidity_percent);
      mqttclient.publish(topic, value);
      sprintf(topic, "%s/pressure", webserver.Address);
      sprintf(value, "%6.0f", envsensor.data.pressure_Pa);
      mqttclient.publish(topic, value);
      Serial.printf(
          "---------------------------------------------------------\r\n");
      Serial.printf("BME280 Data:\r\n");
      Serial.printf(
          "Temp (ºF): %4.1f\tHumidity (%%): %2.0f\tPressure (Pa):%5.0f\r\n",
          envsensor.data.temperature_F, envsensor.data.humidity_percent,
          envsensor.data.pressure_Pa);
      Serial.printf(
          "---------------------------------------------------------\r\n");
    }
    mqtt_update_timer.Reset();
  }
  mqttclient.loop();

  // Handle button presses (used to reset mdns address to default):
  static bool last_button_state = digitalRead(BSP::BUTTON_PIN);
  static LoopTimer button_timer;
  bool button_state = digitalRead(BSP::BUTTON_PIN);
  if (button_state != last_button_state) {
    button_timer.Reset();
    last_button_state = button_state;
  } else if (button_timer.CheckIntervalExceeded(5000) &&
             (last_button_state == BSP::BUTTON_PRESSED)) {
    webserver.ChangeAddress(Webserver::DEFAULT_ADDRESS);
  }

  // Handle reboot if user changes mdns address from webpage:
  static LoopTimer address_change_timer;
  if (!webserver.AddressChanged)
    address_change_timer.Reset();
  if (address_change_timer.CheckIntervalExceeded(2000)) {
    // Address has been changed, give a couple of seconds for webpage to be sent
    // and then reboot.
    reset();
  }

  webserver.Loop();
  ArduinoOTA.handle();

  yield(); // Make sure WiFi can do its thing.
}
