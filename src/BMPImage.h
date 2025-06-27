#pragma once

#include "image.h"
#include <cstdint>
#include <cstring>

class BMPImage : public Image
{
private:
public:
	template<std::derived_from<Image> T> BMPImage(const T& other) : Image(other) { }
	
	int readFromFile(std::string filename) override;
	int writeToFile(std::string filename) override;
};
