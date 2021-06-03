/*
Implements a simple timer for task loops.

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

#ifndef __LOOP_TIMER_H
#define __LOOP_TIMER_H

class LoopTimer {
private:
  unsigned long last_loop_ms;

public:
  LoopTimer(void) { Reset(); }
  void Reset(void) { last_loop_ms = millis(); }
  bool CheckIntervalExceeded(unsigned long interval_ms) {
    return abs((long long)millis() - (long long)last_loop_ms) > interval_ms;
  }
};

#endif //__LOOP_TIMER_H