/*
AQI sensor interface.

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

#include "Adafruit_PM25AQI.h"

class AQISensor {
public:
  void Start(void);
  void Loop(void);

  PM25_AQI_Data data;
  // typedef struct PMSAQIdata {
  //   uint16_t framelen;       ///< How long this data chunk is
  //   uint16_t pm10_standard,  ///< Standard PM1.0
  //       pm25_standard,       ///< Standard PM2.5
  //       pm100_standard;      ///< Standard PM10.0
  //   uint16_t pm10_env,       ///< Environmental PM1.0
  //       pm25_env,            ///< Environmental PM2.5
  //       pm100_env;           ///< Environmental PM10.0
  //   uint16_t particles_03um, ///< 0.3um Particle Count
  //       particles_05um,      ///< 0.5um Particle Count
  //       particles_10um,      ///< 1.0um Particle Count
  //       particles_25um,      ///< 2.5um Particle Count
  //       particles_50um,      ///< 5.0um Particle Count
  //       particles_100um;     ///< 10.0um Particle Count
  //   uint16_t unused;         ///< Unused
  //   uint16_t checksum;       ///< Packet checksum
  // } PM25_AQI_Data;

private:
  Adafruit_PM25AQI sensor = Adafruit_PM25AQI();
};

#endif //__TEMP_SENSOR_H