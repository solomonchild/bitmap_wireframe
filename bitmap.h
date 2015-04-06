#pragma once

#include <string>

class Bitmap
{
public:
    Bitmap(unsigned width, unsigned height, uint16_t bytesPerPixel = 2);

    ~Bitmap();

    void putPixel(unsigned x, unsigned y, unsigned color);

    void commit(std::string filename);

    unsigned Width()
    {
        return width - 1;
    }

    unsigned Height()
    {
        return height - 1;
    }

private:
    unsigned width;
    unsigned height;
    uint16_t bytesPerPixel;
    char *array;
};
