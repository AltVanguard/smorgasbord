#ifndef SMORGASBORD_IMAGE_HPP
#define SMORGASBORD_IMAGE_HPP

#include <glm/glm.hpp>

#include <vector>

using namespace std;
using namespace glm;

namespace Smorgasbord {

class Image
{
public:
	vector<uint8_t> data;
	// width, height (in pixels)
	uvec2 imageSize;
	// size of the array holding the image data (in bytes)
	uint32_t dataSize;
	// size of one pixel (in bytes)
	uint32_t pixelSize;
	// the jump distance from one line to the next (in bytes)
	uint32_t stride;
	// 4-byte alignment line padding size (in bytes)
	uint32_t padding;
	// distance form begining of a line to the start of the padding (in bytes)
	uint32_t lineSize;
	
public:
	Image()
	{
		this->imageSize = uvec2(0, 0);
		this->pixelSize = 4;
		
		UpdateCached();
	}
	
	Image(uvec2 imageSize, uint32_t pixelSize = 4)
	{
		Init(imageSize, pixelSize);
	}
	
public:
	void Init(uvec2 imageSize, uint32_t pixelSize = 4)
	{
		this->imageSize = imageSize;
		this->pixelSize = pixelSize;
		data.resize(imageSize.x * imageSize.y * pixelSize, 127);
		
		UpdateCached();
	}
	
	void UpdateCached()
	{
		dataSize = data.size();
		lineSize = imageSize.x * pixelSize;
		padding = (4 - (lineSize % 4)) % 4;
		stride = lineSize + padding;
	}
};

}

#endif // SMORGASBORD_IMAGE_HPP
