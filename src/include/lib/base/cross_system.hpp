#pragma once

#ifndef _WIN32
#include <wchar.h>
#include <stdio.h>

#define wprintf_s(F, ...) \
  wprintf(F, ##__VA_ARGS__)

#define sprintf_s(buf, buf_size, F, ...) \
  sprintf(buf, F, ##__VA_ARGS__)

#define swprintf_s(buf, buf_size, F, ...) \
  swprintf(buf, buf_size, F, ##__VA_ARGS__)

inline errno_t memcpy_s(void *dst, rsize_t dst_size,
                        const void *src, rsize_t src_size) {
  if (src_size == 0) {
    return 0;
  }
  if (src == nullptr || dst_size < src_size) {
    return EINVAL;
  }
  memcpy(dst, src, src_size);
  return 0;
}

#endif
