#include "arduino_secrets.h"
#include "bsp.h"
#include "loop-timer.h"
#include <ArduinoOTA.h>
#include <ESPmDNS.h>
#include <WiFi.h>
#include <WiFiUdp.h>

extern const char WIFI_SSID[];
extern const char WIFI_PASSWORD[];
extern const char OTA_UPDATE_PWD[];

void setup() {
  pinMode(BSP::BUTTON_PIN, INPUT);
  pinMode(BSP::LED_PIN, OUTPUT);
  digitalWrite(BSP::LED_PIN, BSP::LED_OFF);

  Serial.begin(115200);
  Serial.println("Booting");
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }

  ArduinoOTA.setHostname("lightswitch");

  if (strlen(OTA_UPDATE_PWD) > 0)
    ArduinoOTA.setPassword(OTA_UPDATE_PWD);

  ArduinoOTA
      .onStart([]() {
        String type;
        if (ArduinoOTA.getCommand() == U_FLASH)
          type = "sketch";
        else // U_SPIFFS
          type = "filesystem";

        // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS
        // using SPIFFS.end()
        Serial.println("Start updating " + type);
      })
      .onEnd([]() { Serial.println("\nEnd"); })
      .onProgress([](unsigned int progress, unsigned int total) {
        Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
      })
      .onError([](ota_error_t error) {
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

  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  LoopTimer led_timer;

  while (true) {
    ArduinoOTA.handle();

    // Blink LED
    if (led_timer.CheckIntervalExceeded(1000)) {
      led_timer.Reset();
      digitalWrite(BSP::LED_PIN, !digitalRead(BSP::LED_PIN));
    }
  }
}