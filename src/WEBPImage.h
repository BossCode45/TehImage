#pragma once

#include "image.h"
#include "reader.h"

#define BIT_BUFFER_SIZE 1024

namespace TehImage
{
	typedef enum {
		PREDICTOR_TRANSFORM             = 0,
		COLOR_TRANSFORM                 = 1,
		SUBTRACT_GREEN_TRANSFORM        = 2,
		COLOR_INDEXING_TRANSFORM        = 3,
	} TransformType;
	
	class WEBPImage : public Image
	{
	public:
		WEBPImage() = default;
		~WEBPImage() = default;
		
		WEBPImage(const Image& other) : Image(other) {}
		
		int readFromFile(std::string filename) override;
		int writeToFile(std::string filename) override;

	private:
		std::unique_ptr<Reader> reader;
		uint8_t bitBuffer;
		size_t bitBufferPos = 0;

		bool readBit();
		uint32_t readBits(size_t count);
		void readFourCC(char* dest);
		void refreshBitBuffer();
	};
}
