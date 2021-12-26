#ifndef COMMON_HEADER
#define COMMON_HEADER

#define APPLICATION_NAME "Mesh Engine"

#define GLEW_STATIC

#include <stdint.h>

#define internal        static
#define local_persist   static
#define global_variable extern

#define Pi32 3.14159265359f

typedef int8_t   int8;
typedef int16_t  int16;
typedef int32_t  int32;
typedef int64_t  int64;
typedef int32    bool32;

typedef uint8_t  uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

typedef float    real32;
typedef double   real64;

#if _WIN64
    #ifndef UNICODE
    #define UNICODE
    #endif
    #define WINDOWS_APPLICATION_NAME TEXT(APPLICATION_NAME)
#elif __linux__
#ifdef RASPBERRY_PI

    #define FULLSCREEN 0
    #define SCREEN_PADDING 50

#endif
#endif


#endif