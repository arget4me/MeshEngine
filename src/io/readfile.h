#ifndef READFILE_HEADER
#define READFILE_HEADER

#include <common.h>
#include <utils/log.h>
#include "types_io.h"

namespace MESHAPI
{
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


FullFile ReadFullFile(const char* filepath, uint8* buffer, int buffer_size)
{
    FullFile result{nullptr, 0};
    if( filepath == nullptr || buffer == nullptr || buffer_size <= 0 )
    {
        return result;
    }
    // file at filepath
    // Read into buffer of maximim bytes buffer_size

#if _WIN64 || _WIN32
    int filehandle;
    errno_t error = _sopen_s(&filehandle, filepath, _O_RDONLY | _O_BINARY, _SH_DENYWR, _S_IREAD);
    if(error == 0)
    {
        const int32 bytesReadCount = _read(filehandle, buffer, buffer_size);
        _close( filehandle );
        result.buffer_size = bytesReadCount;
        result.buffer = buffer;
        return result;
    }
    else
    {
        switch(error)
        {
            case EACCES: 
            {
                ERRORLOG("Unable to open file %s: EACCES ERROR: The given path is a directory, or the file is read-only, but an open-for-writing operation was attempted.\n%s\n", filepath, strerror( error ) );
            }break;
            case EEXIST: 
            {
                ERRORLOG("Unable to open file %s: EEXIST ERROR: _O_CREAT and _O_EXCL flags were specified, but filename already exists.\n%s\n", filepath, strerror( error ) );
            }break;
            case EINVAL: 
            {
                ERRORLOG("Unable to open file %s: EINVAL ERROR: Invalid oflag, shflag, or pmode argument, or pfh or filename was a null pointer.\n%s\n", filepath, strerror( error ) );
            }break;
            case EMFILE: 
            {
                ERRORLOG("Unable to open file %s: EMFILE ERROR: No more file descriptors available.\n%s\n", filepath, strerror( error ) );
            }break;
            case ENOENT: 
            {
                ERRORLOG("Unable to open file %s: ENOENT ERROR: File or path not found.\n%s\n", filepath, strerror( error ) );
            }break;
        }
        return result;
    }
#elif __linux__
    TODO_IMPLEMENT_LINUX();
    int filehandle = open(filepath, O_RDONLY);
    if(filehandle != -1)
    {
        ssize_t bytesReadCount = read(filehandle, buffer, buffer_size);
        close( filehandle );
        result.buffer_size = bytesReadCount;
        result.buffer = buffer;
        return result;
    }
    else
    {
        ERRORLOG("Unable to open file %s: %s\n", filepath, strerror( errno ));
        return result;
    }


#endif

    return result;
}
}

#endif
#endif