#pragma once

#include "image.h"
#include <cstdint>
#include <cstring>

namespace TehImage
{

	class BMPImage : public Image
	{
	private:
	public:
		// template<std::derived_from<Image> T> BMPImage(const T& other) : Image(other) { }
		BMPImage() = default;
		BMPImage(const Image& other) : Image(other) {}
	
		int readFromFile(std::string filename) override;
		int writeToFile(std::string filename) override;
	};

}
