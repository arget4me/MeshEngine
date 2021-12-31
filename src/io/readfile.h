#ifndef READFILE_HEADER
#define READFILE_HEADER

#include <common.h>
#include <utils/log.h>


int read_buffer(const char* filepath, void* buffer, int buffer_size);

int read_buffer_offset(const char* filepath, void* buffer, int buffer_size, int file_offset);

#define READFILE_IMPLEMENTATION
#ifdef READFILE_IMPLEMENTATION


#if _WIN64

#include <io.h>
#include <sys\stat.h>

#elif __linux__

#endif


int read_buffer(const char* filepath, void* buffer, int buffer_size)
{
    // file at filepath
    // Read into buffer of maximim bytes buffer_size
#if _WIN64 || _WIN32
    TODO_IMPLEMENT_WINDOWS();

    // int filehandle;
    // _sopen_s(&filehandle, filepath, _O_RDONLY | _O_BINARY, _S_IREAD);
    // if(filedescriptor != -1)
    // {
    //     const int bytesReadCount = _read(filedescriptor, memoryBuffer, buffersize);
    //     _close( filedescriptor );
    // }
    

#elif __linux__
    TODO_IMPLEMENT_LINUX();
#endif

    return 0;
}

int read_buffer_offset(const char* filepath, void* buffer, int buffer_size, int file_offset)
{
    // file at filepath

	// Start reading from file at file offset.

    // Read into buffer of maximim bytes buffer_size
#if _WIN64 || _WIN32
    TODO_IMPLEMENT_WINDOWS();
#elif __linux__
    TODO_IMPLEMENT_LINUX();
#endif
    return 0;
}
#endif


#endif