#include "loadtexture.hpp"

#include <smorgasbord/gpu/gpuapi.hpp>
#include <smorgasbord/image/image.hpp>
#include <smorgasbord/import/loadimage.hpp>
#include <smorgasbord/util/log.hpp>

std::shared_ptr<Smorgasbord::Texture> Smorgasbord::LoadTexture(
	std::shared_ptr<Device> device, std::string filename)
{
	std::shared_ptr<Image> img = LoadImage(filename);
	
	if (img)
	{
		std::shared_ptr<Texture> tex =
			device->CreateTexture(
				img->imageSize,
				TextureFormat::RGBA_8_8_8_8_UNorm);
		
		tex->Upload(*img);
		//tex->Verify(*img);
		
		return tex;
	}
	else
	{
		LogE("Could not load image, returning empty texture.");
		return std::shared_ptr<Texture>();
	}
}
