#include "BMPImage.h"

#include <cstdint>
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
	uint32_t fileSize = 14 + 12 + width*height*/*(bitDepth/8)*/8*3;
	char zero[] = "\0\0\0\0";
	uint32_t offset = 26;
	uint32_t headerSize = 12;
	uint16_t width = this->width;
	uint16_t height = this->height;
	uint16_t colorPlanes = 1;
	uint16_t bitsPerPixel = /*bitDepth*/8*3;
	
	fwrite(magic, sizeof(char), 2, fd);
	fwrite(&fileSize, sizeof(uint32_t), 1, fd);
	fwrite(zero, sizeof(char), 4, fd);
	fwrite(&offset, sizeof(uint32_t), 1, fd);
	fwrite(&headerSize, sizeof(uint32_t), 1, fd);

	fwrite(&width, sizeof(uint16_t), 1, fd);
	fwrite(&height, sizeof(uint16_t), 1, fd);
	fwrite(&colorPlanes, sizeof(uint16_t), 1, fd);
	fwrite(&bitsPerPixel, sizeof(uint16_t), 1, fd);

	for(int y = height-1; y >= 0; y--)
	{
		for(int x = 0; x < width; x++)
		{
			Pixel& pixel = (*this)[x,y];
			fwrite(&pixel.b, sizeof(uint8_t), 1, fd);
			fwrite(&pixel.g, sizeof(uint8_t), 1, fd);
			fwrite(&pixel.r, sizeof(uint8_t), 1, fd);
		}
	}

	fclose(fd);
	
	return 0;
};
