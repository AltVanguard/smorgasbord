#pragma once

#include <smorgasbord/rendering/staticmesh.hpp>

#include <memory>
#include <string>

namespace Smorgasbord {

std::unique_ptr<MeshData> LoadOBJ(std::unique_ptr<std::istream> file);

}
