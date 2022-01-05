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
#include <assert.h>

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

// The output will have the data filed mapped to the same address as outputImageBuffer. And it will be constrianed to an upper byte limit defined by outputMaxSizeBytes.
PNGFile ParsePNGFile(FullFile file, ColorRGBA* outputImageBuffer, uint32 outputMaxSizeBytes);

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




constexpr uint64 PNG_HEADER = 0x0a1a0a0d474e5089; // All PNG files must start with this header: Decimal(137 80 78 71 13 10 26 10) || Hex(0x89 0x50 0x4e 0x47 0x0d 0x0a 0x1a 0x0a) || ASCII(\211 P N G \r \n \032 \n)

#define CHUNK_PROPERTY_BIT 0x32

constexpr uint32 IHDR = 0x49484452; // Image header:  Must be the first chunk
constexpr uint32 IPLT = 0x49504C54; // Palete:        If present, must it appear before the IDAT chunk. Chunk length describe number of entries (rgb) and chunk data length must be divisible by 3. Required for color type 3, valid for color type 2 and 6. must not appear for color type 0 and 4
constexpr uint32 IDAT = 0x49444154; // Image data:    Compressed or non-compressed image data described by IHDR. There can be multiple IDAT chunks
constexpr uint32 IEND = 0x49454E44; // Image trailer: Empty chunk marking the end of the PNG file
// There are several Ancillary chunks that aren't neccessarialy needed to read the PNG file.

inline uint32 LittleEndianToBigEndian(uint32& data)
{
    return (data << 24) | ((data >> 8) & 0xFF00) | ((data& 0xFF00) << 8) | (data >> 24); 
}

#pragma pack(push, 1)
struct IHDRChunk
{
    uint32 Width; 
    uint32 Height; 
    uint8  BitDepth;
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

#define ReadFromFile(file, type) (type*)_ReadFromFile(file, sizeof(type))
#define ReadMultipleFromFile(file, type, count) (type*)_ReadFromFile(file, sizeof(type) * count)
static uint8* _ReadFromFile(FullFile* file, uint32 size)
{
    assert(file->buffer_size >= size);

    uint8* result = file->buffer;
    file->buffer_size -= size;
    file->buffer += size;

    return result;
}

PNGFile ParsePNGFile(FullFile file, ColorRGBA* outputImageBuffer, uint32 outputMaxSizeBytes)
{
    FullFile parseFile = file;
    FullFile* at = &parseFile;

    if(parseFile.buffer == nullptr || parseFile.buffer_size == 0 || outputImageBuffer == nullptr || outputMaxSizeBytes == 0)
    {
        ERRORLOG("Can't parse png. Bad input and output parameteres\n");
        return {0, 0, nullptr};
    }

    PNGFile result{0, 0, nullptr};
    IHDRChunk* ihdr = nullptr;

    uint64* pngHeader = ReadFromFile(at, uint64);
    if(*pngHeader == PNG_HEADER )
    {
        bool EndOfFile = false;
        while(at->buffer_size - sizeof(PNGFileChunkHeader) > 0 && !EndOfFile)
        {
            PNGFileChunk chunk;
            chunk.Header = *ReadFromFile(at, PNGFileChunkHeader);
            chunk.Header.ChunkType = LittleEndianToBigEndian(chunk.Header.ChunkType);
            chunk.Header.Lenght = LittleEndianToBigEndian(chunk.Header.Lenght);
            
            chunk.ChunkData = ReadMultipleFromFile(at, uint8, chunk.Header.Lenght);
            chunk.CRC = *ReadFromFile(at, uint32);
            switch(chunk.Header.ChunkType)
            {
                case IHDR:
                {
                    LOG("IHDR\n");
                    if(chunk.Header.Lenght == sizeof(IHDRChunk))
                    {
                        ihdr = (IHDRChunk*)chunk.ChunkData;
                        result.width = LittleEndianToBigEndian(ihdr->Width);
                        result.height = LittleEndianToBigEndian(ihdr->Height);
                    }
                    else
                    {
                        ERRORLOG("Bad IHDR chunk length field");
                        return {0, 0, nullptr};
                    }
                }break;
                case IPLT:
                {
                    LOG("IPLT\n");
                }break;
                case IDAT:
                {
                    LOG("IDAT\n");
                    
                }break;
                case IEND:
                {
                    LOG("IEND\n");
                    if(result.data == nullptr || result.width <= 0 || result.height <= 0)
                    {
                        ERRORLOG("Got to end of png file but the file didn't contain correct image data or dimensions.\n");
                        return {0, 0, nullptr};
                    }
                    result.data = outputImageBuffer;
                    EndOfFile = true;
                }break;
                default:
                {
                    LOG("%c%c%c%c\n", (uint8)((chunk.Header.ChunkType >> 24) & 0xff), (uint8)((chunk.Header.ChunkType >> 16) & 0xff), (uint8)((chunk.Header.ChunkType >> 8) & 0xff), (uint8)((chunk.Header.ChunkType) & 0xff));
                }break;
            }
        }
    }

    return result;
}
}

#endif
#endif