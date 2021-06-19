#ifndef SMORGASBORD_LOADIMAGE_HPP
#define SMORGASBORD_LOADIMAGE_HPP

#include <iostream>
#include <memory>
#include <string>
#include <vector>

namespace Smorgasbord {

class Image;

std::shared_ptr<Image> LoadImage(std::string filename);
void SaveImage(Image& image, std::string filename);

std::shared_ptr<Image> LoadImagePNG(std::string filename);
void SaveImagePNG(Image& image, std::string filename);

}

#endif // SMORGASBORD_LOADIMAGE_HPP
