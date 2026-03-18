#pragma once

#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <string>

#define BUFFER_SIZE 4096

namespace TehImage
{
	enum FileEndianness
	{
		NO_CONVERT,
		LITTLE,
		BIG
	};
	
	class Reader
	{
	public:
		//Bytes are big endian
		Reader(std::string file, FileEndianness fileEndianness);
		~Reader();

		template <typename T>
		T readData();

		char readByte();

		void readBytes(char* out, size_t len);

		void skipBytes(size_t len);

		void close();
	private:
		char buffer[BUFFER_SIZE];
		size_t pos;
		FILE* file;
		bool ready = false;
		FileEndianness fileEndianness;

		void refreshBuffer();
		void convertEndian(uint8_t* out, size_t bytes);
	};

	// template <typename T>
	// T Reader::readData()
	// {
	// 	T num = 0;
	// 	convertEndian((uint8_t*)&num, sizeof(T));
	// }
}
