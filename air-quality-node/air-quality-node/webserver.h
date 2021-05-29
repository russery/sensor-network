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

#ifndef __WEBSERVER_H
#define __WEBSERVER_H

#include "aqi-sensor.h"
#include <ESPAsyncWebServer.h>

class Webserver {
private:
  static constexpr char *ADDRESS_BASE PROGMEM = "sensor-";
  static constexpr uint ADDRESS_LEN_MAX PROGMEM = 128;
  static constexpr char *MDNS_FILE PROGMEM = "/cfg/MDNS.txt";
  AsyncWebServer server_;
  AQISensor *sensor_;

  String WebpageProcessor_(const String &var);
  void HandlePagePost_(AsyncWebServerRequest *request);

public:
  static constexpr char *DEFAULT_ADDRESS PROGMEM = "unnamed";
  char Address[Webserver::ADDRESS_LEN_MAX] = {};
  bool AddressChanged = false;

  explicit Webserver(AQISensor *sensor, uint port = 80)
      : server_(port), sensor_(sensor){};
  void Start(void);
  void StartMdns(void);
  bool IsMdnsActive(void);
  void Loop(void);
  void ChangeAddress(const char new_address[]);
};

#endif //__WEBSERVER_H