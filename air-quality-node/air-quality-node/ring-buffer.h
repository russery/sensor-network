/*
A ring buffer, where values are shifted in until it is full, and
then the oldest values are discarded on each push. Values can be
read from anywhere in the buffer by index.

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

// cppcheck-suppress [noCopyConstructor, noOperatorEq]
template <class T> class RingBuffer {
private:
  unsigned int length_;
  unsigned int tail_ = 0;
  T *buffer_ = {};

public:
  explicit RingBuffer(unsigned int length) : length_(length) {
    // cppcheck-suppress [noCopyConstructor, noOperatorEq]
    buffer_ = new T[length_];
  }
  ~RingBuffer() { delete[] buffer_; }
  void Push(T newval) {
    // Shift existing values right
    for (int i = tail_; i > 0; i--) {
      if (i != length_) {
        buffer_[i] = buffer_[i - 1];
      }
    }
    buffer_[0] = newval;
    tail_ = (tail_ == length_) ? tail_ : tail_ + 1;
  }
  T Read(unsigned int ind) { return (ind < (tail_)) ? buffer_[ind] : 0; }
  unsigned int Length(void) { return (tail_ > 0) ? tail_ : 0; }
  bool IsEmpty(void) { return tail_ == 0; }
  bool IsFull(void) { return tail_ == length_; }
};
