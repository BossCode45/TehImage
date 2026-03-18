#include <reader.h>
#include <debug.h>
#include <PNGImage.h>
#include <BMPImage.h>
#include <WEBPImage.h>
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
	
	std::string infile = argv[1];
	std::string outfile = argv[2];
	

	TehImage::WEBPImage webp;
	webp.readFromFile(infile);
	
	TehImage::BMPImage bmp(webp);
	bmp.writeToFile(outfile);
}
