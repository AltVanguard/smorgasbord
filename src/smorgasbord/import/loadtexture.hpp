#ifndef SMORGASBORD_LOADTEXTURE_HPP
#define SMORGASBORD_LOADTEXTURE_HPP

#include <smorgasbord/gpu/graphics.hpp>
#include <smorgasbord/image/image.hpp>

#include <iostream>
#include <string>
#include <vector>

using namespace std;

namespace Smorgasbord {

shared_ptr<Texture> LoadTextureImage(
	shared_ptr<Device> device, string filename);

}

#endif // SMORGASBORD_LOADTEXTURE_HPP
