#pragma once

#include "reader.h"
#include "image.h"
#include "zlib.h"
#include <cstddef>
#include <cstdint>
#include <map>
#include <memory>
#include <string>
#include <vector>

#define CHUNK_READER(X) void X(uint32_t chunkSize)
#define REGISTER_CHUNK_READER(X) chunkReaders.insert({#X, &PNGImage::X})
#define DEFINE_CHUNK_READER(X) void PNGImage::X(uint32_t chunkSize)

namespace TehImage
{

	class PNGImage : public Image
	{
	private:
		ZLibInflator zlib;
		uint8_t* idatData;
		unsigned long idatDataSize;
	public:
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

		bool readNextChunk();
	
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

		bool end = false;

		Reader *reader;
	};

}
