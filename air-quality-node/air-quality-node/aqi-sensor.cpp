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

#include "aqi-sensor.h"

void AQISensor::Start(void) {
  delay(1000); // Wait for sensor to boot
  Serial2.begin(9600);
  sensor.begin_UART(&Serial2);
}

void AQISensor::Loop(void) {
  if (sensor.read(&data)) {
    Serial.println("AQI reading success");
    Serial.println();
    Serial.println(F("---------------------------------------"));
    Serial.println(F("Concentration Units (standard)"));
    Serial.println(F("---------------------------------------"));
    Serial.print(F("PM 1.0: "));
    Serial.print(data.pm10_standard);
    Serial.print(F("\t\tPM 2.5: "));
    Serial.print(data.pm25_standard);
    Serial.print(F("\t\tPM 10: "));
    Serial.println(data.pm100_standard);
    Serial.println(F("Concentration Units (environmental)"));
    Serial.println(F("---------------------------------------"));
    Serial.print(F("PM 1.0: "));
    Serial.print(data.pm10_env);
    Serial.print(F("\t\tPM 2.5: "));
    Serial.print(data.pm25_env);
    Serial.print(F("\t\tPM 10: "));
    Serial.println(data.pm100_env);
    Serial.println(F("---------------------------------------"));
    Serial.print(F("Particles > 0.3um / 0.1L air:"));
    Serial.println(data.particles_03um);
    Serial.print(F("Particles > 0.5um / 0.1L air:"));
    Serial.println(data.particles_05um);
    Serial.print(F("Particles > 1.0um / 0.1L air:"));
    Serial.println(data.particles_10um);
    Serial.print(F("Particles > 2.5um / 0.1L air:"));
    Serial.println(data.particles_25um);
    Serial.print(F("Particles > 5.0um / 0.1L air:"));
    Serial.println(data.particles_50um);
    Serial.print(F("Particles > 10 um / 0.1L air:"));
    Serial.println(data.particles_100um);
    Serial.println(F("---------------------------------------"));
  }
}
