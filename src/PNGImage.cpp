#include "PNGImage.h"
#include "files.h"
#include "zlib.h"
#include "debug.h"

#include <bitset>
#include <cctype>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <endian.h>
#include <iostream>
#include <memory>
#include <stdexcept>

using std::cout, std::endl;

namespace TehImage
{

	PNGImage::PNGImage()
	{
		//cout << "Reader good" << endl;
		REGISTER_CHUNK_READER(IHDR);
		REGISTER_CHUNK_READER(iCCP);
		REGISTER_CHUNK_READER(sRGB);
		REGISTER_CHUNK_READER(eXIf);
		REGISTER_CHUNK_READER(iDOT);
		REGISTER_CHUNK_READER(pHYs);
		REGISTER_CHUNK_READER(tIME);
		REGISTER_CHUNK_READER(tEXt);
		REGISTER_CHUNK_READER(IDAT);
		REGISTER_CHUNK_READER(IEND);

		//cout << "Chunk readers loaded" << endl;


		idatData = nullptr;
		idatDataSize = 0;

		//cout << "PNG image initialised" << endl;
	}
	PNGImage::~PNGImage()
	{
		free(idatData);
		idatData = nullptr;
		idatDataSize = 0;
	}

	int PNGImage::readFromFile(std::string filename)
	{
		std::unique_ptr<Reader> readerMem(new Reader(filename, FileEndianness::BIG));
		reader = readerMem.get();
	
		char signature[8];
		uint8_t expected[] = {137, 80, 78, 71, 13, 10, 26, 10};
		reader->readBytes(signature, 8);
		if(strncmp(signature, (char*)expected, 8) != 0)
		{
			cout << "Not a PNG" << endl;
			return 1;
		}

		while (readNextChunk()) {}
	
		return 0;
	};

	int PNGImage::writeToFile(std::string filename)
	{
		std::unique_ptr<Writer> writerMem(new Writer(filename, FileEndianness::BIG));
		writer = writerMem.get();

		uint8_t signature[] = {137, 80, 78, 71, 13, 10, 26, 10};
		writer->writeBytes((char*)signature, 8);

		writeIHDR();
		writeIDAT();
		writeIEND();

		writer->close();
		
		return 2;
	};

	bool PNGImage::readNextChunk()
	{
		if(end)
			return false;
		uint32_t chunkSize = reader->readData<uint32_t>();
	
		char chunkType[5];
		reader->readBytes(chunkType, 4);
		chunkType[4] = 0;
		std::string chunkName(chunkType, 4);
		cout << "-------------" << endl;
		cout << "|Chunk: " << chunkName << "|" << endl;
		cout << "-------------" << endl;

		if(chunkReaders.count(chunkName) == 0)
		{
			cout << "Chunk reader not found!!! " << chunkName << endl;
			// for(int i = 0; i < 4; i++)
			// 	cout << 0+chunkType[i] << ' ';
			// cout << endl;
			
			reader->skipBytes(chunkSize + 4);
			if(islower(chunkType[0]))
			{
				cout << "\tAble to skip chunk" << endl;
				return true;
			}
			cout << "\tFatal error" << endl;
			return false;
		}

		void(PNGImage::*chunkReader)(uint32_t chunkSize) = chunkReaders.find(chunkName)->second;
		(this->*chunkReader)(chunkSize);

		reader->skipBytes(4); // CRC
		
		return true;
	}

	DEFINE_CHUNK_READER(IHDR)
	{
		width = reader->readData<uint32_t>();
		height = reader->readData<uint32_t>();
		bitDepth = reader->readData<uint8_t>();
		colorType = reader->readData<uint8_t>();
		compressionMethod = reader->readData<uint8_t>();
		filterMethod = reader->readData<uint8_t>();
		interlaceMethod = reader->readData<uint8_t>();
		cout << "Width: " << width << ", Height: " << height << ", Bit depth: " << 0+bitDepth << ", Color type: " << 0+colorType << ", Compression method: " << 0+compressionMethod << ", Filter method: " << 0+filterMethod << ", Interlace method: " << 0+interlaceMethod << endl;

		if(colorType != 2 && colorType != 6)
			throw std::invalid_argument("Only color types 2 and 6 are supported");

		if(bitDepth != 8)
			throw std::invalid_argument("Only bit depth of 8 is supported");

		switch(colorType)
		{
		case 2: colorValues = 3; break;
		case 6: colorValues = 4; break;
		}


		bpp = colorValues * (bitDepth/8);

		// unsigned long imageDataSize = width * height * bpp;
		// imageDataSize = imageDataSize;

		// cout << "Assigning " << imageDataSize << " bytes for image" << endl;

		pixels = std::make_unique<Pixel[]>(width * height);
	}

