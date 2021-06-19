#include "loadimage.hpp"

#include <smorgasbord/image/image.hpp>
#include <smorgasbord/util/log.hpp>

#include <lodepng/lodepng.h>

#include <sstream>

std::shared_ptr<Smorgasbord::Image> Smorgasbord::LoadImage(std::string filename)
{
	return LoadImagePNG(filename);
}

void Smorgasbord::SaveImage(Smorgasbord::Image &image, std::string filename)
{
	SaveImagePNG(image, filename);
}

std::shared_ptr<Smorgasbord::Image> Smorgasbord::LoadImagePNG(std::string filename)
{
	Image* img = new Image();
	
	uint32_t error = lodepng::decode(
		img->data, img->imageSize.x, img->imageSize.y, filename);
	
	if (error == 0)
	{
		img->UpdateCached();
		return std::shared_ptr<Image>(img);
	}
	else
	{
		LogE("lodepng DEcoder error {0}: {1}",
			error, lodepng_error_text(error));
		return std::shared_ptr<Image>();
	}
}

void Smorgasbord::SaveImagePNG(Smorgasbord::Image &image, std::string filename)
{
	if (image.data.size() == 0)
	{
		LogE("Empty image, cannot save");
		return;
	}
	
	unsigned int error = lodepng::encode(
		filename, image.data, image.imageSize.x, image.imageSize.y);
	
	if (error != 0)
	{
		LogE("lodepng ENcoder error {0}: {1}",
			error, lodepng_error_text(error));
	}
}
