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
#include <esp_task_wdt.h>
#include <mdns.h>
#include <sys/stat.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ESPAsyncTCP.h>
#include <FS.h>
#endif // ESP32 / ESP8266

namespace {
constexpr char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html><head>
  <title>Sensor Node</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <meta http-equiv="refresh" content="30">
  <style>
    html {
      font-family: Helvetica;
      display: inline-block;
    }
    body {
      position: fixed;
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
  <h2>SENSOR NODE</h2>
  <p>PM2.5 &mu;g/m3: %PM25%</p>
  <div class="uptime">%UPTIME%</div></p>
  <form method="post" action="/post">
    <label for="name">Node Name:</label>
    <input type="text" id="name" name="name" maxlength=%NAME_MAX_LEN% value="%ADDRESS%">
    <input type="submit" value="Change">
  </form>
</body>
</html>
)rawliteral";
}

String Webserver::WebpageProcessor_(const String &var) {
  if (var == "PM25") {
    char buff[16] = {0};
    sprintf(buff, "%4d", sensor_->data.pm25_env);
    return String(buff);
  } else if (var == "UPTIME") {
    char buff[128] = {0};
    unsigned long sec = millis() / 1000;
    sprintf(buff, "Uptime: %3d days %2d hrs %2d mins %2d sec\r\n",
            (uint)(sec / 86400L), (uint)(sec / 3600L) % 24,
            (uint)(sec / 60L) % 60, (uint)sec % 60);
    return String(buff);
  } else if (var == "NAME_MAX_LEN") {
    return String(Webserver::ADDRESS_LEN_MAX);
  } else if (var == "ADDRESS") {
    return String(Address).substring(strlen(ADDRESS_BASE));
  }
  return String();
}

void Webserver::HandlePagePost_(AsyncWebServerRequest *request) {
  if (request->hasParam(F("name"), true)) {
    ChangeAddress(request->getParam(F("name"), true)->value().c_str());
    char redirect_link[512];
    sprintf(redirect_link,
            "<html><head><meta http-equiv=\"refresh\" content=\"10; "
            "URL=http://%s.local/\" /></head><body><a "
            "href=\"http://%s.local/\">http://%s.local/</a></"
            "body></html>",
            Address, Address, Address);
    request->send(200, "text/html", redirect_link);
  }
}

void Webserver::Start(void) {
#if defined(ESP32)
  // Start up flash filesystem and initialize config
  esp_vfs_spiffs_conf_t conf = {.base_path = "",
                                .partition_label = NULL,
                                .max_files = 5,
                                .format_if_mount_failed = true};
  // Change the task watchdog timer to 60s to allow plenty of time for SPIFFS
  // flash file system initialization if necessary
  esp_task_wdt_init(60, true);
  esp_vfs_spiffs_register(&conf);
  esp_task_wdt_init(1, true); // Change watchdog back to something reasonable
  struct stat st;
  if (stat(MDNS_FILE, &st) == 0) {
    FILE *mdns_file_handle = fopen(MDNS_FILE, "r");
    fgets(Address, Webserver::ADDRESS_LEN_MAX, mdns_file_handle);
    fclose(mdns_file_handle);
    // char* pos = strchr(Address, "\n"); // strip newline
    // if (pos) *pos = "\0";
    Serial.printf("\r\nUsing MDNS Address from flash: %s.local", Address);
  } else {
    memcpy(Address, Webserver::DEFAULT_ADDRESS,
           strlen(Webserver::DEFAULT_ADDRESS));
    Serial.printf("\r\nUsing MDNS Address: %s.local", Address);
  }
  mdns_init();
  mdns_hostname_set(Address);
  mdns_service_add(NULL, "_http", "_tcp", 80, NULL, 0);
#elif defined(ESP8266)
  SPIFFS.begin();
  if (SPIFFS.exists(MDNS_FILE)) {
    File mdns_file_handle = SPIFFS.open(MDNS_FILE, "r");
    String addr_string = mdns_file_handle.readString();
    mdns_file_handle.close();
    addr_string.toCharArray(
        Address, min(addr_string.length() + 1, Webserver::ADDRESS_LEN_MAX));
    Serial.printf("\r\nUsing MDNS Address from flash: %s.local", Address);
  } else {
    memcpy(Address, Webserver::DEFAULT_ADDRESS,
           strlen(Webserver::DEFAULT_ADDRESS));
    Serial.printf("\r\nUsing MDNS Address: %s.local", Address);
  }
  MDNS.begin(Address);
  MDNS.addService("http", "tcp", 80);
#endif
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
  server_.on("/post", HTTP_POST, [&](AsyncWebServerRequest *request) -> void {
    this->HandlePagePost_(request);
  });
  server_.begin();
}

void Webserver::Loop(void) {
#if defined(ESP8266)
  MDNS.update();
#endif
}

void Webserver::ChangeAddress(const char new_address[]) {
  memset(Address, 0, Webserver::ADDRESS_LEN_MAX);
  sprintf(Address, "%s%s", Webserver::ADDRESS_BASE, new_address);
#if defined(ESP32)
  unlink(MDNS_FILE);
  FILE *mdns_file_handle = fopen(MDNS_FILE, "w");
  fprintf(mdns_file_handle, Address);
  fclose(mdns_file_handle);
#elif defined(ESP8266)
  SPIFFS.remove(MDNS_FILE);
  File mdns_file_handle = SPIFFS.open(MDNS_FILE, "w");
  mdns_file_handle.print(Address);
  mdns_file_handle.close();
#endif
  AddressChanged = true;
}
