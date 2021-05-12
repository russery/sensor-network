/*
Includes pin definitions and other hardware-specific things.

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

#ifndef __BSP_H
#define __BSP_H

#include <Arduino.h>

class BSP {
public:
#if defined(ESP32)
  // Status LED and states:
  static constexpr char LED_PIN PROGMEM = 2;
  static constexpr char LED_OFF PROGMEM = LOW;
  static constexpr char LED_ON PROGMEM = HIGH;

#elif defined(ESP8266)
  // Status LED and states:
  static constexpr char LED_PIN PROGMEM = 16;
  static constexpr char LED_OFF PROGMEM = HIGH;
  static constexpr char LED_ON PROGMEM = LOW;
#endif // ESP32 / ESP8266

#if defined(HARDWARE_PROTO_ESP8266)
  static constexpr char DHT11_DATA_PIN = 1;
#endif // Hardware versions
};

#endif // __BSP_H