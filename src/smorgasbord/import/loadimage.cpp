#include "loadimage.hpp"

#include <smorgasbord/image/image.hpp>
#include <smorgasbord/util/log.hpp>

#include <lodepng.h>

#include <sstream>

shared_ptr<Smorgasbord::Image> Smorgasbord::LoadImage(string filename)
{
	return LoadImagePNG(filename);
}

void Smorgasbord::SaveImage(Smorgasbord::Image &image, string filename)
{
	SaveImagePNG(image, filename);
}

shared_ptr<Smorgasbord::Image> Smorgasbord::LoadImagePNG(string filename)
{
	Image* img = new Image();
	
	uint32_t error = lodepng::decode(
		img->data, img->imageSize.x, img->imageSize.y, filename);
	
	if (error == 0)
	{
		img->UpdateCached();
		return shared_ptr<Image>(img);
	}
	else
	{
		LogE("lodepng DEcoder error {0}: {1}",
			error, lodepng_error_text(error));
		return shared_ptr<Image>();
	}
}

void Smorgasbord::SaveImagePNG(Smorgasbord::Image &image, string filename)
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
