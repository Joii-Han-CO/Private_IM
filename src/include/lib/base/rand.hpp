#pragma once
#include <vector>

#pragma region namespace
namespace base {
namespace im_rand {
#pragma endregion

#ifdef WIN32
__declspec (naked) inline unsigned __int64 GetCpuCycle(void) {
  _asm
  {
    rdtsc
    ret
  }
}
#else
static inline unsigned __int64 GetCpuCycle() {
  unsigned int lo, hi;
  __asm__ __volatile__("rdtsc" : "=a" (lo), "=d" (hi));
  return ((uint64_t)hi << 32) | lo;
}
#endif

inline void SetRand() {
  srand((unsigned int)GetCpuCycle());
}

inline int GetRandInt(int begin, int end) {
  int i = rand();
  i = i % (end - begin);
  i += begin;
  return i;
}

// 填充随机数
inline void FillingRand(void *data, int len) {
  unsigned char *d = (unsigned char*)data;
  for (int i = 0; i < len; i++) {
    d[i] = (unsigned char)rand();
  }
}

inline std::vector<char> FillingRand(int len) {
  std::vector<char> buf;
  buf.resize(len);
  FillingRand(buf.data(), (int)buf.size());
  return buf;
}

#pragma region namespace
}
}
#pragma endregion
