#ifndef IMAGE_IO_HEADER
#define IMAGE_IO_HEADER
/** 
   =====================================================================================
    NOTE: This is for internal use only.
    It does not provide a robust image loading in case of malicious or unexpected input.
   =====================================================================================
**/

#include <common.h>
#include <utils/log.h>


namespace MESHAPI
{
struct ColorRGBA
{
    uint8 red;
    uint8 green;
    uint8 blue;
    uint8 alpha;
};

struct PNGFile
{
    uint32 width;
    uint32 height;
    ColorRGBA* data;
};

// The output will have the data filed mapped to the same address as outputImageBuffer. And it will be constrianed to an upper byte limit defined by outputMaxSize.
PNGFile ParsePNGFile(const uint8* filebuffer, uint32 filesize, ColorRGBA* outputImageBuffer, uint32 outputMaxSize);

}

// #define IMAGE_IO_IMPLEMENTATION
#ifdef IMAGE_IO_IMPLEMENTATION

namespace MESHAPI
{

/**
  ================================================================================
    PNG (Portable Network Graphics) (Second Edition)
    
    Following spec: Portable Network Graphics (PNG) Specification (Second Edition)
    https:// www.w3.org/ TR/2003/ REC-PNG-20031110/
  ================================================================================
**/




constexpr uint64 PNG_HEADER = 0x89504e470d0a1a0a; // All PNG files must start with this header: Decimal(137 80 78 71 13 10 26 10) || Hex(0x89 0x50 0x4e 0x47 0x0d 0x0a 0x1a 0x0a) || ASCII(\211 P N G \r \n \032 \n)

#define CHUNK_PROPERTY_BIT 0x32

constexpr uint32 IHDR = 0x49484452; // Image header:  Must be the first chunk
constexpr uint32 IPLT = 0x49504C54; // Palete:        If present, must it appear before the IDAT chunk. Chunk length describe number of entries (rgb) and chunk data length must be divisible by 3. Required for color type 3, valid for color type 2 and 6. must not appear for color type 0 and 4
constexpr uint32 IDAT = 0x49444154; // Image data:    Compressed or non-compressed image data described by IHDR. There can be multiple IDAT chunks
constexpr uint32 IEND = 0x49454E44; // Image trailer: Empty chunk marking the end of the PNG file
// There are several Ancillary chunks that aren't neccessarialy needed to read the PNG file.

#pragma pack(push, 1)
struct IHDRChunk
{
    uint32 Width; 
    uint32 Height; 
    uint8  ColorRGBA; 
    uint8  Compression;
    uint8  Filter; 
    uint8  Interlace; 
};

struct IPLTChunkEntry
{
    uint8 red;
    uint8 green;
    uint8 blue;
};

struct PNGFileChunkHeader
{
    uint32 Lenght;
    union 
    {
        uint32 ChunkType;
        struct
        {
            uint8 _ancillary;
            uint8 _private;
            uint8 _reversed;
            uint8 _safetocopy;
        };
    };
};

struct PNGFileChunk
{
    PNGFileChunkHeader Header;
    uint8* ChunkData;
    uint32 CRC;
};
#pragma pack(pop)

PNGFile ParsePNGFile(const uint8* filebuffer, uint32 filesize, ColorRGBA* outputImageBuffer, uint32 outputMaxSize)
{
    if(filebuffer == nullptr || filesize == 0 || outputImageBuffer == nullptr || outputMaxSize == 0)
    {
        ERRORLOG("Can't parse png. Bad input and output parameteres\n");
        return {0, 0, nullptr};
    }

    PNGFile result{0, 0, nullptr};
    IHDRChunk* ihdr = nullptr;
    uint32 index = 0;
    
    if( filesize - index > sizeof(PNG_HEADER) && *((decltype(PNG_HEADER)*)(filebuffer + index)) == PNG_HEADER )
    {
        index += sizeof(PNG_HEADER);
        int prev_index = index;
        while(filesize - index - sizeof(PNGFileChunkHeader) > 0)
        {
            PNGFileChunk chunk;
            chunk.Header = *(PNGFileChunkHeader*)(void*)(filebuffer + index);
            index += sizeof(PNGFileChunkHeader);
            if(filesize - index - chunk.Header.Lenght - sizeof(uint32) > 0 )
            {
                chunk.ChunkData = (uint8*)(filebuffer + index);
                index += chunk.Header.Lenght;
                chunk.CRC = *((uint32*)(filebuffer + index));
                index += sizeof(decltype(PNGFileChunk::CRC));
            }
            if(prev_index >= index)
            {
                // Some error happened
                ERRORLOG("Unkown error parsing png file.\n");
                return {0, 0, nullptr};
            }

            switch(chunk.Header.ChunkType)
            {
                case IHDR:
                {
                    if(chunk.Header.Lenght == sizeof(IHDRChunk))
                    {
                        ihdr = (IHDRChunk*)chunk.ChunkData;
                        result.width = ihdr->Width;
                        result.height = ihdr->Height;
                    }
                    else
                    {
                        ERRORLOG("Bad IHDR chunk length field");
                        return {0, 0, nullptr};
                    }
                }break;
                case IPLT:
                {

                }break;
                case IDAT:
                {

                }break;
                case IEND:
                {
                    
                }break;
            }
        }
    }

    return result;
}
}

#endif
#endif