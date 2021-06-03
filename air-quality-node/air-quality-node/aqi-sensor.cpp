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

#include "aqi-sensor.h"

void AQISensor::Start(void) {
  BSP::AQI_SERIAL_PORT->begin(9600);
  digitalWrite(BSP::AQI_RESET_PIN, 0);
  pinMode(BSP::AQI_RESET_PIN, OUTPUT);

  digitalWrite(BSP::AQI_SET_PIN, 1);
  pinMode(BSP::AQI_SET_PIN, OUTPUT);
}

void AQISensor::Loop(void) {
  enum class SensorState {
    RESET_LOW,
    WAIT_RESET,
    WAIT_BOOT,
    WAIT_PACKET,
    PARSE_BODY
  };
  static SensorState state = SensorState::RESET_LOW;
  static LoopTimer timer;

  switch (state) {
  case SensorState::RESET_LOW:
    digitalWrite(BSP::AQI_RESET_PIN, 0);
    state = SensorState::WAIT_RESET;
    timer.Reset();
    break;
  case SensorState::WAIT_RESET:
    if (timer.CheckIntervalExceeded(100)) {
      digitalWrite(BSP::AQI_RESET_PIN, 1);
      state = SensorState::WAIT_BOOT;
      timer.Reset();
    }
    break;
  case SensorState::WAIT_BOOT:
    if (timer.CheckIntervalExceeded(30000)) {
      // Datasheet recommends:
      // "Stable data should be got at least 30 seconds after the sensor wakeup
      // from the sleep mode because of the fan’s performance"
      state = SensorState::WAIT_PACKET;
      while (BSP::AQI_SERIAL_PORT->read() != -1)
        ; // Flush receive buffer
      timer.Reset();
    }
    break;
  case SensorState::WAIT_PACKET:
    if (timer.CheckIntervalExceeded(1000)) {
      state = SensorState::RESET_LOW;
    } else if (BSP::AQI_SERIAL_PORT->available() >= PACKET_LENGTH) {
      int16_t read_byte;
      do {
        read_byte = BSP::AQI_SERIAL_PORT->read();
      } while ((read_byte != -1) && (read_byte != HEADER_BYTE1));
      if (read_byte == HEADER_BYTE1) {
        if (BSP::AQI_SERIAL_PORT->read() == HEADER_BYTE2) {
          state = SensorState::PARSE_BODY;
        }
      }
    }
    break;
  case SensorState::PARSE_BODY:
    if (BSP::AQI_SERIAL_PORT->available() >= PACKET_LENGTH) {
      assert(PACKET_LENGTH % 2 == 0); // Must be an even number for 16-bit data
      uint16_t buffer[PACKET_LENGTH / 2];
      uint16_t checksum = HEADER_BYTE1 + HEADER_BYTE2;
      uint16_t length = (uint16_t)(BSP::AQI_SERIAL_PORT->read() << 8) +
                        BSP::AQI_SERIAL_PORT->read();
      checksum += (length & 0x00FF) + (length >> 8);
      if (length != PACKET_LENGTH) {
        state = SensorState::WAIT_PACKET;
        break;
      }
      uint8_t d[2];
      for (int i = 0; i < length / 2 - 1; i++) {
        BSP::AQI_SERIAL_PORT->readBytes(d, 2);
        buffer[i] = (uint16_t)(d[0] << 8) + d[1];
        checksum += d[0] + d[1];
      }
      BSP::AQI_SERIAL_PORT->readBytes(d, 2);
      uint16_t checksum_received = (uint16_t)(d[0] << 8) + d[1];
      if (checksum == checksum_received) {
        memcpy((void *)&data, (void *)buffer, PACKET_LENGTH - 4);
        stale_timer_.Reset();
      }
      state = SensorState::RESET_LOW;
    }
    break;
  }
}

bool AQISensor::IsDataStale(unsigned long stale_ms) {
  return stale_timer_.CheckIntervalExceeded(stale_ms);
}
