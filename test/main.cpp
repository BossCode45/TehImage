#include <bitset>
#include <debug.h>
#include <PNGImage.h>
#include <BMPImage.h>
#include <HashTable.h>
#include <zlib.h>
#include <cstdint>
#include <cstring>
#include <iostream>

using std::cout, std::endl;

int main(int argc, char* argv[])
{
	// uint8_t poly = 0b10011;
	// uint32_t data = 0b11010110110000;
	// uint32_t ones = 0b11111;
	// char dashes[15];
	// memset(dashes, '-', 14);
	// dashes[14] = 0;
	// for(int i = 9; i >= 0; i--)
	// {
	// 	uint32_t shiftedPoly = poly << i;
	// 	cout << std::bitset<14>(data) << endl << std::bitset<14>(shiftedPoly) << endl << dashes << endl;
	// 	bool greater = false;
	// 	for(int j = 4; j >= 0; j++)
	// 	{
	// 		if(data & (0b1 << (j+i)))
	// 		{
	// 			greater = true;
	// 			break;
	// 		}
	// 		else if (shiftedPoly & (0b1 << (j+i)))
	// 			break;
	// 	}
	// 	if(greater)
	// 		data ^= shiftedPoly;
	// }

	// cout << std::bitset<5>(data) << endl;
	
	// return 0;

	
	if(argc < 3)
	{
		cout << "usage: " << argv[0] << " <in file> <out file>" << endl;
		return 1;
	}

	// TehImage::ZLib encoder;
	// TehImage::ZLib decoder;

	// const int inSize = 1000;
	// uint8_t in[inSize];
	// for(int i = 0; i < 10; i++)
	// {
	// 	for(int j = 0; j < 100; j++)
	// 	{
	// 		in[i*100+j] = i;
	// 	}
	// }
	
	// const int encodedSize = 1024;
	// uint8_t encoded[encodedSize];
	
	// int compressedSize = encoder.encodeData(in, inSize, encoded, encodedSize);
	
	// // for(int i = 0; i < compressedSize; i++)
	// // {
	// // 	cout << std::bitset<8>(encoded[i]) << endl;
	// // }

	// uint8_t out[inSize];
	// int decodedSize = decoder.decodeData(encoded, encodedSize, out, inSize);
	// cout << ((memcmp(in, out, inSize) == 0)?"Pass":"Fail") << ", Size: " << compressedSize << endl;

	// // for(int i = 0; i < inSize; i++)
	// // {
	// // 	cout << 0 + in[i] << " " <<  0 + out[i] << endl;
	// // }

	// return 0;
	
	std::string infile = argv[1];
	std::string outfile = argv[2];

	TehImage::BMPImage bmp;
	bmp.readFromFile(infile);

	TehImage::PNGImage png(bmp);
	png.writeToFile(outfile);
	

	// TehImage::PNGImage png;
	// png.readFromFile(infile);
	
	// TehImage::BMPImage bmp(png);
	// bmp.writeToFile(outfile);
}
