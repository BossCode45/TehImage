#include "files.h"

#include <bit>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <iostream>


using std::cout, std::endl;

namespace TehImage
{
	// HELPERS
	void convertEndian(uint8_t* in, uint8_t* out, std::size_t bytes, FileEndianness target)
	{
		constexpr bool LE = std::endian::native == std::endian::little;
		constexpr bool BE = !LE;
		if(target == NO_CONVERT || (target == BIG && BE) || (target == LITTLE && LE))
		{
			for(int i = 0; i < bytes; i++)
			{
				out[i] = in[i];
			}
			return;
		}
		// std::cout << "Converting" << std::endl;
		for(int i = 0; i < bytes; i++)
		{
			out[bytes - 1 - i] = in[i];
		}
	}


	// READER
	Reader::Reader(std::string filename, FileEndianness fileEndianness)
		:fileEndianness(fileEndianness)
	{
		file = fopen(filename.c_str(), "rb");
		refreshBuffer();
		ready = true;
	}
	Reader::~Reader()
	{
		if(ready)
			fclose(file);
	}

	char Reader::readByte()
	{
		if(pos == READER_BUFFER_SIZE)
			refreshBuffer();
		return buffer[pos++];
	}

	void Reader::refreshBuffer()
	{
		fread(buffer, sizeof(buffer), 1, file);
		pos = 0;
	}

	template<> uint8_t Reader::readData<uint8_t>()
	{
		return readByte();
	}
	
	DEFINE_INT_READER(uint16_t);
	DEFINE_INT_READER(uint32_t);
	DEFINE_INT_READER(uint64_t);


	void Reader::readBytes(char* out, size_t len)
	{
		while(len > 0)
		{
			size_t bytesToRead = std::min(len, READER_BUFFER_SIZE - pos);
			if(bytesToRead == 0)
			{
				refreshBuffer();
				continue;
			}
			memcpy(out, buffer + pos, bytesToRead);
			out += bytesToRead;
			len -= bytesToRead;
			pos += bytesToRead;
		}
	}

	void Reader::skipBytes(size_t len)
	{
		while(len > 0)
		{
			size_t bytesToRead = std::min(len, READER_BUFFER_SIZE - pos);
			if(bytesToRead == 0)
			{
				refreshBuffer();
				continue;
			}
			len -= bytesToRead;
			pos += bytesToRead;
		}
	}

	void Reader::close()
	{
		fclose(file);
		ready = false;
	}

	
	// WRITER
	Writer::Writer(std::string filename, FileEndianness fileEndianness)
		:fileEndianness(fileEndianness)
	{
		file = fopen(filename.c_str(), "w");
		ready = true;
	}
	
	Writer::~Writer()
	{
		close();
	}

	void Writer::writeByte(char toWrite)
	{
		fwrite(&toWrite, sizeof(char), 1, file);
	}

	void Writer::writeBytes(char toWrite[], std::size_t len)
	{
		// for(int i = 0; i < len; i++)
		// 	cout << toWrite[i] << endl;
		fwrite(toWrite, sizeof(char), len, file);
	}

	void Writer::zeroBytes(std::size_t len)
	{
		char *zeroes = new char[len];
		memset(zeroes, 0, len);
		writeBytes(zeroes, len);
		delete[] zeroes;
	}

	void Writer::close()
	{
		if(ready)
			fclose(file);
		ready = false;
	}

	template <> void Writer::writeData(uint8_t toWrite)
	{
		writeByte(toWrite);
	}

	DEFINE_INT_WRITER(uint16_t);
	DEFINE_INT_WRITER(uint32_t);
	DEFINE_INT_WRITER(uint64_t);
}
