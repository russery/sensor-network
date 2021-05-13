/*
Implements web server to display temperature webpage.

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

#include "webserver.h"
#if defined(ESP32)
#include <AsyncTCP.h>
#include <WiFi.h>
#include <esp_spiffs.h>
#include <mdns.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ESPAsyncTCP.h>
#endif

namespace {
constexpr char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html><head>
  <title>Temp Monitor</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <meta http-equiv="refresh" content="10">
  <style>
    html {
      font-family: Helvetica;
      display: inline-block;
    }
    body {
      position: fixed;
      width: 300px;
      left: 50%%;
      transform: translateX(-50%%);
      border: 3px solid #222222;
      border-radius: 3%%;
      padding: 20px;
    }
    h2 {
      text-align: center;
      margin-top: 0;
    }
    .uptime {
      font-size: small;
    }
  </style>
</head>
<body>
  <h2>TEMP NETWORK</h2>
  <p>Temperature: %TEMP%</p>
  <p>Humidity:    %HUM%</p>
  <div class="uptime">%UPTIME%</div>
</body>
</html>
)rawliteral";
}

String Webserver::WebpageProcessor_(const String &var) {
  if (var == "TEMP") {
    char buff[16] = {0};
    sprintf(buff, "%4.1f&deg;F", sensor_->TemperatureFahrenheit);
    return String(buff);
  } else if (var == "HUM") {
    char buff[16] = {0};
    sprintf(buff, "%4.1f&percnt;", sensor_->HumidityPercent);
    return String(buff);
  } else if (var == "UPTIME") {
    char buff[128] = {0};
    unsigned long sec = millis() / 1000;
    sprintf(buff, "Uptime: %3d days %2d hrs %2d mins %2d sec\r\n",
            (uint)(sec / 86400L), (uint)(sec / 3600L) % 24,
            (uint)(sec / 60L) % 60, (uint)sec % 60);
    return String(buff);
  }
  return String();
}

void Webserver::Start(void) {
  server_.on("/", [&](AsyncWebServerRequest *request) {
    request->send_P(200, "text/html", index_html,
                    [&](const String &var) -> String {
                      return this->WebpageProcessor_(var);
                    });
  });
  server_.onNotFound([&](AsyncWebServerRequest *request) {
    request->send_P(200, "text/html", index_html,
                    [&](const String &var) -> String {
                      return this->WebpageProcessor_(var);
                    });
  }); // Just direct everything to the same page
  server_.begin();
}

void Webserver::StartMdns(void) {
#if defined(ESP32)
  mdns_init();
  mdns_hostname_set(MDNS_ADDRESS);
  mdns_service_add(NULL, "_http", "_tcp", 80, NULL, 0);
#elif defined(ESP8266)
  MDNS.begin(MDNS_ADDRESS);
  MDNS.addService("http", "tcp", 80);
#endif
}

void Webserver::Loop(void) {
#if defined(ESP8266)
  MDNS.update();
#endif
}
