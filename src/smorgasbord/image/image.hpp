#pragma once

#include <glm/glm.hpp>

#include <vector>

namespace Smorgasbord {

class Image
{
public:
	std::vector<uint8_t> data;
	// width, height (in pixels)
	glm::uvec2 imageSize;
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
		this->imageSize = glm::uvec2(0, 0);
		this->pixelSize = 4;
		
		UpdateCached();
	}
	
	Image(glm::uvec2 imageSize, uint32_t pixelSize = 4)
	{
		Init(imageSize, pixelSize);
	}
	
public:
	void Init(glm::uvec2 imageSize, uint32_t pixelSize = 4)
	{
		this->imageSize = imageSize;
		this->pixelSize = pixelSize;
		data.resize(imageSize.x * imageSize.y * pixelSize, 127);
		
		UpdateCached();
	}
	
	void UpdateCached()
	{
		dataSize = uint32_t(data.size());
		lineSize = imageSize.x * pixelSize;
		padding = (4 - (lineSize % 4)) % 4;
		stride = lineSize + padding;
	}
};

}
