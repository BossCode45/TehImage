#pragma once

#include "files.h"
#include "image.h"
#include "zlib.h"
#include <cstddef>
#include <cstdint>
#include <map>
#include <memory>
#include <string>
#include <vector>

#define CHUNK_READER(X) void read##X(uint32_t chunkSize)
#define REGISTER_CHUNK_READER(X) chunkReaders.insert({#X, &PNGImage::read##X})
#define DEFINE_CHUNK_READER(X) void PNGImage::read##X(uint32_t chunkSize)

#define CHUNK_WRITER(X) void write##X()
#define DEFINE_CHUNK_WRITER(X) void PNGImage::write##X()



namespace TehImage
{
	class PNGImage : public Image
	{
	public:
		typedef enum
		{
			GRAYSCALE = 0,
			TRUECOLOR = 2,
			INDEXED = 3,
			ALPHA = 4
		} ColorTypes;
		
		PNGImage();
		~PNGImage();

		// template<std::derived_from<Image> T> PNGImage(const T& other) : Image(other) { }
		PNGImage(const Image& other) : Image(other) {}

		int readFromFile(std::string filename) override;
		int writeToFile(std::string filename) override;
	
		// sRGB
		uint8_t renderingIntent;

		// pHYs
		uint32_t pixelsPerX;
		uint32_t pixelsPerY;
		uint8_t unit;

		// tIME
		uint16_t year;
		uint8_t month;
		uint8_t day;
		uint8_t hour;
		uint8_t minute;
		uint8_t second;
	
	private:
		std::map<std::string, void(PNGImage::*)(uint32_t chunkSize)> chunkReaders;
		CHUNK_READER(IHDR);
		CHUNK_READER(iCCP);
		CHUNK_READER(sRGB);
		CHUNK_READER(eXIf);
		CHUNK_READER(iDOT);
		CHUNK_READER(pHYs);
		CHUNK_READER(tIME);
		CHUNK_READER(tEXt);
		CHUNK_READER(IDAT);
		CHUNK_READER(IEND);

		CHUNK_WRITER(IHDR);
		CHUNK_WRITER(IDAT);
		CHUNK_WRITER(IEND);

		

		bool readNextChunk();

		uint32_t calculateCRC(uint8_t *buffer, std::size_t bufflen);

		const uint64_t PNG_CRC = 0x104C11DB7;

		ZLib zlib;
		uint8_t* idatData;
		unsigned long idatDataSize;

		bool end = false;

		Reader *reader;
		Writer *writer;
	};

}
