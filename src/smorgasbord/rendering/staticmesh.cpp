#include "staticmesh.hpp"

#include <smorgasbord/util/log.hpp>

void Smorgasbord::MeshData::UpdateStatistics()
{
	polyCount = c.size();
	
	if (polyCount > 0)
	{
		minVerticesPerFace = c[0];
		maxVerticesPerFace = c[0];
		for (size_t i = 1; i < polyCount; i++)
		{
			if (c[i] < minVerticesPerFace)
				minVerticesPerFace = c[i];
				
			if (c[i] > maxVerticesPerFace)
				maxVerticesPerFace = c[i];
		}
	}
	
	vertCount = c.size();
	if (vertCount > 0)
	{
		boundingMin = p[0];
		boundingMax = p[0];
		for (size_t i = 1; i < vertCount; i++)
		{
			boundingMin = min(boundingMin, p[i]);
			boundingMax = max(boundingMax, p[i]);
		}
		
		boundingCenter = (boundingMin + boundingMax) / 2.0f;
		vec3 boundingDiff = boundingMax - boundingMin;
		boundingScale = glm::max(
			glm::max(boundingDiff.x, boundingDiff.y),
			boundingDiff.z);
	}
}

Smorgasbord::StaticMesh::StaticMesh()
{ }

Smorgasbord::StaticMesh::StaticMesh(
	shared_ptr<Smorgasbord::Device> device,
	unique_ptr<Smorgasbord::MeshData> meshData)
{
	MeshData &mesh = *meshData;
	
	if (mesh.minVerticesPerFace < 3)
	{
		LogE("Meshes with points or lines are not supported.");
		return;
	}
	
	if (mesh.minVerticesPerFace != mesh.maxVerticesPerFace)
	{
		LogE("Meshes with mixed vertex count per face are not supported.");
		return;
	}
	
	bool hasNormal = mesh.n.size() > 0;
	bool hasTexCoord = mesh.t.size() > 0;
	
	// Upload buffers
	
	uint32_t polyCount = mesh.polyCount;
	uint32_t vertsPerFace = mesh.minVerticesPerFace;
	
	uint32_t bufferSize = sizeof(vec3) * polyCount * vertsPerFace;
	uint32_t nStartByte = 0;
	uint32_t tcStartByte = 0;
	
	if (hasNormal)
	{
		nStartByte = bufferSize;
		bufferSize += sizeof(vec3) * polyCount * vertsPerFace;
	}
	
	if (hasTexCoord)
	{
		tcStartByte = bufferSize;
		bufferSize += sizeof(vec2) * polyCount * vertsPerFace;
	}
	
	shared_ptr<Buffer> buffer = device->CreateBuffer(
		BufferType::Vertex,
		BufferUsageType::Draw,
		BufferUsageFrequency::Static,
		bufferSize);
	
	{ Scope(buffer, MappedDataAccessType::Write);
		uint8_t* pBase = buffer->GetMappedData();
		
		{
			vec3* p = (vec3*)pBase;
			for (uint32_t i = 0; i < polyCount; i++)
			{
				for (uint32_t j = 0; j < vertsPerFace; j++)
				{
					const uint32_t index = i * vertsPerFace + j;
					p[index] = mesh.p[mesh.fp[index]];
				}
			}
		}
		
		if (hasNormal)
		{
			vec3* n = reinterpret_cast<vec3*>(&pBase[nStartByte]);
			for (uint32_t i = 0; i < polyCount; i++)
			{
				for (uint32_t j = 0; j < vertsPerFace; j++)
				{
					const uint32_t index = i * vertsPerFace + j;
					n[index] = mesh.n[mesh.fn[index]];
				}
			}
		}
		
		if (hasTexCoord)
		{
			vec2* tc = reinterpret_cast<vec2*>(&pBase[tcStartByte]);
			for (uint32_t i = 0; i < polyCount; i++)
			{
				for (uint32_t j = 0; j < vertsPerFace; j++)
				{
					const uint32_t index = i * vertsPerFace + j;
					tc[index] = mesh.t[mesh.ft[index]];
				}
			}
		}
	}
	
	// Set up geometry layout
	
	GeometryLayout geometryLayout;
	geometryLayout.primitiveType =
		vertsPerFace > 3
		? PrimitiveTopology::PatchList
		: PrimitiveTopology::TriangleList;
	geometryLayout.numVertsPerPatch = vertsPerFace;
	
	{
		Attribute attribute;
		attribute.location = 0;
		attribute.name = "v_p";
		attribute.dataType = AttributeDataType::Float;
		attribute.numComponents = 3;
		attribute.accessType = AttributeAccessType::Float;
		attribute.normalize = false;
		attribute.stride = 0;
		attribute.offset = 0;
		geometryLayout.attributes.push_back(attribute);
	}
	
	if (hasNormal)
	{
		Attribute attribute;
		attribute.location = 1;
		attribute.name = "v_n";
		attribute.dataType = AttributeDataType::Float;
		attribute.numComponents = 3;
		attribute.accessType = AttributeAccessType::Float;
		attribute.normalize = false;
		attribute.stride = 0;
		attribute.offset = nStartByte;
		geometryLayout.attributes.push_back(attribute);
	}
	
	if (hasTexCoord)
	{
		Attribute attribute;
		attribute.location = 2;
		attribute.name = "v_uv";
		attribute.dataType = AttributeDataType::Float;
		attribute.numComponents = 2;
		attribute.accessType = AttributeAccessType::Float;
		attribute.normalize = false;
		attribute.stride = 0;
		attribute.offset = tcStartByte;
		geometryLayout.attributes.push_back(attribute);
	}
	
	// Init staticmesh
	
	Init(buffer, { }, polyCount * vertsPerFace, geometryLayout);
}

Smorgasbord::StaticMesh::~StaticMesh()
{ }

void Smorgasbord::StaticMesh::Init(
	shared_ptr<Buffer> vertexBuffer,
	IndexBufferRef indexBuffer,
	uint32_t numVertices,
	GeometryLayout &geometryLayout)
{
	geometry.vertexBuffer = vertexBuffer;
	geometry.indexBuffer = indexBuffer;
	geometry.numVertices = numVertices;
	this->geometryLayout = geometryLayout;
	
	// Can you draw with an index buffer, but without a vertex buffer?
	///if (geometry.vertexBuffer == nullptr)
	///{
	///	LogE("Vertex attribute buffer is empty");
	///	return;
	///}
}
