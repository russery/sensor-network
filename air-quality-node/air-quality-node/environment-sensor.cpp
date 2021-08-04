/*
Interface for a BME280 pressure/humidity/temp sensor.

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

#include "environment-sensor.h"

void EnvSensor::Start(void) {
  if (!bme.begin(0x76, wire_)) {
    Serial.println("Could not find BME280 sensor");
    return;
  }
}

void EnvSensor::Loop(void) {
  data.temperature_C = bme.readTemperature();
  data.temperature_F = data.temperature_C * 9 / 5 + 32.0;
  data.humidity_percent = bme.readHumidity();
  data.pressure_Pa = bme.readPressure();
  data.valid = true;
}

void EnvSensor::SetTempOffset(float offset) {
  bme.setTemperatureCompensation(offset);
}
