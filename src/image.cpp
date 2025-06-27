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
	
	unsigned long imageDataSize = width * height * bpp;
	imageData = std::make_unique<uint8_t[]>(imageDataSize);
	std::copy_n(other.imageData.get(), imageDataSize, imageData.get());
}
