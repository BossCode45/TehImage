#include "BMPImage.h"
#include "files.h"

#include <cstdint>
#include <iostream>
#include <memory>

using std::cout, std::endl;

namespace TehImage
{

	int BMPImage::readFromFile(std::string filename)
	{
		Reader reader(filename, FileEndianness::LITTLE);

		char magic[2];
		uint8_t expected[] = {0x42, 0x4D};
		reader.readBytes(magic, 2);
		uint32_t fileSize = reader.readData<uint32_t>();
		reader.skipBytes(4);
		uint32_t offset = reader.readData<uint32_t>();

		if(strncmp(magic, (char*)expected, 2) != 0)
		{
			cout << "Not a BMP" << endl;
			return 1;
		}

		uint32_t headerSize = reader.readData<uint32_t>();

		// BITMAPCOREHEADER
		if(headerSize == 12)
		{
			width = reader.readData<uint16_t>();
			height = reader.readData<uint16_t>();
			uint16_t colorPlanes = reader.readData<uint16_t>();
			bpp = reader.readData<uint16_t>();
		}
		// BITMAPINFOHEADER
		else if(headerSize == 40)
		{
			width = reader.readData<uint32_t>();
			height = reader.readData<uint32_t>();
			uint16_t colorPlanes = reader.readData<uint16_t>();
			bpp = reader.readData<uint16_t>();
			uint32_t compressionMethod = reader.readData<uint32_t>();
			if(compressionMethod != 0)
			{
				cout << "Compression method not supported: " << compressionMethod << endl;
				return 1;
			}
			uint32_t size = reader.readData<uint32_t>();
			uint32_t horrRes = reader.readData<uint32_t>();
			uint32_t vertRes = reader.readData<uint32_t>();
			uint32_t paletteSize = reader.readData<uint32_t>();
			uint32_t importantColors = reader.readData<uint32_t>();
			// cout << compressionMethod << " " << size << endl;
		}
		else
		{
			cout << "Header type not supported: " << headerSize << endl;
			return 1;
		}

		
		if(bpp != 24)
		{
			cout << "bpp not supported: " << bpp << endl;
			return 1;
		}

		pixels = std::make_unique<Pixel[]>(width * height);
		for(int y = height-1; y >= 0; y--)
		{
			for(int x = 0; x < width; x++)
			{
				Pixel& pixel = (*this)[x,y];
				pixel.b = reader.readByte();
				pixel.g = reader.readByte();
				pixel.r = reader.readByte();
				pixel.a = 255;
			}
			
			reader.skipBytes((4-((width * 3)%4))%4);
		}
			
		
		// cout << "Not implemented" << endl;
		return 2;
	};

	int BMPImage::writeToFile(std::string filename)
	{
		Writer writer(filename, LITTLE);
		char magic[] = "BM";
		uint32_t rowSize = (bpp * width + 31)/32;
		uint32_t fileSize = 14 + 12 + rowSize*height*/*(bitDepth/8)*/8*3;
		// char zero[] = "\0\0\0\0";
		uint32_t offset = 26;
		uint32_t headerSize = 12;
		uint16_t width = this->width;
		uint16_t height = this->height;
		uint16_t colorPlanes = 1;
		uint16_t bitsPerPixel = /*bpp*/8*3;

		writer.writeBytes(magic, 2);
		writer.writeData(fileSize);
		writer.zeroBytes(4);
		writer.writeData(offset);
		writer.writeData(headerSize);

		writer.writeData(width);
		writer.writeData(height);
		writer.writeData(colorPlanes);
		writer.writeData(bitsPerPixel);

		for(int y = height-1; y >= 0; y--)
		{
			writer.flushBuffer();
			for(int x = 0; x < width; x++)
			{
				Pixel& pixel = (*this)[x,y];
				writer.writeData(pixel.b);
				writer.writeData(pixel.g);
				writer.writeData(pixel.r);
			}
			writer.zeroBytes((4-((width * 3)%4))%4);
			writer.flushBuffer();
		}

		writer.close();
	
		return 0;
	};

}
