/*
Temperature sensor interface.

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

#include "temp-sensor.h"
#include "bsp.h"

void TempSensor::Start(void) { pinMode(BSP::DHT11_DATA_PIN, INPUT); }

float TempSensor::GetTemperatureCelcius(void) {
  return sensor.readTemperature();
}

float TempSensor::GetHumidityPercent(void) { return sensor.readHumidity(); }
