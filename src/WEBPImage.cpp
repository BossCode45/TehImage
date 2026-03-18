#include "WEBPImage.h"
#include "reader.h"

#define ENABLE_DEBUG
#include "debug.h"

#include <iostream>

using std::cout, std::endl;

namespace TehImage
{
	int WEBPImage::writeToFile(std::string filename)
	{
		cout << "Not implemented" << endl;
		return 1;
	}
	
	int WEBPImage::readFromFile(std::string filename)
	{
		static_assert(std::endian::native == std::endian::little, "Must use little endian");
		reader = std::make_unique<Reader>(filename, FileEndianness::LITTLE);
		
		refreshBitBuffer();

		char RIFF[4], WEBP[4];
		char RIFFExpected[] = {'R','I','F','F'};
		readFourCC(RIFF);
		uint32_t imgSize = readBits(32);
		readFourCC(WEBP);
		char WEBPExpected[] = {'W','E','B','P'};
		if(strncmp(RIFF, RIFFExpected, 4) != 0 || strncmp(WEBP, WEBPExpected, 4) != 0)
		{
			cout << "Not a webp" << endl;
			// cout << '#' << std::hex << RIFF.raw << " #" << RIFFExpected.raw << std::dec << endl;
			// cout << '#' << std::hex << WEBP.raw << " #" << WEBPExpected.raw << std::dec << endl;
			return 1;
		}

		// refreshBitBuffer();

		char VP8L[4];
		readFourCC(VP8L);
		char VP8LExpected[] = {'V', 'P', '8', 'L'};
		uint32_t VP8LSize = readBits(32);
		uint8_t signature = readBits(8);
		if(strncmp(VP8L, VP8LExpected, 4) != 0)
		{
			cout << "VP8L error" << endl;
			return 1;
		}

		uint16_t width = readBits(14) + 1;
		uint16_t height = readBits(14) + 1;

		cout << width << " " << height << endl;

		bool alpha = readBits(1);
		uint8_t versionNumber = readBits(3);

		cout << "Alpha: " << (alpha?"Y":"N") << ", version: " << 0+versionNumber << endl;

		if (readBits(1)) {
			cout << (int) readBits(2) << endl;
		}
		
		cout << "Not finished" << endl;
		return 1;
	}

	bool WEBPImage::readBit()
	{
		bool res = bitBuffer & (1 << (bitBufferPos++));
		if(bitBufferPos >= 8)
			refreshBitBuffer();
		return res?1:0;
	}

	uint32_t WEBPImage::readBits(size_t count)
	{
		uint32_t out = 0;
		for(int i = 0; i < count; i++)
		{
			out |= readBit() << i;
		}
		return out;
	}

	void WEBPImage::readFourCC(char* dest)
	{
		for(int i = 0; i < 4; i++)
		{
			dest[i] = readBits(8);
		}
	}

	void WEBPImage::refreshBitBuffer()
	{
		bitBuffer = reader->readByte();
		bitBufferPos = 0;
	}
}
