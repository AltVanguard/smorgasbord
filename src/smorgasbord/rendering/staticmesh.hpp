#ifndef SMORGASBORD_STATICMESH_HPP
#define SMORGASBORD_STATICMESH_HPP

#include <smorgasbord/gpu/gpuapi.hpp>

#include <glm/glm.hpp>

#include <memory>

using namespace glm;
using namespace std;

namespace Smorgasbord {

struct MeshData
{
	// Data
	
	vector<vec3> p; // vertex positions
	vector<vec3> n; // normals
	vector<vec2> t; // texture coordinates
	vector<int32_t> c; // face vertex counts
	vector<uint32_t> fp; // face position indices
	vector<uint32_t> fn; // face normal indices
	vector<uint32_t> ft; // face texture coordinate indices
	
	// Statistics
	
	size_t polyCount = 0;
	size_t vertCount = 0;
	int32_t minVerticesPerFace = 0;
	int32_t maxVerticesPerFace = 0;
	// bounding box
	vec3 boundingMin = vec3(0);
	vec3 boundingMax = vec3(0);
	vec3 boundingCenter = vec3(0);
	float boundingScale = 0;
	
	void UpdateStatistics();
	
	bool IsEmpty()
	{
		return polyCount == 0 && vertCount == 0;
	}
};

class StaticMesh
{
private:
	Geometry geometry;
	GeometryLayout geometryLayout;
	
public:
	StaticMesh();
	StaticMesh(shared_ptr<Device> device, unique_ptr<MeshData> meshData);
	~StaticMesh();
	
	void Init(
		shared_ptr<Buffer> vertexBuffer,
		IndexBufferRef indexBuffer,
		uint32_t numVertices,
		GeometryLayout &geometryLayout);
	
	const Geometry &GetGeometry() const
	{
		return geometry;
	}
	
	const GeometryLayout &GetGeometryLayout() const
	{
		return geometryLayout;
	}
};

}

#endif // SMORGASBORD_STATICMESH_HPP
