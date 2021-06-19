#ifndef SMORGASBORD_LOADTEXTURE_HPP
#define SMORGASBORD_LOADTEXTURE_HPP

#include <iostream>
#include <memory>
#include <string>
#include <vector>

namespace Smorgasbord {

class Texture;
class Device;

std::shared_ptr<Texture> LoadTexture(std::shared_ptr<Device> device, std::string filename);

}

#endif // SMORGASBORD_LOADTEXTURE_HPP