	DEFINE_CHUNK_READER(iCCP)
	{
		std::string profileName;
		char c = reader->readByte();
		chunkSize--;
		while(c != 0)
		{
			profileName.push_back(c);
			c = reader->readByte();
			chunkSize--;
		}
		// cout << profileName << endl;
		uint8_t compresssionMethod = reader->readByte();
		chunkSize--;
		cout << 0+compresssionMethod << endl;
		uint8_t CMF = reader->readByte();
		uint8_t CM = CMF & 0b00001111;
		uint8_t CINFO = (CMF & 0b11110000) >> 4;
		chunkSize--;
		uint8_t FLG = reader->readByte();
		bool check = (CMF * 256 + FLG)%31 == 0;
		bool FDICT = FLG & 0b00100000;
		uint8_t FLEVEL = FLG & 0b11000000;
		chunkSize--;
		if(CM != 8)
			cout << "Invalid CM: " << 0+CM << endl;
		// cout << 0+CM << ", " << 0+CINFO << ", " << (check?"Valid":"Failed checksum") << ", " << (FDICT?"Dict is present":"No dict present") << ", " << 0+FLEVEL << endl;
		char compressedData[chunkSize - 4];
		reader->readBytes(compressedData, chunkSize - 4);

		char outData[1024];

		ZLib inflator;
		inflator.decodeData((uint8_t*)compressedData, chunkSize - 4, (uint8_t*)outData, 1024);

		cout << "iCCP not supported" << endl;
	
		uint32_t checkValue = reader->readData<uint32_t>();

		//end = true;
	}

	DEFINE_CHUNK_READER(sRGB)
	{
		renderingIntent = reader->readData<uint8_t>();
		cout << "Rendering intent: " << 0+renderingIntent << endl;
	}

	DEFINE_CHUNK_READER(eXIf)
	{
		char endian[4];
		reader->readBytes(endian, 4);
		for(int i = 0; i < 2; i++)
		{
			// cout << endian[i];
		}
		for(int i = 2; i < 4; i++)
		{
			// cout << " " << 0+endian[i];
		}
		// cout << endl;
		char rest[chunkSize - 4];
		reader->readBytes(rest, chunkSize - 4);
		// cout << std::hex;
		for(int i = 0; i < chunkSize - 4; i++)
		{
			// cout << 0+rest[i] << " ";
		}
		// cout << std::dec << endl;
	}

	DEFINE_CHUNK_READER(iDOT)
	{
		cout << "!!! Ignoring iDOT !!!" << endl;
		reader->skipBytes(chunkSize);
	}

	DEFINE_CHUNK_READER(pHYs)
	{
		pixelsPerX = reader->readData<uint32_t>();
		pixelsPerY = reader->readData<uint32_t>();
		unit = reader->readData<uint8_t>();
		cout << "Pixels per unit (x): " << pixelsPerX << ", Pixels per unit (y): " << pixelsPerY << ", unit: " << 0+unit << endl;
	}

	DEFINE_CHUNK_READER(tIME)
	{
		year = reader->readData<uint16_t>();
		month = reader->readData<uint8_t>();
		day = reader->readData<uint8_t>();
		hour = reader->readData<uint8_t>();
		minute = reader->readData<uint8_t>();
		second = reader->readData<uint8_t>();
		cout << "Image last modified: " << 0+hour << ":" << 0+minute << ":" << 0+second << " " << 0+day << "-" << 0+month << "-" << 0+year << endl;
	}

	DEFINE_CHUNK_READER(tEXt)
	{
	
		std::string keyword;
		char c = reader->readByte();
		chunkSize--;
		while(c != 0)
		{
			keyword.push_back(c);
			c = reader->readByte();
			chunkSize--;
		}
		cout << keyword << endl;
		std::string textString;
		c = reader->readByte();
		chunkSize--;
		while(chunkSize > 0)
		{
			textString.push_back(c);
			c = reader->readByte();
			chunkSize--;
		}
		textString.push_back(c);
		cout << textString << endl;
	}

