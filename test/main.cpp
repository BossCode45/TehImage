#include <reader.h>
#include <debug.h>
#include <PNGImage.h>
#include <BMPImage.h>
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
	

	PNGImage png;
	png.readFromFile(infile);
	
	BMPImage bmp(png);
	bmp.writeToFile(outfile);
}
