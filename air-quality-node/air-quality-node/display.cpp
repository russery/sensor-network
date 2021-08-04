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

#include "display.h"
#include "bsp.h"
#include <algorithm>

void Display::Start(void) {
  if (!display_.begin(
          SSD1306_SWITCHCAPVCC, // Generate display voltage from 3.3V internally
          SCREEN_ADDRESS,
          false, // reset = false; no hardware reset
          false  // periphBegin = false; Wire.begin() is handled elsewhere
          )) {
    Serial.println(F("SSD1306 allocation failed"));
    return;
  }

  display_.dim(true);
  display_.setTextSize(2);
  display_.setTextColor(SSD1306_WHITE);
  display_.cp437(true); // Use "Code Page 437" font
  display_.clearDisplay();
  display_.display();
}

void Display::WriteText(char *text) {
  display_.clearDisplay();
  display_.setCursor(0, 0);
  display_.write(text);
  display_.display();
}

void Display::DrawGraphTicks(void) {
  // Draw vertical axis from bottom of screen:
  for (int y = SCREEN_HEIGHT - 1; y > (SCREEN_HEIGHT - 1 - GRAPH_HEIGHT);
       y -= GRAPH_TICK_INTERVAL_Y) {
    display_.drawPixel(0, y, SSD1306_WHITE);
  }
  // Draw horizontal axis along bottom of screen:
  for (int x = 0; x < GRAPH_WIDTH; x += GRAPH_TICK_INTERVAL_X) {
    display_.drawPixel(x, SCREEN_HEIGHT - 1, SSD1306_WHITE);
  }
  display_.display();
}

void Display::Update(uint16_t value) {
  display_.clearDisplay();
  DrawGraphTicks();
  display_.setCursor(0, 0);
  char buff[16];
  sprintf(buff, "PM2.5:%4d", value);
  display_.write(buff);

  sum_values_ += value;
  num_values_++;
  if (millis() - last_update_time_ > GRAPH_UPDATE_PERIOD_MS) {
    display_history_.Push(sum_values_ / num_values_);
    sum_values_ = 0;
    num_values_ = 0;

    // Dynamically scale graph to data history:
    graph_max_value_ = 0;
    graph_min_value_ = 65535;
    for (int i = 0; i < display_history_.Length(); i++) {
      graph_min_value_ =
          min(graph_min_value_, (uint16_t)display_history_.Read(i));
      graph_max_value_ =
          max(graph_max_value_, (uint16_t)display_history_.Read(i));
    }
    last_update_time_ = millis();
  }
  // Graph all data in buffer:
  for (int i = 0; i < display_history_.Length(); i++) {
    uint8_t x = SCREEN_WIDTH - i - 1;
    uint16_t range = graph_max_value_ - graph_min_value_;
    range = max(range, (uint16_t)10);
    uint8_t y =
        SCREEN_HEIGHT -
        min(GRAPH_HEIGHT,
            (uint8_t)((uint32_t)GRAPH_HEIGHT *
                      (display_history_.Read(i) - graph_min_value_) / range)) -
        2;
    display_.drawPixel(x, y, SSD1306_WHITE);
  }
  display_.display();
}
