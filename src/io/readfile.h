#ifndef READFILE_HEADER
#define READFILE_HEADER

#include <common.h>
#include <utils/log.h>


int read_buffer(const char* filepath, void* buffer, int buffer_size);

// #define READFILE_IMPLEMENTATION
#ifdef READFILE_IMPLEMENTATION


#if _WIN64

#include <io.h>
#include <sys\stat.h>
#include <fcntl.h>

#elif __linux__

#endif


int read_buffer(const char* filepath, void* buffer, int buffer_size)
{
    if( filepath == nullptr || buffer == nullptr || buffer_size <= 0 )
    {
        return -1;
    }
    // file at filepath
    // Read into buffer of maximim bytes buffer_size
#if _WIN64 || _WIN32
    int filehandle;
    errno_t error = _sopen_s(&filehandle, filepath, _O_RDONLY | _O_BINARY, _SH_DENYWR, _S_IREAD);
    if(error == 0)
    {
        const int bytesReadCount = _read(filehandle, buffer, buffer_size);
        _close( filehandle );
        return bytesReadCount;
    }
    else
    {
        switch(error)
        {
            case EACCES: 
            {
                ERRORLOG("EACCES ERROR: The given path is a directory, or the file is read-only, but an open-for-writing operation was attempted.\n%s\n", strerror( error ) );
            }break;
            case EEXIST: 
            {
                ERRORLOG("EEXIST ERROR: _O_CREAT and _O_EXCL flags were specified, but filename already exists.\n%s\n", strerror( error ) );
            }break;
            case EINVAL: 
            {
                ERRORLOG("EINVAL ERROR: Invalid oflag, shflag, or pmode argument, or pfh or filename was a null pointer.\n%s\n", strerror( error ) );
            }break;
            case EMFILE: 
            {
                ERRORLOG("EMFILE ERROR: No more file descriptors available.\n%s\n", strerror( error ) );
            }break;
            case ENOENT: 
            {
                ERRORLOG("ENOENT ERROR: File or path not found.\n%s\n", strerror( error ) );
            }break;
        }
        return -1;
    }
#elif __linux__
    TODO_IMPLEMENT_LINUX();
#endif

    return 0;
}

#endif