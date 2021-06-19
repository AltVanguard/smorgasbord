#ifndef LOADOBJ_HPP
#define LOADOBJ_HPP

#include <smorgasbord/rendering/staticmesh.hpp>

#include <memory>
#include <string>

namespace Smorgasbord {

std::unique_ptr<MeshData> LoadOBJ(std::unique_ptr<std::istream> file);

}


#endif // LOADOBJ_HPP
