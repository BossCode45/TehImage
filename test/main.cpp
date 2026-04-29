#include <bitset>
#include <reader.h>
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
	

	// TehImage::PNGImage png;
	// png.readFromFile(infile);
	
	// TehImage::BMPImage bmp(png);
	// bmp.writeToFile(outfile);
}
