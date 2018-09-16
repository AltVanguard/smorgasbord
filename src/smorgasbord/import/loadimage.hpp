#ifndef SMORGASBORD_LOADIMAGE_HPP
#define SMORGASBORD_LOADIMAGE_HPP

#include <smorgasbord/image/image.hpp>

#include <iostream>
#include <memory>
#include <string>
#include <vector>

using namespace std;

namespace Smorgasbord {

shared_ptr<Image> LoadImage(string filename);
void SaveImage(Image& image, string filename);

shared_ptr<Image> LoadImagePNG(string filename);
void SaveImagePNG(Image& image, string filename);

}

#endif // SMORGASBORD_LOADIMAGE_HPP
