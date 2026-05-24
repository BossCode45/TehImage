#include "zlib.h"

#include "HashTable.h"

#include <bitset>
#include <cstdint>
#include <cstring>
#include <exception>
#include <iomanip>
#include <iostream>
#include <stdexcept>

#define MAXBITS 15

using std::cout, std::endl;

namespace TehImage
{
	
	const unsigned int lenStart[] = { /* Size base for length codes 257..285 */
		3, 4, 5, 6, 7, 8, 9, 10, 11, 13, 15, 17, 19, 23, 27, 31,
		35, 43, 51, 59, 67, 83, 99, 115, 131, 163, 195, 227, 258};
	const unsigned int lenExtra[] = { /* Extra bits for length codes 257..285 */
		0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2,
		3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 0};

	
	const unsigned int distStart[] = { /* Offset base for distance codes 0..29 */
		1, 2, 3, 4, 5, 7, 9, 13, 17, 25, 33, 49, 65, 97, 129, 193,
		257, 385, 513, 769, 1025, 1537, 2049, 3073, 4097, 6145,
		8193, 12289, 16385, 24577};
	const unsigned int distExtra[] = { /* Extra bits for distance codes 0..29 */
		0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6,
		7, 7, 8, 8, 9, 9, 10, 10, 11, 11,
		12, 12, 13, 13};

	void ZLib::calculateCodes(uint8_t* lengths, uint16_t* codesOut, int codeCount)
	{
		const int biggestLen = 15;

		uint16_t lenCounts[biggestLen + 1];
		memset(lenCounts, 0, (biggestLen + 1)*sizeof(uint16_t));
		for(int i = 0; i < codeCount; i++)
			lenCounts[lengths[i]]++;

	
		lenCounts[0] = 0;
	
		uint16_t nextCodes[biggestLen + 1];
		uint16_t code = 0;
		for(int bits = 1; bits < biggestLen + 1; bits++)
		{
			code = (code + lenCounts[bits - 1]) << 1;
			nextCodes[bits] = code;
		}

		for(int i = 0; i < codeCount; i++)
		{
			uint8_t len = lengths[i];
			if(len == 0)
				continue;
			codesOut[i] = nextCodes[len]++;
		}
	}

	void ZLib::buildHuffmanTree(uint8_t* lengths, uint16_t* codes, int codeCount)
	{
		buildHuffmanTree(lengths, codes, codeCount, &tree);
	}

	void ZLib::buildHuffmanTree(uint8_t* lengths, uint16_t* codes, int codeCount, HuffmanTree* tree)
	{
		for(uint16_t i = 0; i < codeCount; i++)
		{
			uint16_t code = codes[i];
			uint8_t len = lengths[i];
			if(len == 0)
				continue;
			HuffmanTree* current = tree;
			for(int j = 0; j < len; j++)
			{
				bool right = code & (0b1 << (len - 1 - j));
				if(right)
				{
					if(current->right != nullptr)
						current = current->right;
					else
					{
						current->right = new HuffmanTree();
						current = current->right;
					}
				}
				else
				{
					if(current->left != nullptr)
						current = current->left;
					else
					{
						current->left = new HuffmanTree();
						current = current->left;
					}
				}
			}
			current->val = i;
		}
	}

	void ZLib::getStaticHuffmanCodes(uint16_t* codesOut, uint8_t* codeLensOut, uint16_t* distCodesOut, uint8_t* distCodeLensOut)
	{
		
		const int codeCount = 288;
		for(int i = 0; i < codeCount; i++)
		{
			if(i < 144)
				codeLensOut[i] = 8;
			else if(i < 256)
				codeLensOut[i] = 9;
			else if(i < 280)
				codeLensOut[i] = 7;
			else if(i < 288)
				codeLensOut[i] = 8;
		}
		calculateCodes(codeLensOut, codesOut, codeCount);
	
		const uint8_t nDistCodes = 32;

		for(int i = 0; i < nDistCodes; i++)
			distCodeLensOut[i] = 5;

		calculateCodes(distCodeLensOut, distCodesOut, nDistCodes);
	}
	
