#ifndef LOG_HEADER
#define LOG_HEADER

#include <stdio.h>
#ifdef DEBUG
    #define ERRORLOG(...) fprintf(stderr, __VA_ARGS__);
    #define LOG(...) fprintf(stdout, __VA_ARGS__);
    #define TODO_IMPLEMENT(PLATFORM) ERRORLOG("%s :%d [ %s ] is not implemented for [ %s ]\n", __FILE__, __LINE__, __FUNCTION__, PLATFORM)
    #define TODO_IMPLEMENT_WINDOWS() TODO_IMPLEMENT("Windows")
    #define TODO_IMPLEMENT_LINUX() TODO_IMPLEMENT("Linux")
    
#else
    #define ERRORLOG(...) 
    #define LOG(...) 
    #define TODO_IMPLEMENT(PLATFORM) 
    #define TODO_IMPLEMENT_WINDOWS()
    #define TODO_IMPLEMENT_LINUX()
#endif

#endif