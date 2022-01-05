#ifndef READFILE_HEADER
#define READFILE_HEADER

#include <common.h>
#include <utils/log.h>
#include "types_io.h"

namespace MESHAPI
{
FullFile ReadFullFileAllocate(const char* filepath);
FullFile ReadFullFile(const char* filepath, uint8* buffer, int buffer_size);
}

// #define READFILE_IMPLEMENTATION
#ifdef READFILE_IMPLEMENTATION

namespace MESHAPI
{
#if _WIN64

#include <io.h>
#include <sys\stat.h>
#include <fcntl.h>

#elif __linux__
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#endif
FullFile ReadFullFileAllocate(const char* filepath)
{
    FullFile result{nullptr, 0};
    if( filepath == nullptr )
    {
        return result;
    }

    FILE * filehandle = fopen (filepath, "rb");
    if (filehandle != NULL)
    {
        fseek(filehandle, 0L, SEEK_END);
        int32 buffer_size = (int32)ftell(filehandle);
        fseek(filehandle, 0L, SEEK_SET);

        uint8* buffer = (uint8*)malloc(buffer_size);
        const int32 bytesReadCount = fread(buffer, 1, buffer_size, filehandle);
        fclose(filehandle);
        
        result.buffer_size = bytesReadCount;
        result.buffer = buffer;
        return result;
    }
    else
    {
        ERRORLOG("Unable to open file %s: %s\n", filepath, strerror( errno ) );
    }
    return result;
}


FullFile ReadFullFile(const char* filepath, uint8* buffer, int buffer_size)
{
    FullFile result{nullptr, 0};
    if( filepath == nullptr || buffer == nullptr || buffer_size <= 0 )
    {
        return result;
    }
    // file at filepath
    // Read into buffer of maximim bytes buffer_size
    FILE * filehandle;
    filehandle = fopen (filepath, "rb");
    if (filehandle != NULL)
    {
        const int32 bytesReadCount = fread(buffer, 1, buffer_size, filehandle);
        fclose(filehandle);
        result.buffer_size = bytesReadCount;
        result.buffer = buffer;
        return result;
    }
    else
    {
        ERRORLOG("Unable to open file %s: %s\n", filepath, strerror( errno ) );
    }

    return result;
}
}

#endif
#endif