#pragma once

#include <cstdint>
#include <cstring>
#include <string>

template <typename T>
struct Pixel
{
	T r, g, b, a;
};

struct ImageData
{



	
	//unsigned long imageDataSize = 0;
};

class Image
{
protected:
	uint8_t* imageData;
	uint8_t colorValues;
	uint8_t bpp;
public:
	Image() = default;
	~Image();

	template<std::derived_from<Image> T>
	Image(const T& other);

	virtual int readFromFile(std::string filename) = 0;
	virtual int writeToFile(std::string filename) = 0;
	
	template <typename T>
	Pixel<T> getPixel(unsigned int x, unsigned int y);

	uint32_t width = 0;
	uint32_t height = 0;
	uint8_t bitDepth;
	uint8_t colorType;
	uint8_t compressionMethod;
	uint8_t filterMethod;
	uint8_t interlaceMethod;
};



template<std::derived_from<Image> T> Image::Image(const T& other)
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
	imageData = new uint8_t[imageDataSize];
	mempcpy(imageData, other.imageData, imageDataSize);
}


template <typename T>
Pixel<T> Image::getPixel(unsigned int x, unsigned int y)
{
	Pixel<T> pixel;

	pixel.r = (T)imageData[y * width * colorValues * bitDepth/8 + x * colorValues * bitDepth/8];
	pixel.g = (T)imageData[y * width * colorValues * bitDepth/8 + x * colorValues * bitDepth/8 + 1];
	pixel.b = (T)imageData[y * width * colorValues * bitDepth/8 + x * colorValues * bitDepth/8 + 2];

	if(colorValues == 4)
		pixel.a = imageData[y * width * colorValues * bitDepth/8 + x * colorValues * bitDepth/8 + 3];
	else
		pixel.a = 0;

	return pixel;
}
