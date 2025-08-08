#pragma once

#include <cstdint>
#include <cstring>
#include <memory>
#include <string>

struct Pixel
{
	uint8_t r, g, b, a;
};

class Image
{
protected:
	std::unique_ptr<Pixel[]> pixels;
	// uint8_t* imageData;
	uint8_t colorValues;
	uint8_t bpp;
public:
	Image() = default;

	// template<std::derived_from<Image> T>
	Image(const Image& other);

	virtual int readFromFile(std::string filename) = 0;
	virtual int writeToFile(std::string filename) = 0;
	
	Pixel& operator[](int x, int y);

	uint32_t width = 0;
	uint32_t height = 0;
	uint8_t bitDepth;
	uint8_t colorType;
	uint8_t compressionMethod;
	uint8_t filterMethod;
	uint8_t interlaceMethod;
};
