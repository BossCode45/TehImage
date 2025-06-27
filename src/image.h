#pragma once

#include <cstdint>
#include <cstring>
#include <memory>
#include <string>

template <typename T>
struct Pixel
{
	T r, g, b, a;
};


class Image
{
protected:
	std::unique_ptr<uint8_t[]> imageData;
	// uint8_t* imageData;
	uint8_t colorValues;
	uint8_t bpp;
public:
	Image() = default;

	// template<std::derived_from<Image> T>
	Image(const Image& other);

	virtual int readFromFile(std::string filename) = 0;
	virtual int writeToFile(std::string filename) = 0;
	
	template <typename T = uint8_t>
	Pixel<T> getPixel(unsigned int x, unsigned int y);

	uint32_t width = 0;
	uint32_t height = 0;
	uint8_t bitDepth;
	uint8_t colorType;
	uint8_t compressionMethod;
	uint8_t filterMethod;
	uint8_t interlaceMethod;
};


template <typename T>
Pixel<T> Image::getPixel(unsigned int x, unsigned int y)
{
	Pixel<T> pixel;

	if(sizeof(T)*8 == bitDepth)
	{
		unsigned long rIndex = y * width * colorValues + x * colorValues;
		pixel.r = ((T*)imageData.get())[rIndex];
		pixel.g = ((T*)imageData.get())[rIndex + 1];
		pixel.b = ((T*)imageData.get())[rIndex + 2];

		if(colorValues == 4)
			pixel.a = ((T*)imageData.get())[rIndex + 3];
		else
			pixel.a = 0;
	}
	else
	{
		unsigned long startIndex = y * width * colorValues * bitDepth/8 + x * colorValues * bitDepth/8;
		
		uint32_t pixelData[4];
		int tsize = sizeof(T);
		int bytesToTake = std::min(tsize, bitDepth/8);
		for(int i = 0; i < colorValues; i++)
		{
			for(int j = 0; j < bytesToTake; j++)
				pixelData[i] += imageData[startIndex++] << (bitDepth - j*8 - 8);
			startIndex += bitDepth/8 - bytesToTake;
		}
	}
	return pixel;
}
