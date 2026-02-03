#ifndef __CIRCULAR_BUFFER_H__
#define __CIRCULAR_BUFFER_H__

#include <deque>
#include <cstddef>

/**
 * Simple circular buffer implementation to replace boost::circular_buffer.
 * Uses std::deque internally with manual capacity enforcement.
 */
template<typename T>
class CircularBuffer
{
public:
  using value_type = T;
  using size_type = std::size_t;
  using reference = T&;
  using const_reference = const T&;
  using iterator = typename std::deque<T>::iterator;
  using const_iterator = typename std::deque<T>::const_iterator;

  CircularBuffer() : _capacity(0) {}

  explicit CircularBuffer(size_type capacity) : _capacity(capacity) {}

  void set_capacity(size_type capacity)
  {
    _capacity = capacity;
    while (_data.size() > _capacity)
      _data.pop_front();
  }

  size_type capacity() const { return _capacity; }
  size_type size() const { return _data.size(); }
  bool empty() const { return _data.empty(); }
  bool full() const { return _data.size() >= _capacity; }

  void push_back(const T& value)
  {
    if (_capacity == 0) return;
    if (_data.size() >= _capacity)
      _data.pop_front();
    _data.push_back(value);
  }

  void push_back(T&& value)
  {
    if (_capacity == 0) return;
    if (_data.size() >= _capacity)
      _data.pop_front();
    _data.push_back(std::move(value));
  }

  void clear() { _data.clear(); }

  reference operator[](size_type index) { return _data[index]; }
  const_reference operator[](size_type index) const { return _data[index]; }

  reference front() { return _data.front(); }
  const_reference front() const { return _data.front(); }
  reference back() { return _data.back(); }
  const_reference back() const { return _data.back(); }

  iterator begin() { return _data.begin(); }
  iterator end() { return _data.end(); }
  const_iterator begin() const { return _data.begin(); }
  const_iterator end() const { return _data.end(); }
  const_iterator cbegin() const { return _data.cbegin(); }
  const_iterator cend() const { return _data.cend(); }

private:
  std::deque<T> _data;
  size_type _capacity;
};

#endif // __CIRCULAR_BUFFER_H__