	DEFINE_CHUNK_READER(IDAT)
	{
		if(idatDataSize == 0)
		{
			uint8_t CMF = reader->readByte();
			uint8_t CM = CMF & 0b00001111;
			uint8_t CINFO = (CMF & 0b11110000) >> 4;
			chunkSize--;
			uint8_t FLG = reader->readByte();
			bool check = (CMF * 256 + FLG)%31 == 0;
			bool FDICT = FLG & 0b00000100;
			uint8_t FLEVEL = FLG & 0b00000011;
			chunkSize--;
			if(CM != 8)
				cout << "Invalid CM: " << 0+CM << endl;
			cout << 0+CM << ", " << 0+CINFO << ", " << (check?"Valid":"Failed checksum") << ", " << (FDICT?"Dict is present":"No dict present") << ", " << 0+FLEVEL << endl;
			// cout << 0 + CMF << " " << 0 + FLG << endl;
			idatData = (uint8_t*)malloc(0);
		}

		idatData = (uint8_t *)realloc(idatData, idatDataSize + chunkSize);
		reader->readBytes((char *)&idatData[idatDataSize], chunkSize);
		idatDataSize += chunkSize;

		/*
		  unsigned long compressedSize = chunkSize - 4;
	
		  unsigned long imageDataSize = height * (width * 3 + 1);
		  cout << zlib.decodeData((uint8_t*)compressedData, compressedSize, imageData, imageDataSize) << endl;
		  //cout << (int)puff((unsigned char*)imageData, &imageDataSize, (const unsigned char*)compressedData, &compressedSize) << endl;
		  */
	
		//uint32_t checkValue = reader->readData<uint32_t>();

		//end = true;
	}

	uint8_t paethPredictor(uint8_t a, uint8_t b, uint8_t c)
	{
		int p = a + b - c;
		int pa = abs(p - a);
		int pb = abs(p - b);
		int pc = abs(p - c);
		if (pa <= pb && pa <= pc)
			return a;
		else if (pb <= pc)
			return b;
		else
			return c;
	}

	DEFINE_CHUNK_READER(IEND)
	{
		unsigned long imageDataSize = height * (width * bpp + 1);
		uint8_t* pngImageData = new uint8_t[imageDataSize];
		uint8_t* rawImage = new uint8_t[width * height * bpp];
		cout << "My inflate " << zlib.decodeData(idatData, idatDataSize, pngImageData, imageDataSize) << endl;
		end = true;
		reader->close();

#define imageDataIndex(x, y) rawImage[y*width*bpp + x]
#define pngImageDataIndex(x, y) pngImageData[y*(width*bpp + 1) + x + 1]
#define filterByte(y) pngImageDataIndex(-1, y)

		for(int y = 0; y < height; y++)
		{
			for(int x = 0; x < width*bpp; x++)
			{
				if(filterByte(y) == 0)
				{
					imageDataIndex(x, y) = pngImageDataIndex(x, y);
				}
				else if(filterByte(y) == 1)
				{
					uint8_t sub = pngImageDataIndex(x, y);
					uint8_t raw = (x>=bpp)?imageDataIndex((x-bpp), y):0;
					imageDataIndex(x, y) = sub + raw;
				}
				else if(filterByte(y) == 2)
				{
					uint8_t up = pngImageDataIndex(x, y);
					uint8_t prior = (y>=1)?imageDataIndex(x, (y-1)):0;
					imageDataIndex(x, y) = up + prior;
				}
				else if(filterByte(y) == 3)
				{
					uint8_t avg = pngImageDataIndex(x, y);
					uint8_t a = (x>=bpp)?imageDataIndex((x-bpp), y):0;
					uint8_t b = (y>=1)?imageDataIndex(x, (y-1)):0;
					imageDataIndex(x, y) = avg + std::floor((a + b)/2);
				
				}
				else if(filterByte(y) == 4)
				{
					uint8_t a = (x>=bpp)?imageDataIndex((x-bpp), y):0;
					uint8_t b = (y>=1)?imageDataIndex(x, (y-1)):0;
					uint8_t c = (x>=bpp && y>=1)?imageDataIndex((x-bpp), (y-1)):0;
					uint8_t paeth = pngImageDataIndex(x, y);
					uint8_t predictor = paethPredictor(a, b, c);
					imageDataIndex(x, y) = paeth + predictor;
				}
				else
				{
					cout << "No method for filter type: " << (int)filterByte(y) << ", row: " << y << endl;
					throw std::invalid_argument("Filter type not implemented");
				}
			}
		}

#undef imageDataIndex
#undef pngImageDataIndex
#undef filterByte

		for(int y = 0; y < height; y++)
		{
			for(int x = 0; x < width; x++)
			{
				(*this)[x, y].r = rawImage[(y*width + x)*colorValues];
				(*this)[x, y].g = rawImage[(y*width + x)*colorValues + 1*bitDepth/8];
				(*this)[x, y].b = rawImage[(y*width + x)*colorValues + 2*bitDepth/8];
				(*this)[x, y].a = (colorValues == 4)?rawImage[(y*width + x)*colorValues]:255;
			
			}
		}
	
		delete [] pngImageData;
		delete [] rawImage;
	}

