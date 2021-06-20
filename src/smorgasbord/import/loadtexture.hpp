#pragma once

#include <iostream>
#include <memory>
#include <string>
#include <vector>

namespace Smorgasbord {

class Texture;
class Device;

std::shared_ptr<Texture> LoadTexture(std::shared_ptr<Device> device, std::string filename);

}
