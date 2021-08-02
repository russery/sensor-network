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

#include <Adafruit_BME280.h>
#include <Wire.h>

class EnvSensor {
  typedef struct {
    float temperature_C;
    float temperature_F;
    float humidity_percent;
    float pressure_Pa;
    bool valid;
  } SensorData_t;

private:
  TwoWire *wire_;
  Adafruit_BME280 bme;

public:
  SensorData_t data;
  explicit EnvSensor(TwoWire *wire) : wire_(wire) { data.valid = false; }
  void Start(void);
  void Loop(void);
};