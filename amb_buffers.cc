#include "amb_buffers.h"

#include <cerrno>
#include <cstdlib>
#include <queue>
#include <vector>

namespace amb {

using ::std::queue;

int Buffers::Allocate(const int number, const size_t capacity,
                      queue<Buffer>* buffers) {
  if (number == 0 || capacity == 0 || buffers == nullptr)
    return -EINVAL;
  else if (!buffers_.empty())
    return -EBUSY;
  else
    for (int index = 0; index < number; ++index) {
      Buffer buffer;

      buffer.data = calloc(1, capacity);
      if (buffer.data == NULL) {
        Deallocate();
        return -ENOMEM;
      }
      buffer.capacity = capacity;
      buffer.size = 0;
      buffer.last = false;

      buffers_.push_back(buffer);
    }

  for (const Buffer& buffer : buffers_)
    buffers->push(buffer);

  return 0;
}

void Buffers::Deallocate() {
  for (const Buffer& buffer : buffers_)
    free(buffer.data);
  buffers_.clear();
}

};
