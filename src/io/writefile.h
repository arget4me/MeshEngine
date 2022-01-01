#ifndef WRITEFILE_HEADER
#define WRITEFILE_HEADER

#include <common.h>
#include <utils/log.h>
#include "types_io.h"

namespace MESHAPI
{
int WriteFullFileOverwrite(const char* filepath, FullFile file);
int WriteFullFileAppend(const char* filepath, FullFile file);
}

//#define WRITEFILE_IMPLEMENTATION
#ifdef WRITEFILE_IMPLEMENTATION

namespace MESHAPI
{
int WriteFullFileOverwrite(const char* filepath, FullFile file)
{
	// open file for writing
	// write to file
}

int WriteFullFileAppend(const char* filepath, FullFile file)
{
	// open file for writing with append flag
	// write to file
}
}

#endif


#endif