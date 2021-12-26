#ifndef LOG_HEADER
#define LOG_HEADER

#include <stdio.h>
#if defined(DEBUG)
    #define ERRORLOG(...) fprintf(stderr, __VA_ARGS__);
    #define LOG(...) fprintf(stdout, __VA_ARGS__);
#else
    #define ERRORLOG(...) 
    #define LOG(...) 
#endif

#endif