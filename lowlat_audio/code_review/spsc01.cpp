#include <atomic>
#include <vector>
#include <stdexcept>
#include<iostream>

template<typename T>
class RingBuffer {
public:
  explicit RingBuffer(size_t capacity): buffer_(capacity) {
    head_ = 0;
    tail_ = 0;
  }

  bool push(const T& item) {
    auto currentHead = head_.load();
    auto nextHead = (currentHead + 1) & (buffer_.size() - 1);
    if (nextHead == tail_.load()) {
      return false;  // full
    }
    buffer_[currentHead] = item;
    head_.store(nextHead);
    return true;
  }

  bool pop(T& item) {
    auto currentTail = tail_.load();
    if (currentTail == head_.load()) {
      return false;  // empty
    }
    item = buffer_[currentTail];
    tail_.store((currentTail + 1) & (buffer_.size() - 1));
    return true;
  }

private:
  std::vector<T>      buffer_;
  std::atomic<size_t> head_;
  std::atomic<size_t> tail_;
};  // missing semicolon fixed

// Usage example
int main() {
  RingBuffer<int> rb(1024);
  if (!rb.push(42)) {
    throw std::runtime_error("Buffer full");
  }
  int value;
  if (rb.pop(value)) {
    std::cout << value << "\n";
  }
  return 0;
}
