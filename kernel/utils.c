
#include "utils.h"
void *memset(void *start, char val, size_t n_bytes) {
  uint8_t *buf_bytes = (uint8_t *)start;
  for (size_t i = 0; i < n_bytes; ++i) {
    buf_bytes[i] = val;
  }
  return start;
}
