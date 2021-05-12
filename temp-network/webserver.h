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

#include "temp-sensor.h"
#include <ESPAsyncWebServer.h>

class Webserver {
public:
  static constexpr char *MDNS_ADDRESS PROGMEM = "tempmon";

  explicit Webserver(uint port = 80) : server_(port){};
  void Start(void);
  void StartMdns(void);
  bool IsMdnsActive(void);
  void Loop(void);

private:
  AsyncWebServer server_;
  // TempSensor sensor_;

  String WebpageProcessor_(const String &var);
};

#endif //__WEBSERVER_H