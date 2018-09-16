#ifndef LOADOBJ_HPP
#define LOADOBJ_HPP

#include <smorgasbord/rendering/staticmesh.hpp>

#include <memory>
#include <string>

using namespace std;

namespace Smorgasbord {

unique_ptr<MeshData> LoadOBJ(unique_ptr<istream> file);

}


#endif // LOADOBJ_HPP
