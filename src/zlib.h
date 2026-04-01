#pragma once

#include <cstdint>
#include <list>
#include <string>
#include <vector>

namespace TehImage
{

	struct HuffmanTree
	{
		uint16_t val = 0xFFFF;
		HuffmanTree* left = nullptr; // 0
		HuffmanTree* right = nullptr; // 1
		HuffmanTree() = default;
		~HuffmanTree();
		void free();
	};

	struct StreamData
	{
		uint8_t* data;
		unsigned long length;
		unsigned long pos;
	};

	enum CompressionType : uint16_t
	{
		NONE = 0b00,
		STATIC_CODES = 0b01,
		DYNAMIC_CODES = 0b10,
		RESERVED = 0b11
	};

	// RFC 1950/1951
	class ZLib
	{
	private:
		HuffmanTree tree;
		HuffmanTree distTree;
		bool staticTree = false;
		bool haveTree = false;
	public:
		ZLib() = default;
		~ZLib() = default;

		static bool nextBit(StreamData* stream);
		static uint16_t nextBits(StreamData* stream, int count); // Max 16 bits

		static void writeBit(StreamData* stream, bool bit);
		static void writeBits(StreamData* stream, int count, uint16_t bits);

		void writeCode(StreamData* stream, uint16_t code, uint8_t codeLen);

		void calculateCodes(uint8_t* lengths, uint16_t* codesOut, int codeCount);

		void buildHuffmanTree(uint8_t* lengths, uint16_t* codes, int codeCount);
		void buildHuffmanTree(uint8_t* lengths, uint16_t* codes, int codeCount, HuffmanTree* treeOut);
		
		void getStaticHuffmanCodes(uint16_t* codesOut, uint8_t* codeLensOut, uint16_t* distCodesOut, uint8_t* distCodeLensOut);
		
		void buildStaticHuffmanTree();
		void buildStaticHuffmanTree(HuffmanTree* treeOut, HuffmanTree* distTreeOut);

		void buildDynamicHuffmanTree(StreamData* stream);
		void buildDynamicHuffmanTree(StreamData* stream, HuffmanTree* treeOut, HuffmanTree* distTreeOut);

		uint16_t getNextCode(StreamData* stream);
		uint16_t getNextCode(StreamData* stream, HuffmanTree* tree);

		int decodeData(uint8_t* data, unsigned long length, uint8_t* out, unsigned long outLength);
		int encodeData(uint8_t* data, unsigned long length, uint8_t* out, unsigned long outLength);
	};
	
}
