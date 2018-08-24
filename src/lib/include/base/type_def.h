// define some type
#pragma once

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
