#pragma once

#include <cstdint>
#include <string>

#define READER_BUFFER_SIZE 4096
#define WRITER_BUFFER_SIZE 65536

#define DEFINE_INT_READER(TYPE)											\
	template<> TYPE Reader::readData<TYPE>()							\
	{																	\
		TYPE in, out;													\
		readBytes((char*)&in, sizeof(TYPE));							\
		convertEndian((uint8_t*)&in, (uint8_t*)&out, sizeof(TYPE), fileEndianness); \
		return out;														\
	}																	\


#define DEFINE_INT_WRITER(TYPE)											\
	template <> void Writer::writeData(TYPE toWrite)					\
	{																	\
		TYPE converted;													\
		convertEndian((uint8_t*)&toWrite, (uint8_t*)&converted, sizeof(TYPE), fileEndianness); \
		writeBytes((char*)&converted, sizeof(TYPE));					\
	}																	\

namespace TehImage
{
	enum FileEndianness
	{
		NO_CONVERT,
		LITTLE,
		BIG
	};

	void convertEndian(uint8_t* in, uint8_t* out, std::size_t bytes, FileEndianness target);

	class Reader
	{
	public:
		//Bytes are big endian
		Reader(std::string file, FileEndianness fileEndianness);
		~Reader();

		template <typename T>
		T readData();

		char readByte();

		void readBytes(char* out, std::size_t len);

		void skipBytes(std::size_t len);

		void close();
	private:
		char buffer[READER_BUFFER_SIZE];
		std::size_t pos;
		FILE* file;
		bool ready = false;
		FileEndianness fileEndianness;

		void refreshBuffer();
	};

	class Writer
	{
	public:
		//Bytes are big endian
		Writer(std::string filename, FileEndianness fileEndianness);
		~Writer();

		template <typename T>
		void writeData(T toWrite);

		void writeByte(char toWrite);

		void writeBytes(char toWrite[], std::size_t len);

		void zeroBytes(std::size_t len);

		void close();

		void flushBuffer();
		
		char buffer[WRITER_BUFFER_SIZE];
		std::size_t pos;
	private:
		FILE* file;
		bool ready = false;
		FileEndianness fileEndianness;
	};
}
