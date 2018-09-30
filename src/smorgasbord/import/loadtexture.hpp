#ifndef SMORGASBORD_LOADTEXTURE_HPP
#define SMORGASBORD_LOADTEXTURE_HPP

#include <iostream>
#include <memory>
#include <string>
#include <vector>

using namespace std;

namespace Smorgasbord {

class Texture;
class Device;

shared_ptr<Texture> LoadTexture(shared_ptr<Device> device, string filename);

}

#endif // SMORGASBORD_LOADTEXTURE_HPP
