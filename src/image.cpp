#include "image.h"

Image::~Image()
{
	if(width != 0)
	{
		delete[] imageData;
	}
}
