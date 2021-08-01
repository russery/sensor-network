/*
Handles display of data on a SSD1306-compatible OLED display.

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

#include "ring-buffer.h"
#include <Adafruit_SSD1306.h>
#include <Wire.h>

// cppcheck-suppress noConstructor
class Display {
private:
  TwoWire *wire_;
  // Display dimensions:
  static constexpr uint8_t SCREEN_WIDTH PROGMEM = 128;
  static constexpr uint8_t SCREEN_HEIGHT PROGMEM = 32;
  // Graph section dimensions:
  static constexpr uint8_t GRAPH_HEIGHT PROGMEM = 16;
  uint16_t graph_min_value_,
      graph_max_value_; // Init'd in Update() prior to use
  static constexpr uint8_t GRAPH_WIDTH PROGMEM = 128;
  static constexpr uint8_t GRAPH_TICK_INTERVAL_X PROGMEM = 12;
  static constexpr uint8_t GRAPH_TICK_INTERVAL_Y PROGMEM = 4;
  // Graph averaging:
  uint32_t sum_values_ = 0;
  uint8_t num_values_ = 0;
  long long last_update_time_ = 0;
  static constexpr uint32_t GRAPH_UPDATE_PERIOD_MS = 1000 * 60;

  RingBuffer<uint8_t> display_history_ = RingBuffer<uint8_t>(SCREEN_WIDTH - 1);

  static constexpr int8_t OLED_RESET PROGMEM =
      -1; // Reset pin # (-1 means no reset)
  static constexpr uint8_t SCREEN_ADDRESS =
      0x3C; // 0x3D for 128x64, 0x3C for 128x32
  Adafruit_SSD1306 display_;

public:
  explicit Display(TwoWire *wire) : wire_(wire) {
    display_ = Adafruit_SSD1306(SCREEN_WIDTH, SCREEN_HEIGHT, wire_, OLED_RESET);
  };
  void Start(void);
  void DrawGraphTicks(void);
  void Update(uint16_t value);
  void WriteText(char *text);
};