	void ZLib::buildStaticHuffmanTree()
	{
		if(staticTree)
			return;
		// cout << "Building static tree" << endl;
		buildStaticHuffmanTree(&tree, &distTree);
		staticTree = true;
		haveTree = true;
	}
	void ZLib::buildStaticHuffmanTree(HuffmanTree* treeOut, HuffmanTree* distTreeOut)
	{
		const int codeCount = 288;
		uint8_t lens[codeCount];
		uint16_t codes[codeCount];
	
		const uint8_t nDistCodes = 32;
		uint8_t distCodeLens[nDistCodes];
		uint16_t distCodes[nDistCodes];

		getStaticHuffmanCodes(codes, lens, distCodes, distCodeLens);

		buildHuffmanTree(lens, codes, codeCount, treeOut);
		buildHuffmanTree(distCodeLens, distCodes, nDistCodes, distTreeOut);
	}

	void ZLib::buildDynamicHuffmanTree(StreamData* stream)
	{
		if(haveTree)
		{
			tree.free();
			distTree.free();
		}
		buildDynamicHuffmanTree(stream, &tree, &distTree);
		haveTree = true;
	}
	void ZLib::buildDynamicHuffmanTree(StreamData* stream, HuffmanTree* treeOut, HuffmanTree* distTreeOut)
	{
		unsigned int nLitCodes = nextBits(stream, 5) + 257;
		uint16_t litCodes[nLitCodes];

		unsigned int nDistCodes = nextBits(stream, 5) + 1;
		uint16_t distCodes[nDistCodes];

		uint8_t codeLens[nLitCodes + nDistCodes];
	
		uint8_t nLenCodes = nextBits(stream, 4) + 4;
		uint8_t lenCodeLens[19];
		memset(lenCodeLens,  0, sizeof(uint8_t)*19);
		uint16_t lenCodes[19];

		const static uint8_t lenCodeOrder[] = { 16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15 };
	
		for(int i = 0; i < nLenCodes; i++)
			lenCodeLens[lenCodeOrder[i]] = nextBits(stream, 3);

		calculateCodes(lenCodeLens, lenCodes, 19);
		HuffmanTree lenCodeTree;
		buildHuffmanTree(lenCodeLens, lenCodes, 19, &lenCodeTree);

		int i = 0;
		uint8_t extraBits[] = {2, 3, 7};
		uint8_t repStarts[] = {3, 3, 11};
	
		while(i < nLitCodes + nDistCodes)
		{
			uint16_t code = getNextCode(stream, &lenCodeTree);
			if(code < 16)
				codeLens[i++] = (uint8_t)code;
			else if(code < 19)
			{
				code -= 16;
				int reps = repStarts[code] + nextBits(stream, extraBits[code]);
				uint8_t len = 0;
				if(code == 0)
				{
					if(i == 0)
						cout << "Trying to repeat non existent value in dynamic huffman code creation" << endl;
					else
						len = codeLens[i - 1];
				}
				if(i + reps > nLitCodes + nDistCodes)
					cout << "other big errror oh no " << i << " " << 0+reps << endl;
				for(int j = 0; j < reps; j++)
				{
					codeLens[i++] = len;
				}
			}
			else
			{
				cout << "big error oh no" << endl;
			}
		}

		calculateCodes(codeLens, litCodes, nLitCodes);
		buildHuffmanTree(codeLens, litCodes, nLitCodes, treeOut);

		calculateCodes(&codeLens[nLitCodes], distCodes, nDistCodes);
		buildHuffmanTree(&codeLens[nLitCodes], distCodes, nDistCodes, distTreeOut);
	}

	void HuffmanTree::free()
	{
		if(left != nullptr)
		{
			delete left;
			left = nullptr;
		}
		if(right != nullptr)
		{
			delete right;
			right = nullptr;
		}
		val = 0xFFFF;
	}
	HuffmanTree::~HuffmanTree()
	{
		free();
	}

	uint16_t ZLib::getNextCode(StreamData* stream)
	{
		return getNextCode(stream, &tree);
	}

