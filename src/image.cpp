#include "image.h"
#include <algorithm>
#include <cstdint>

Image::Image(const Image& other)
{
	this->colorValues = other.colorValues;
	this->bpp = other.bpp;
	
	this->width = other.width;
	this->height = other.height;
	this->bitDepth = other.bitDepth;
	this->colorType = other.colorType;
	this->compressionMethod = other.compressionMethod;
	this->filterMethod = other.filterMethod;
	this->interlaceMethod = other.interlaceMethod;
	
	pixels = std::make_unique<Pixel[]>(width*height);
	std::copy_n(other.pixels.get(), width*height, pixels.get());
}

Pixel& Image::operator[](int x, int y)
{
	return pixels[x + y*width];
};
