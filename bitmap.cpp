#include "bitmap.h"

#include <fstream>
#include <iostream>
#include <ios>
#include <memory>

#include <cstdio>
#include <cmath>

typedef struct
{
    char header[2];
    char size[4];
    int16_t res1;
    int16_t res2;
    char offset[4];

} BitmapFileHeader;

typedef struct
{
    uint32_t headerSize;
    uint32_t width;
    uint32_t height;
    uint16_t planes;
    uint16_t BpP;
    uint32_t compression;
    uint32_t imageSize;
    uint32_t xPixelsPerM;
    uint32_t yPixelsPerM;
    uint32_t colorsInCTable;
    uint32_t importantColorCount;
} DIBHeader;

Bitmap::~Bitmap()
{
    delete [] array;
}

void Bitmap::putPixel(unsigned x, unsigned y, unsigned color)
{
    memcpy(array + (y * width + x) * bytesPerPixel, &color, bytesPerPixel);
}

Bitmap::Bitmap(unsigned width, unsigned height, uint16_t bytesPerPixel)
: width(width), 
  height(height),
  bytesPerPixel(bytesPerPixel)
{
    array = new char[width * height * bytesPerPixel] { 0 }; 
}

void Bitmap::commit(std::string filename)
{

    std::ofstream bmp(filename, std::ios_base::binary);

    size_t size = bytesPerPixel * width * height;

    BitmapFileHeader header { 0 };
    memcpy(header.header, "BM", 2);
    int32_t siz = sizeof(BitmapFileHeader) + sizeof(DIBHeader) + size;
    memcpy(header.size, &siz, 4);
    int32_t offset = sizeof(BitmapFileHeader) + sizeof(DIBHeader);
    memcpy(header.offset, &offset, 4);
    bmp.write((const char*) &header, sizeof(header));

    DIBHeader dibHeader 
    {
        sizeof(DIBHeader), //headersize
        width, //width
        height, //height
        1, //planes, should be 1
        static_cast<uint16_t>(bytesPerPixel * 8), //Bits per pixel
        0,//compression, 0 - uncompressed
        size, //image size 2 - one pixel + 2 for padding
        2835, // xPix
        2835, //yPix
        0, //colors
        0, //importantcolors
    };

    bmp.write((const char*) &dibHeader, sizeof(DIBHeader));

    for(int i = 0; i < width * height; i++)
    {
        bmp.write((const char*) (array + i * bytesPerPixel), bytesPerPixel);
    }

    bmp.close();
}