	uint16_t ZLib::getNextCode(StreamData* stream, HuffmanTree* tree)
	{
		while(tree->val == 0xFFFF)
		{
			bool right = nextBit(stream);

			if(tree->left == nullptr && !right)
				cout << "bad left" << endl;
			if(tree->right == nullptr && right)
				cout << "bad right" << endl;
			tree = (right)?tree->right:tree->left;
		}
		return tree->val;
	}

	bool ZLib::nextBit(StreamData* stream)
	{
		long bit = stream->pos % 8;
		long byte = stream->pos / 8;
		if(byte >= stream->length)
		{
			throw std::out_of_range("Ran out of compressed data");
		}
		stream->pos++;
		//cout <<  ((stream->data[byte] & (0b1 << bit))?"1":"0");
		return (stream->data[byte] & (0b1 << bit))?0b1:0b0;
	}

	uint16_t ZLib::nextBits(StreamData* stream, int count)
	{
		if(count > 16)
			cout << "Too many bits(" << count << ")!!!" << endl;
		uint16_t out = 0;
		for(int i = 0; i < count; i++)
		{
			out |= nextBit(stream) << i;
		}
		return out;
	}

	void ZLib::writeBit(StreamData* stream, bool bit)
	{
		long bitPos = stream->pos % 8;
		long bytePos = stream->pos / 8;
		if(bytePos >= stream->length)
		{
			cout << bytePos << " " << stream->length << endl;
			throw std::out_of_range("Ran out of space");
		}
		stream->pos++;
		if(bit)
			stream->data[bytePos] |= (0b1 << bitPos);
	}

	void ZLib::writeBits(StreamData* stream, int count, uint16_t bits)
	{
		for(int i = 0; i < count; i++)
		{
			writeBit(stream, bits & (0b1 << i));
		}
	}

	void ZLib::writeCode(StreamData* stream, uint16_t code, uint8_t codeLen)
	{
		for(int i = codeLen - 1; i >= 0; i--)
		{
			writeBit(stream, code & (0b1 << i));
		}
	}

	int ZLib::decodeData(uint8_t* data, unsigned long length, uint8_t* out, unsigned long outLength)
	{
		staticTree = false;
	
		StreamData stream = { data, length, 0};
		long outPos = 0;
	
		bool final;
		do
		{
			final = nextBit(&stream);
			CompressionType method = (CompressionType)nextBits(&stream, 2);

			//cout << (final?"Final chunk!\n":"") << "Compression method: " << method << endl;

			//cout << outPos << " " << outLength << endl;
		
			if(method == CompressionType::STATIC_CODES)
				buildStaticHuffmanTree();
			else if(method == CompressionType::DYNAMIC_CODES)
				buildDynamicHuffmanTree(&stream);
			else if(method == CompressionType::NONE)
			{
				while(stream.pos%8 != 0)
					stream.pos++;
				int16_t LEN = nextBits(&stream, 16);
				int16_t NLEN = nextBits(&stream, 16) + 1;
				if(LEN + NLEN != 0)
					throw std::invalid_argument("NLEN and LEN don't match");
				for(int i = 0; i < LEN; i++)
					out[outPos++] = nextBits(&stream, 8);
				continue;
			}
			else
			{
				cout << "Reserved???" << endl;
				return -1;
			}

			uint16_t code;
			do
			{
				code = getNextCode(&stream);
				
				// cout << 'a' << endl;
				if(outPos > outLength && code != 256)
				{
					throw std::out_of_range("No more space left in image (normal)");
				}
				if(code < 256)
					out[outPos++] = (uint8_t)code;
				else if(code > 256)
				{
					// cout << 'b' << endl;
					unsigned int len = lenStart[code-257] + (int)nextBits(&stream, lenExtra[code-257]);
					// cout << 'c' << endl;
					unsigned int distCode = getNextCode(&stream, &distTree);
					// cout << 'd' << endl;
				
					unsigned int dist = distStart[distCode] + (int)nextBits(&stream, distExtra[distCode]);
					// cout << 'e' << endl;
					if(outPos + len > outLength)
					{
						throw std::out_of_range("No more space left in image (RLE error)");
					}
					for(int i = 0; i < len; i++)
					{
						out[outPos] = out[outPos - dist];
						outPos++;
					}
					// cout << 'f' << endl;
				}
				// cout << 'z' << endl;
			}
			while(code != 256);
		}
		while(!final);

		return 0;
	}

