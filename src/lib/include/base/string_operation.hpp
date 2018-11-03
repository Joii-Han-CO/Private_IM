#pragma once
#include "character_conversion.hpp"
#include <map>
#include <string>

#pragma region
namespace base {
#pragma endregion

#pragma region 解析参数

template<typename cus_char>
inline std::vector<cus_string> ArgsToVec(int argc, cus_char* argv[]) {
  std::vector<cus_string> vec_ref;
  if (argc == 0 || argv == nullptr || argv[0] == nullptr)
    return vec_ref;
  for (int i = 1; i < argc; i++) {
    if (argv[i])
      vec_ref.push_back(cus_string(argv[i]));
  }
  return vec_ref;
};

template<typename cus_char>
inline std::vector<cus_string> ArgsToVec(cus_string cmd) {
  std::vector<cus_string> vec_ref;

  size_t s = cmd.size();
  bool is_word = false;
  size_t begin_pos = 0;
  size_t i = 0;
  for (; i < s; i++) {
    if (is_word == true)
      continue;
    if (cmd[i] == '\"')
      is_word = !is_word;
    else if (cmd[i] == ' ') {
      auto val = cmd.substr(begin_pos, i = begin_pos);
      if (!val.empty())
        vec_ref.push_back(val);
      begin_pos = i + 1;
    }
  }
  if (begin_pos != i && begin_pos != i + 1) {
    vec_ref.push_back(cmd.substr(begin_pos, i = begin_pos));
  }
  return vec_ref;
};

template<typename _cus_string>
inline bool IsArgsCmd(_cus_string &d) {
  if (d.size() <= 1)
    return false;
  if (d[0] == '-' || d[0] == '\\' || d[0] == '/') {
    return true;
  }
  else
    return false;
}

template<typename cus_char>
inline std::map<cus_string, cus_string> ArgsToMap(int argc,
                                                  cus_char* argv[]) {
  auto v = ArgsToVec(argc, argv);
  std::map<cus_string, cus_string> m;
  size_t s = v.size();
  for (size_t i = 0; i < s; i++) {
    if (IsArgsCmd(v[i]) == false)
      continue;
    cus_string key, val;
    if (i + 1 < s) {
      if (IsArgsCmd(v[i + 1]) == false)
        val = v[i + 1];
    }
    m.insert(std::pair<cus_string, cus_string>(v[i].substr(1, v.size()),
                                               val));
    i++;
  }

  return m;
};

template<typename cus_char>
inline std::map<cus_string, cus_string> ArgsToMap(cus_string cmd) {
  auto v = ArgsToVec(cmd);
  std::map<cus_string, cus_string> m;
  size_t s = v.size();
  for (size_t i = 0; i < s; i++) {
    if (IsArgsCmd(v[i]) == false)
      continue;
    cus_string key, val;
    if (i + 1 < s) {
      if (IsArgsCmd(v[i + 1]) == false)
        val = v[i + 1];
    }
    m.insert(std::pair<cus_string, cus_string>(v[i].substr(1, v.size()),
                                               val));
    i++;
  }

  return m;
};


#define  GetArgsVal(Key, Key_Name, Def_Val) \
  std::string args_##Key = Def_Val; \
  if (args.find(Key_Name) != args.end()) \
    args_##Key = args[Key_Name];

#pragma endregion

#pragma region 字符串处理

inline size_t GetStrLen(const char *s) { return strlen(s); }
inline size_t GetStrLen(const wchar_t *s) { return wcslen(s); }

template <class _cus_string>
inline std::vector<_cus_string> SpliteStr(const _cus_string &d,
                                          const _cus_string &mark) {
  std::vector<_cus_string> ref;

  int b = 0;
  bool goon = true;
  while (goon) {
    auto i = d.find(mark, b);
    if (i == d.npos) {
      goon = false;
      i = GetStrLen(d.c_str());
    }
    auto t = d.substr(b, i - b);
    b = i + 1;
    if (t.empty())
      continue;
    ref.push_back(t);
  }
  return ref;
}

#pragma endregion

#pragma region
}
#pragma endregion

