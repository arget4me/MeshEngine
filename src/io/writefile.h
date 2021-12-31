#ifndef WRITEFILE_HEADER
#define WRITEFILE_HEADER

#include <common.h>
#include <utils/log.h>


int write_buffer_overwrite(const char* filepath, void* buffer, int buffer_size);

int write_buffer_append(const char* filepath, void* buffer, int buffer_size);


//#define WRITEFILE_IMPLEMENTATION
#ifdef WRITEFILE_IMPLEMENTATION

int write_buffer_overwrite(const char* filepath, void* buffer, int buffer_size)
{
	// open file for writing
	// write to file
}

int write_buffer_append(const char* filepath, void* buffer, int buffer_size)
{
	// open file for writing with append flag
	// write to file
}

#endif


#endif