	static uint32_t CRCTable[256];
	static bool CRCComputed = false;

	static void generateCRC()
	{
		for(uint32_t i = 0; i < 256; i++)
		{
			uint32_t c = i;
			for(int bit = 0; bit < 8; bit++)
			{
				if(c & 1)
					c = 0xEDB88320 ^ (c >> 1);
				else
					c >>= 1;
			}
			CRCTable[i] = c;
		}
		CRCComputed = true;
	}

	uint32_t PNGImage::calculateCRC(uint8_t* buffer, std::size_t bufflen)
	{
		uint32_t reg = 0xFFFFFFFF;
		if (!CRCComputed)
			generateCRC();
		for (int i = 0; i < bufflen; i++) {
			reg = CRCTable[(reg ^ buffer[i]) & 0xff] ^ (reg >> 8);
		}
		return reg^0xFFFFFFFF;
	}
	

	DEFINE_CHUNK_WRITER(IHDR)
	{
		uint32_t chunkSize = 13;
		writer->writeData(chunkSize);
		writer->flushBuffer();
		char header[] = "IHDR";
		writer->writeBytes(header, 4);
		
		writer->writeData(width);
		writer->writeData(height);
		writer->writeData<uint8_t>(8); // bpp
		writer->writeData<uint8_t>(ColorTypes::TRUECOLOR | ColorTypes::ALPHA);
		writer->writeData<uint8_t>(0); // Compression method
		writer->writeData<uint8_t>(0); // Filter method
		writer->writeData<uint8_t>(0); // Interlace method

		uint32_t CRC = calculateCRC((uint8_t*)writer->buffer, writer->pos);
		writer->writeData(CRC);
	}

	DEFINE_CHUNK_WRITER(IDAT)
	{
		unsigned long imageDataSize = height * (width * 4 + 1);
		uint8_t* pngImageData = new uint8_t[imageDataSize];

		for(int y = 0; y < height; y++)
		{
			pngImageData[y*(width*4 + 1)] = 0;
			for(int x = 0; x < width; x++)
			{
				pngImageData[y*(width*4 + 1) + 1 + x*4 + 0] = (*this)[x, y].r;
				pngImageData[y*(width*4 + 1) + 1 + x*4 + 1] = (*this)[x, y].g;
				pngImageData[y*(width*4 + 1) + 1 + x*4 + 2] = (*this)[x, y].b;
				pngImageData[y*(width*4 + 1) + 1 + x*4 + 3] = (*this)[x, y].a;
			}
		}

		ZLib encoder;
		uint8_t compressed[WRITER_BUFFER_SIZE];

		size_t encodedSize = encoder.encodeData(pngImageData, imageDataSize, compressed, WRITER_BUFFER_SIZE);

		// uint8_t *test = new uint8_t[imageDataSize];
		// ZLib decoder;
		// size_t decodeSize = decoder.decodeData(compressed, encodedSize, test, imageDataSize);
		// if(decodeSize != imageDataSize)
		// 	cout << "Wrong size" << endl;
		// if(memcmp(pngImageData, test, imageDataSize) != 0)
		// 	cout << "Wrong data" << endl;

		// encoder.decodeData(compressed, encodedSize, pngImageData, imageDataSize);
		
		uint32_t chunkSize = encodedSize + 2;
		
		delete[] pngImageData;
		writer->writeData(chunkSize);
		writer->flushBuffer();
		
		char header[] = "IDAT";
		writer->writeBytes(header, 4);

		writer->writeByte(120);
		writer->writeByte(1);

		writer->writeBytes((char*)compressed, encodedSize);

		// writer->zeroBytes(4);
		
		uint32_t CRC = calculateCRC((uint8_t*)writer->buffer, writer->pos);
		writer->writeData(CRC);
	}

	DEFINE_CHUNK_WRITER(IEND)
	{
		uint32_t chunkSize = 0;
		writer->writeData(chunkSize);
		writer->flushBuffer();
		
		char header[] = "IEND";
		writer->writeBytes(header, 4);

		uint32_t CRC = calculateCRC((uint8_t*)writer->buffer, writer->pos);
		writer->writeData(CRC);
	}
}
