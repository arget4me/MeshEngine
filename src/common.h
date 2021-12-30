#ifndef COMMON_HEADER
#define COMMON_HEADER

#define APPLICATION_NAME "Mesh Engine"

#define GLEW_STATIC

#include <stdint.h>

#define Pi32 3.14159265359f

using int8   = int8_t  ;
using int16  = int16_t ; 
using int32  = int32_t ; 
using int64  = int64_t ; 
using bool32 = int32   ;
using uint8  = uint8_t ; 
using uint16 = uint16_t;
using uint32 = uint32_t;
using uint64 = uint64_t;
using real32 = float   ;
using real64 = double  ;

#if _WIN64
    #ifndef UNICODE
    #define UNICODE
    #endif
    #define WINDOWS_APPLICATION_NAME (TEXT(APPLICATION_NAME) TEXT(" - Windows"))
#elif __linux__
#define LINUX_APPLICATION_NAME (APPLICATION_NAME " - Linux")
#ifdef RASPBERRY_PI
    #define RASPBERRY_PI_APPLICATION_NAME (APPLICATION_NAME " - RaspberryPi GLX")
#endif
#endif


#endif