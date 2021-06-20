#pragma once

#include <smorgasbord/gpu/gpuapi.hpp>

#include <glm/glm.hpp>

#include <memory>

namespace Smorgasbord {

struct MeshData
{
	// Data
	
	std::vector<glm::vec3> p; // vertex positions
	std::vector<glm::vec3> n; // normals
	std::vector<glm::vec2> t; // texture coordinates
	std::vector<int32_t> c; // face vertex counts
	std::vector<uint32_t> fp; // face position indices
	std::vector<uint32_t> fn; // face normal indices
	std::vector<uint32_t> ft; // face texture coordinate indices
	
	// Statistics
	
	size_t polyCount = 0;
	size_t vertCount = 0;
	int32_t minVerticesPerFace = 0;
	int32_t maxVerticesPerFace = 0;
	// bounding box
	glm::vec3 boundingMin = glm::vec3(0);
	glm::vec3 boundingMax = glm::vec3(0);
	glm::vec3 boundingCenter = glm::vec3(0);
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
	StaticMesh(std::shared_ptr<Device> device, std::unique_ptr<MeshData> meshData);
	~StaticMesh();
	
	void Init(
		std::shared_ptr<Buffer> vertexBuffer,
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
