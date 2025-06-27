#include "BMPImage.h"

#include <iostream>

using std::cout, std::endl;

int BMPImage::readFromFile(std::string filename)
{
	cout << "Not implemented" << endl;
	return 2;
};

int BMPImage::writeToFile(std::string filename)
{
	FILE* fd = fopen(filename.c_str(), "w");
	char magic[] = "BM";
	fwrite(magic, sizeof(char), 2, fd);
	uint32_t fileSize = 14 + 12 + width*height*/*(bitDepth/8)*/8*3;
	fwrite(&fileSize, sizeof(uint32_t), 1, fd);
	char zero[] = "\0\0\0\0";
	fwrite(zero, sizeof(char), 4, fd);
	uint32_t offset = 26;
	fwrite(&offset, sizeof(uint32_t), 1, fd);
	uint32_t headerSize = 12;
	fwrite(&headerSize, sizeof(uint32_t), 1, fd);
	uint16_t width = this->width;
	uint16_t height = this->height;
	uint16_t colorPlanes = 1;
	uint16_t bitsPerPixel = /*bitDepth*/8*3;
	fwrite(&width, sizeof(uint16_t), 1, fd);
	fwrite(&height, sizeof(uint16_t), 1, fd);
	fwrite(&colorPlanes, sizeof(uint16_t), 1, fd);
	fwrite(&bitsPerPixel, sizeof(uint16_t), 1, fd);

	for(int y = height-1; y >= 0; y--)
	{
		for(int x = 0; x < width; x++)
		{
			Pixel<uint8_t> pixel = getPixel<uint8_t>(x, y);
			fwrite(&pixel.b, bitDepth/8, 1, fd);
			fwrite(&pixel.g, bitDepth/8, 1, fd);
			fwrite(&pixel.r, bitDepth/8, 1, fd);
		}
	}

	fclose(fd);
	
	return 0;
};
