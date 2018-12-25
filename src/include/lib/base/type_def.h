﻿// define some type
#pragma once
#include <functional>

// interface args type
#define IN
#define OUT
#define IN_OUT

#define INLINE inline

// Windows
#ifdef WIN32
#ifdef IM_STATIC
  // static
#define IM_LIB_EXP extern
#else
  // dynamic
#ifdef IM_LIB  // input
#define IM_LIB_EXP __declspec(dllexport)
#else  // output
#define IM_LIB_EXP __declspec(dllimport)
#endif
#endif
#else
#define IM_LIB_EXP
#endif

#ifndef UCHAR
typedef unsigned char UCHAR;
#endif

#include <string>
#define cus_string std::basic_string<cus_char, \
                                     std::char_traits<cus_char>, \
                                     std::allocator<cus_char>>

// 异步函数的完成回调
typedef std::function<void(bool)> Func_AsyncResult;

