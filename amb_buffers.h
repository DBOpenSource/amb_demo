#pragma once

#include <cstddef>
#include <queue>
#include <vector>

namespace amb {

using ::std::queue;
using ::std::vector;

struct Metadata {
  int num_channels;
  int sample_rate;
  int sample_size;
};

struct Buffer {
  void* data;
  size_t capacity;
  size_t size;
  bool last;
};

class Buffers {
 public:
  Buffers() {};
  ~Buffers() {};

  int Allocate(const int number, const size_t capacity, queue<Buffer>* buffers);
  void Deallocate();

 private:
  vector<Buffer> buffers_;

  Buffers(const Buffers&) = delete;
  Buffers(Buffers&&) = delete;
  Buffers& operator=(const Buffers&) = delete;
  Buffers& operator=(const Buffers&&) = delete;
};

};
