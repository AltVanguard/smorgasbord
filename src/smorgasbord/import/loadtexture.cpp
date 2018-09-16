#include "loadtexture.hpp"

#include <smorgasbord/util/log.hpp>
#include <smorgasbord/import/loadimage.hpp>

shared_ptr<Smorgasbord::Texture> Smorgasbord::LoadTextureImage(
	shared_ptr<Device> device, string filename)
{
	shared_ptr<Image> img = LoadImage(filename);
	
	if (img)
	{
		shared_ptr<Texture> tex =
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
		return shared_ptr<Texture>();
	}
}
