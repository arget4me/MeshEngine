#ifndef WRITEFILE_HEADER
#define WRITEFILE_HEADER

#include <common.h>
#include <utils/log.h>


int write_buffer_overwrite(std::string filepath, void* buffer, int buffer_size);

int write_buffer_append(std::string filepath, void* buffer, int buffer_size);


//#define WRITEFILE_IMPLEMENTATION
#ifdef WRITEFILE_IMPLEMENTATION

int write_buffer_overwrite(std::string filepath, void* buffer, int buffer_size)
{
	std::ofstream out;
	out.open(filepath, std::ios::out | std::ios::binary);
	if (out.is_open())
	{
		out.write((char*)buffer, buffer_size);
		out.close();
		return 0;
	}
	else
	{
		ERROR_LOG("Unable to open file \"" << filepath << "\"\n");
		return -1;
	}
}

int write_buffer_append(std::string filepath, void* buffer, int buffer_size)
{
	std::ofstream out;
	out.open(filepath, std::ios::out | std::ios::binary | std::ios::app);
	if (out.is_open())
	{
		out.write((char*)buffer, buffer_size);
		out.close();
		return 0;
	}
	else
	{
		ERROR_LOG("Unable to open file \"" << filepath << "\"\n");
		return -1;
	}
}

#endif


#endif