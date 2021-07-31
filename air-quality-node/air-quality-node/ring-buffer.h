
template <class T> class RingBuffer {
private:
  unsigned int length_;
  unsigned int tail_ = 0;
  T *buffer_ = {};

public:
  RingBuffer(unsigned int length) : length_(length) {
    buffer_ = new T[length_];
  }
  ~RingBuffer() { delete buffer_; }
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
