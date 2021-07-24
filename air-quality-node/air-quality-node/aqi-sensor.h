/*
AQI sensor interface, based on the Adafruit library but more robust.

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

#ifndef __TEMP_SENSOR_H
#define __TEMP_SENSOR_H

#include "bsp.h"
#include "loop-timer.h"

class AQISensor {
  typedef struct __attribute__((packed)) {
    uint16_t pm10_standard,  ///< Standard PM1.0
        pm25_standard,       ///< Standard PM2.5
        pm100_standard;      ///< Standard PM10.0
    uint16_t pm10_env,       ///< Environmental PM1.0
        pm25_env,            ///< Environmental PM2.5
        pm100_env;           ///< Environmental PM10.0
    uint16_t particles_03um, ///< 0.3um Particle Count
        particles_05um,      ///< 0.5um Particle Count
        particles_10um,      ///< 1.0um Particle Count
        particles_25um,      ///< 2.5um Particle Count
        particles_50um,      ///< 5.0um Particle Count
        particles_100um;     ///< 10.0um Particle Count
  } SensorData_t;

public:
  void Start(void);
  void Loop(void);
  bool IsDataStale(unsigned long stale_ms = 60 * 1000);
  SensorData_t data = {0};

private:
  LoopTimer stale_timer_;
  bool data_valid_ = false;

  // PMS5003 protocol definition:
  static constexpr uint8_t HEADER_BYTE1 PROGMEM = 0x42;
  static constexpr uint8_t HEADER_BYTE2 PROGMEM = 0x4D;
  static constexpr uint8_t PACKET_LENGTH PROGMEM =
      28; // Packet length without header bytes
  static constexpr uint8_t CMD_READ PROGMEM = 0xE2;
  static constexpr uint8_t CMD_CHANGE_MODE = 0xE1;
  static constexpr uint8_t SERIAL_MODE_PASSIVE = 0x00;
  static constexpr uint16_t CMD_SERIAL_MODE_PASSIVE_CHECKSUM PROGMEM =
      HEADER_BYTE1 + HEADER_BYTE2 + CMD_CHANGE_MODE + SERIAL_MODE_PASSIVE;
  static constexpr uint16_t CMD_READ_CHECKSUM =
      HEADER_BYTE1 + HEADER_BYTE2 + CMD_READ;
};

#endif //__TEMP_SENSOR_H