	int ZLib::encodeData(uint8_t* data, unsigned long length, uint8_t* out, unsigned long outLength)
	{
		
		StreamData outStream = { out, outLength, 0};
		unsigned long inPos = 0;

		CompressionType compressionType = STATIC_CODES;

		// BFINAL
		writeBit(&outStream, 1);
		// BTYPE
		writeBits(&outStream, 2, compressionType);

		if(compressionType == NONE)
		{
			// Padding for non-compressed
			while(outStream.pos % 8 != 0)
				writeBit(&outStream, 0);
			
			// LEN
			writeBits(&outStream, 16, (uint16_t) length);
			// NLEN
			writeBits(&outStream, 16, ((uint16_t) length) ^ 65535);

			// Write the actual bytes
			while(inPos < length)
			{
				writeBits(&outStream, 8, data[inPos++]);
			}

			// Return length of compressed data in bytes
			return (outStream.pos / 8) + ((outStream.pos % 8 == 0)?0:1);
		}
		if(compressionType == STATIC_CODES)
		{
			uint16_t codes[288];
			uint8_t codeLens[288];
			uint16_t distCodes[32];
			uint8_t distCodeLens[32];
			getStaticHuffmanCodes(codes, codeLens, distCodes, distCodeLens);

			// cout << inPos << " " << length << endl;
			HashTable hashTable(256);
			while(inPos < length)
			{
				if(inPos + 2 >= length)
				{
					writeCode(&outStream, codes[data[inPos]], codeLens[data[inPos]]);
					inPos++;
					continue;
				}
				
				char curr[3];
				memcpy(curr, &(data[inPos]), 3);
				if(hashTable.contains(curr) && inPos - hashTable.get(curr) < 32768)
				{
					unsigned long pos = hashTable.get(curr);
					int length = 3;
					while(data[pos + length] == data[inPos + length]
						  && pos + length < inPos && length < 258)
						length++;
					// length--;

					bool lFound = false, dFound = false;
					uint16_t lenCode, lenExtraBits;
					for(uint16_t i = 0; i < 29; i++)
					{
						if(lenStart[i] <= length && lenStart[i] + (0b1 << lenExtra[i]) - 1 >= length)
						{
							lenCode = i;
							lenExtraBits = length - lenStart[i];
							lFound = true;
							break;
						}
					}
					unsigned long dist = inPos - pos;
					uint16_t distCode, distExtraBits;
					for(uint16_t i = 0; i < 30; i++)
					{
						if(distStart[i] <= dist && distStart[i] + (0b1 << distExtra[i]) - 1 >= dist)
						{
							distCode = i;
							distExtraBits = dist - distStart[i];
							dFound = true;
							break;
						}
					}

					if(!(lFound && dFound))
					{
						cout << ((lFound)?'y':'n') << " " << ((dFound)?'y':'n') << endl;
						cout << length << " " << dist << endl;
						throw std::runtime_error("UH OH");
					}

					writeCode(&outStream, codes[lenCode + 257], codeLens[lenCode + 257]);
					writeBits(&outStream, lenExtra[lenCode], lenExtraBits);
					writeCode(&outStream, distCodes[distCode], distCodeLens[distCode]);
					writeBits(&outStream, distExtra[distCode], distExtraBits);
					inPos += length;
					
				}
				else
				{
					writeCode(&outStream, codes[data[inPos]], codeLens[data[inPos]]);
					hashTable.insertOrUpdate(curr, inPos);
					inPos++;
				}
			}

			writeCode(&outStream, codes[256], codeLens[256]);
			
			return (outStream.pos / 8) + ((outStream.pos % 8 == 0)?0:1);
		}

		// Return length of compressed data in bytes
		return (outStream.pos / 8) + ((outStream.pos % 8 == 0)?0:1);
	}

}
