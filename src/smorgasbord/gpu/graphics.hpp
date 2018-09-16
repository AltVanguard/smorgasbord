#ifndef SMORGASBORD_GPU_GRAPHICS_HPP
#define SMORGASBORD_GPU_GRAPHICS_HPP

#include <smorgasbord/image/image.hpp>
#include <smorgasbord/util/log.hpp>
#include <smorgasbord/util/resourcemanager.hpp>
#include <smorgasbord/util/scope.hpp>

#include <glm/glm.hpp>

#include <algorithm>
#include <map>
#include <memory>
#include <sstream>
#include <vector>

/*

# Notes #

 - Avoid mat3 in ParameterBuffers: mat3 must be uploaded as a mat4 according
	to the OpenGL std140​ layout specification, so it will ruin field
	alignment and you won't even be able to read the mat3 from the shader
	properly


# Shader file description #

Sample file structure:

	##stage vertex
	##output o {
		vec3 pos;
	}
	
	void main()
	{
		o.pos = ...;
		...
	}
	
	##stage fragment
	##include "util.glsl"
	##input a
	
	void main()
	{
		... = util_function(a.pos);
	}

Remarks:

- Stage may take one of the following values (all lowercase): vertex,
	tesselation_control/domain, tesselation_evaluation/hull, geometry,
	fragment
- Compute shader cannot be used in combination with any other shader stage.
	##stage doesn't accept "compute" for this reason
- The stages should be ordered according to the they are executed in the
	pipeline
- The file might contain only a single section targeting a specific stage
- Inclusion uses relative file names. Quotes are mandatory. Angle brackets
	not supported
- Specifying the name for ##output is optional. Defaults to "o", short for
	Output.
- Specifying the ##input directive is optional. Defaults to "a", short for
	Argument ("i" for Input would be less convenient because it's common
	use of a loop variable).

Warning:

- The current parser implementation is quite rigid. Avoid custom formatting
	when using ## directives!
- Guidelines: single space between directive and value, no trailing
	whitespace, filenames with spaces are allowed (quotes required either
	way)

# Design principles and considerations #

## ParameterBuffer

Meant to be a simple way to upload sets of flat parameters, not chunks of
data (like arrays of structs). Also meant to be easily regroupable, so you
could move a group of variables into or out of push constants with minimal
code change. The intent is to preserve the advantage that the API versioning
internally, as if the data a root/push constant.

ParameterBuffer also has the advantage of have information about its
individual members, so it can alias members. So you don't need to
differentiate variables on the shader side.

Larger chunks of data should use SMORGASBORD_CONSTANT_BUFFER_LAYOUT(). This
option lacks automatic versioning, and overall needs more explicit data
allocation, upload, etc.

*/

/// Current API limitations (need some sort of API rework to implement):
/// [easy] glDrawElementsInstancedBaseVertex() support
/// [easy] Vulkan: Multiple bound vertex buffers. Let CommandBuffer::Draw()
///		provide multiple vertex buffers at various binding points. This
///		also need changes in PipelineState::vertexAttributeLayout
/// [hard] Vulkan: Subpass support
/// [hard] Framebuffer: Support texture formats other than a single mip
///		level 2D texture
/// [?] OpenGL: Support for glVertexAttribDivisor() functionality. Not
///		interested enaugh for now to read what it is/does

/// TODO: Compute shaders
/// TODO: query maxColorAttachments
/// TODO?: making buffers typesafe e.g. GlobalBuffer, ConstantBuffer. Probably
///		difficult and pointless as many of the buffers can be bound
///		interchangeably
/// TODO?: dummy field to skip an attachment index in a Pass.
///		e.g. SMORGASBORD_SKIP_ATTACHMENT(), and enumerator.Skip()
/// TODO: SetSource(string): handling includes through ResourceManager is not
///		trivial, but still would be nice. SetSource needs access to a
///		ResourceManager and some form of ResourceReference to resolve
///		relative include path. Maybe ResourceReference could be expanded
///		to directory references, or could have a reference to a
///		nonexistent file in the desired base folder
/// TODO: ##attribute_prefix directive. You could specify a prefix other
///		than "v_"
/// TODO: implement ##input directive. It doesn't have a block, only
///		specifies the instance name of the input block
/// TODO: GraphicsShader::SetName()
/// TODO: PrimitiveTopology: Support for *Adjacency primitive topologies.
///		Only used in geometry shaders, otherwise ignored

#define SMORGASBORD_CONTANTBUFFER(name, instance, body) \
	struct name { \
		body \
		string GetInterface() \
		{ \
			return "struct " #name "{\n" #body "}" #instance ";"; \
		} \
	};

#define SMORGASBORD_FIELD(type, name, ...) \
	type name = enumerator.Add<type>(&name, #type, #name, #__VA_ARGS__)
#define SMORGASBORD_FIELD_ARRAY(type, arraySize, name, ...) \
	std::array<type, arraySize> name = enumerator.AddArray<type, arraySize>( \
		&name, #type, #name, #__VA_ARGS__)
#define SMORGASBORD_SAMPLER(type, name, ...) \
	TextureSampler name = enumerator.Add(&name, #type, #name, #__VA_ARGS__)
#define SMORGASBORD_COLOR_ATTACHMENT(type, name, ...) \
	ColorAttachment name = enumerator.Add(&name, #type, #name, #__VA_ARGS__)
#define SMORGASBORD_DEPTH_ATTACHMENT(type, name, ...) \
	DepthAttachment name = enumerator.Add(&name, #type, #name, #__VA_ARGS__)

using namespace glm;
using namespace std;

namespace Smorgasbord {

enum class BufferType
{
	Global = 0,
	Constant,
	Vertex,
	Index
	// TODO?: transform feedback, copy read/write, pixel pack/unpack,
	//	buffer texture (probably doesn't need this last one)
};

enum class BufferUsageType
{
	/// The host will be writing data to the buffer, but the host
	/// will not read it back
	Draw = 0,
	/// The host will not be writing data, but the host
	/// will be reading it back
	Read,
	/// The host will be neither writing nor reading the data.
	/// All access happens within the device
	Copy
};

enum class BufferUsageFrequency
{
	/// The host will set the data once
	Static = 0,
	/// The host will set the data occasionally
	Dynamic,
	/// The host will be changing the data after every use
	/// or almost every use
	Stream
};

enum class MappedDataAccessType
{
	Read = 0,
	Write,
	ReadWrite
};

enum class IndexDataType
{
	UInt8 = 0,
	UInt16,
	UInt32
};

enum class AttributeDataType
{
	Float = 0,
	Double,
	Int8,
	Int16,
	Int32,
	UInt8,
	UInt16,
	UInt32
	
	// TODO?:
	// GL_HALF_FLOAT, GL_FIXED, GL_INT_2_10_10_10_REV,
	// GL_UNSIGNED_INT_2_10_10_10_REV and GL_UNSIGNED_INT_10F_11F_11F_REV
};

enum class AttributeAccessType
{
	Float = 0,
	Double = 1,
	Int = 2
};

enum class PrimitiveTopology
{
	PointList = 0,
	LineList,
	LineStrip,
	LineStripWithReset,
	TriangleList,
	TriangleStrip,
	TriangleStripWithReset,
	TriangleFan,
	TriangleFanWithReset,
	PatchList
};

enum class ShaderStage : uint32_t
{
	Vertex = 0,
	/// Domain shader in DX terminology
	TesselationControl,
	/// Hull shader in DX terminology
	TesselationEvaluation,
	Geometry,
	/// Pixel shader in DX terminology
	Fragment,
	Num
	
	/// Compute shader is not included because it cannot be
	/// in the same pipeline with the rest
};

enum class ShaderStageFlag : uint32_t
{
	None = 0,
	Vertex = (1 << (uint32_t)ShaderStage::Vertex),
	TesselationControl = (1 << (uint32_t)ShaderStage::TesselationControl),
	TesselationEvaluation =
		(1 << (uint32_t)ShaderStage::TesselationEvaluation),
	Geometry = (1 << (uint32_t)ShaderStage::Geometry),
	Fragment = (1 << (uint32_t)ShaderStage::Fragment),
	All = Vertex
		| TesselationControl
		| TesselationEvaluation
		| Geometry
		| Fragment
};

inline ShaderStageFlag operator|(ShaderStageFlag a, ShaderStageFlag b)
{
	return static_cast<ShaderStageFlag>(
		static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
}

inline ShaderStageFlag operator|=(ShaderStageFlag &a, ShaderStageFlag b)
{
	a = static_cast<ShaderStageFlag>(
		static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
	return a;
}

inline ShaderStageFlag operator&(ShaderStageFlag a, ShaderStageFlag b)
{
	return static_cast<ShaderStageFlag>(
		static_cast<uint32_t>(a) & static_cast<uint32_t>(b));
}

inline ShaderStageFlag operator&=(ShaderStageFlag &a, ShaderStageFlag b)
{
	a = static_cast<ShaderStageFlag>(
		static_cast<uint32_t>(a) & static_cast<uint32_t>(b));
	return a;
}

inline bool operator>(ShaderStageFlag a, uint32_t b)
{
	return static_cast<uint32_t>(a) > b;
}

enum class LoadOp
{
	None = 0,
	Load,
	Clear
};

enum class StoreOp
{
	None = 0,
	Store
};

/// <channels>_<bit_layout_per_channel>_<access_type>
/// <channels>: one or more of R, G, B, A, Depth, Stencil
/// <access_type>: sRGB, UNorm, ...
///		sRGB: accessed as float [0,1], A is linear, RGB are in gamma space
///		UNorm: the integer value range ir remapped to a [0,1] float value
enum class TextureFormat
{
	RGBA_8_8_8_8_UNorm = 0,
	R_16_UNorm,
	Depth_24_UNorm,
	Depth_32_UNorm,
	// TODO: more values
};

enum class SetOp
{
	// Only upload if invalidated earlier
	None = 0,
	// Upload even if wasn't separately invalidated
	Invalidate,
	// Upload as push/root constants
	Constants
};

enum class SamplerFilter
{
	Nearest = 0,
	Linear
};

enum class SamplerWrap
{
	Clamp = 0,
	Repeat,
	MirroredRepeat
};

enum class VariableBaseType
{
	Float = 0,
	Int,
	UInt,
	Matrix
};

struct VariableType
{
	VariableBaseType baseType = VariableBaseType::Float;
	uint32_t numRows = 0;
	uint32_t numColumns = 0;
	
	VariableType()
	{ }
	
	VariableType(
		VariableBaseType _baseType,
		uint32_t numComponents)
		: baseType(_baseType), numRows(numComponents), numColumns(1)
	{ }
	
	VariableType(
		VariableBaseType _baseType,
		 uint32_t _numRows,
		 uint32_t _numColumns)
		: baseType(_baseType), numRows(_numRows), numColumns(_numColumns)
	{ }
};

inline uint32_t GetIndexDataTypeSize(IndexDataType type)
{
	switch (type)
	{
	case IndexDataType::UInt8:
		return 1;
	case IndexDataType::UInt16:
		return 2;
	case IndexDataType::UInt32:
		return 4;
	}
	
	return 0;
}

inline ShaderStageFlag ParseShaderStageFlags(string stageFlagsString)
{
	ShaderStageFlag stageFlags = ShaderStageFlag::None;
	
	const size_t numChars = stageFlagsString.length();
	for (size_t i = 0; i < numChars; i++)
	{
		switch (stageFlagsString[i])
		{
		case 'a':
			return ShaderStageFlag::All;
		case 'v':
			stageFlags = stageFlags | ShaderStageFlag::Vertex;
			break;
		case 'c':
		case 'd':
			stageFlags = stageFlags | ShaderStageFlag::TesselationControl;
			break;
		case 'e':
		case 'h':
			stageFlags = stageFlags | ShaderStageFlag::TesselationEvaluation;
			break;
		case 'g':
			stageFlags = stageFlags | ShaderStageFlag::Geometry;
			break;
		case 'f':
			stageFlags = stageFlags | ShaderStageFlag::Fragment;
			break;
		
		default:
			LogE("Not recognised stage flag");
			return ShaderStageFlag::None;
		}
	}
	
	return stageFlags;
}

inline SamplerFilter ParseSamplerFilter(char c)
{
	switch (c)
	{
	case 'n':
		return SamplerFilter::Nearest;
	case 'l':
		return SamplerFilter::Linear;
	
	default:
		return SamplerFilter::Nearest;
	}
}

inline SamplerWrap ParseSamplerWrap(char c)
{
	switch (c)
	{
	case 'c':
		return SamplerWrap::Clamp;
	case 'r':
		return SamplerWrap::Repeat;
	case 'm':
		return SamplerWrap::MirroredRepeat;
	
	default:
		return SamplerWrap::Clamp;
	}
}

inline ShaderStageFlag StageToFlag(ShaderStage stage)
{
	return (ShaderStageFlag)(1u << (uint32_t)stage);
}

inline string StageToString(ShaderStage stage)
{
	switch (stage)
	{
	case ShaderStage::Vertex:
		return "vertex";
	case ShaderStage::TesselationControl:
		return "tesselation control";
	case ShaderStage::TesselationEvaluation:
		return "tesselation evaluation";
	case ShaderStage::Geometry:
		return "geometry";
	case ShaderStage::Fragment:
		return "fragment";
	case ShaderStage::Num:
		LogF("Invalid shader stage");
		return "num";
	}
}

class Buffer
{
protected:
	BufferType bufferType;
	BufferUsageType accessType;
	BufferUsageFrequency accessFrequency;
	uint32_t size; // in bytes
	
public:
	Buffer(
		BufferType bufferType, 
		BufferUsageType accessType, 
		BufferUsageFrequency accessFrequency, 
		int size);
	virtual ~Buffer() { }
	
public:
	virtual uint8_t *GetMappedData() = 0;
	virtual void Map(MappedDataAccessType mapAccessType) = 0;
	virtual void Unmap() = 0;
	
	uint32_t GetSize() const
	{
		return size;
	}
	
	BufferType GetBufferType() const
	{
		return bufferType;
	}
	
	unique_ptr<IScope> GetScope(MappedDataAccessType mapAccessType)
	{
		Map(mapAccessType);
		return SMORGASBORD_CREATESCOPE(MapScope, Buffer);
	}
};

struct IndexBufferRef
{
	shared_ptr<Buffer> buffer;
	IndexDataType dataType = IndexDataType::UInt8;
	
	IndexBufferRef()
	{ }
	
	IndexBufferRef(shared_ptr<Buffer> _buffer, IndexDataType _dataType)
		: buffer(_buffer), dataType(_dataType)
	{ }
	
	bool IsValid()
	{
		return buffer != nullptr;
	}
};

struct ParameterBufferField
{
	void *p = nullptr;
	uint32_t size = 0; // size of an array element in bytes
	string type;
	string name;
	uint32_t arraySize = 0; // 0: not an array
	string modifier;
	
	ParameterBufferField(
		void *_p,
		uint32_t _size,
		const char *_type,
		const char *_name,
		uint32_t _arraySize,
		const char *_modifier = "")
		: p(_p)
		, size(_size)
		, type(_type)
		, name(_name)
		, arraySize(_arraySize)
		, modifier(_modifier)
	{ }
};

class ParameterBufferFieldEnumerator
{
private:
	vector<ParameterBufferField> fields;
	string modifier;
	uint32_t bufferSize = 0;
	
public:
	ParameterBufferFieldEnumerator()
	{ }
	
	ParameterBufferFieldEnumerator(const char *modifier)
	{
		this->modifier = modifier;
	}
	
	const string &GetModifier() const
	{
		return modifier;
	}
	
	template <typename T>
	T& Add(
		T *p, const char *type, const char *name, const char *modifier = "")
	{
		/// Avoid mat3 in ParameterBuffers: mat3 must be uploaded as a mat4
		/// according to the OpenGL std140​ layout specification, so it will
		/// ruin field alignment and you won't even be able to read the
		/// mat3 from the shader properly
		AssertE(
			string(type) != "mat3",
			"Look at that! A mat3 in a ParameterBuffer? That won't do!");
		
		const size_t size = sizeof(T);
		fields.emplace_back(
			p, (uint32_t)size, type, name, 0, modifier);
		
		// Cache bufferSize
		const size_t min = (size_t)fields[0].p;
		size_t end = (size_t)p + (size_t)size;
		bufferSize = (uint32_t)(end - min);
		
		*p = { };
		return *p;
	}
	
	template <typename T, uint32_t arraySize>
	std::array<T, arraySize>& AddArray(
		std::array<T, arraySize> *p,
		const char *type,
		const char *name,
		const char *modifier = "")
	{
		/// Avoid mat3 in ParameterBuffers: mat3 must be uploaded as a mat4
		/// according to the OpenGL std140​ layout specification, so it will
		/// ruin field alignment and you won't even be able to read the
		/// mat3 from the shader properly
		AssertE(
			string(type) != "mat3",
			"Look at that! A mat3 in a ParameterBuffer? That won't do!");
		
		const size_t size = sizeof(T);
		fields.emplace_back(
			p, (uint32_t)size, type, name, arraySize, modifier);
		
		// Cache bufferSize
		const size_t min = (size_t)fields[0].p;
		size_t end = (size_t)p + (size_t)size;
		bufferSize = (uint32_t)(end - min);
		
		*p = { };
		return *p;
	}
	
	const vector<ParameterBufferField> &GetFields() const
	{
		return fields;
	}
	
	uint32_t GetBufferSize() const
	{
		return bufferSize;
	}
};

class ParameterBuffer
{
protected:
	ParameterBufferFieldEnumerator enumerator;
	shared_ptr<Buffer> gpuBuffer;
	bool isValid = false;
	
public:
	ParameterBuffer()
	{ }
	
	ParameterBuffer(const char* modifier)
		: enumerator(modifier)
	{ }
	
	virtual ~ParameterBuffer() { };
	
	void SetBuffer(shared_ptr<Buffer> buffer)
	{
		AssertF(buffer != nullptr, "Given buffer isn't valid");
		AssertF(buffer->GetSize() >= GetBufferSize(),
			"Given buffer isn't large enough");
		
		gpuBuffer = buffer;
	}
	
	shared_ptr<Buffer> GetBuffer() const
	{
		return gpuBuffer;
	}
	
	void Invalidate()
	{
		isValid = false;
	}
	
	const string &GetModifier() const
	{
		return enumerator.GetModifier();
	}
	
	const vector<ParameterBufferField> GetFields() const
	{
		return enumerator.GetFields();
	}
	
	uint32_t GetBufferSize() const
	{
		return enumerator.GetBufferSize();
	}
};

class Texture
{
protected:
	uvec2 size = vec2(0, 0);
	TextureFormat format = TextureFormat::RGBA_8_8_8_8_UNorm;
	
public:
	Texture(uvec2 imageSize, TextureFormat textureFormat);
	virtual ~Texture() { }
	
	virtual void Upload(Image &image) = 0;
	virtual void Verify(Image &image) = 0;
	virtual shared_ptr<Image> Download() = 0;
	
	uvec2 GetSize()
	{
		return size;
	}
	
	TextureFormat GetFormat()
	{
		return format;
	}
};

class TextureSampler
{
public:
	string type;
	string name;
	string modifier;
	shared_ptr<Texture> texture;
	
	SamplerFilter minify = SamplerFilter::Nearest;
	SamplerFilter magnify = SamplerFilter::Nearest;
	SamplerWrap s = SamplerWrap::Clamp;
	SamplerWrap t = SamplerWrap::Clamp;
	SamplerWrap r = SamplerWrap::Clamp;
	
public:
	TextureSampler(
		const char *_type,
		const char *_name,
		const char *_modifier)
	{
		type = _type;
		name = _name;
		modifier = _modifier;
		
		ParseFilter(modifier);
	}
	
	void ParseFilter(const string &text);
	
	ShaderStageFlag GetStageMask()
	{
		// TODO: parse like ParseFilter()
		return ShaderStageFlag::All;
	}
	
	TextureSampler& operator= (shared_ptr<Texture> texture)
	{
		this->texture = texture;
		return *this;
	}
};

class TextureSamplerSetFieldEnumerator
{
	vector<TextureSampler*> samplers;
	
public:
	TextureSampler Add(
		TextureSampler *p,
		const char *type,
		const char *name,
		const char *modifier = "")
	{
		samplers.emplace_back(p);
		return { type, name, modifier };
	}
	
	const vector<TextureSampler*> &GetSamplers() const
	{
		return samplers;
	}
};

class TextureSamplerSet
{
protected:
	TextureSamplerSetFieldEnumerator enumerator;
	
public:
	virtual ~TextureSamplerSet() { }
	
	const vector<TextureSampler*> &GetSamplers() const
	{
		return enumerator.GetSamplers();
	}
};

class Attachment
{
protected:
	string type;
	string name;
	string modifier;
	
	LoadOp loadOp = LoadOp::None;
	StoreOp storeOp = StoreOp::None;
	
public:
	Attachment(
		const char *_type,
		const char *_name,
		const char *_modifier)
		: type(_type), name(_name), modifier(_modifier)
	{ }
	
	void SetToLoad()
	{
		this->loadOp = LoadOp::Load;
	}
	
	void SetToStore()
	{
		this->storeOp = StoreOp::Store;
	}
	
	string GetType() const
	{
		return type;
	}
	
	string GetName() const
	{
		return name;
	}
	
	string GetModifier() const
	{
		return modifier;
	}
	
	LoadOp GetLoadOp() const
	{
		return loadOp;
	}
	
	StoreOp GetStoreOp() const
	{
		return storeOp;
	}
	
	bool operator!=(const Attachment &b) const
	{
		return loadOp != b.loadOp
			|| storeOp != b.storeOp
			|| type != b.type
			|| name != b.name
			|| modifier != b.modifier;
	}
};

class ColorAttachment : public Attachment
{
private:
	uint32_t index = 0;
	
	vec4 clearColor = vec4(0,0,0,0);
	
public:
	ColorAttachment(
		uint32_t _index,
		const char *_type,
		const char *_name,
		const char *_modifier = "")
		: Attachment(_type, _name, _modifier), index(_index)
	{ }
	
	void SetToClear(vec4 clearColor = vec4(0,0,0,0))
	{
		this->loadOp = LoadOp::Clear;
		this->clearColor = clearColor;
	}
	
	uint32_t GetIndex() const
	{
		return index;
	}
	
	vec4 GetClearColor() const
	{
		return clearColor;
	}
	
	bool operator!=(const ColorAttachment &b) const
	{
		return Attachment::operator!=(b)
			|| clearColor != b.clearColor;
	}
};

class DepthAttachment : public Attachment
{
private:
	float depthClearValue = 0;
	float stencilClearValue = 0;
	
public:
	DepthAttachment(
		const char *_type,
		const char *_name,
		const char *_modifier = "")
		: Attachment(_type, _name, _modifier)
	{ }
	
	void SetToClear(
		float _depthClearValue = 1.0f, float _stencilClearValue = 1.0f)
	{
		this->loadOp = LoadOp::Clear;
		depthClearValue = _depthClearValue;
		stencilClearValue = _stencilClearValue;
	}
	
	float GetDepthClearValue() const
	{
		return depthClearValue;
	}
	
	float GetStencilClearValue() const
	{
		return stencilClearValue;
	}
	
	bool operator!=(const DepthAttachment &b) const
	{
		return Attachment::operator!=(b)
			|| stencilClearValue != b.stencilClearValue;
	}
};

class AttachmentEnumerator
{
private:
	uint32_t nextIndex = 0;
	vector<ColorAttachment*> colorAttachments;
	DepthAttachment *depthAttachment = nullptr;
	
public:
	ColorAttachment Add(
		ColorAttachment *p,
		const char *type,
		const char *name,
		const char *modifier = "")
	{
		uint32_t index = nextIndex;
		nextIndex++;
		colorAttachments.emplace_back(p);
		return { index, type, name, modifier };
	}
	
	DepthAttachment Add(
		DepthAttachment *p,
		const char *type,
		const char *name,
		const char *modifier = "")
	{
		// TODO: error check
		depthAttachment = p;
		return { type, name, modifier };
	}
	
	const vector<ColorAttachment*> &GetColorAttachments() const
	{
		return colorAttachments;
	}
	
	DepthAttachment *GetDepthAttachment() const
	{
		return depthAttachment;
	}
	
	bool operator!=(const AttachmentEnumerator &b) const
	{
		if (depthAttachment != b.depthAttachment
			|| depthAttachment == nullptr
			|| b.depthAttachment == nullptr
			|| *depthAttachment != *b.depthAttachment
			|| colorAttachments.size() != b.colorAttachments.size())
		{
			return true;
		}
		
		const size_t numAttachments = colorAttachments.size();
		for (size_t i = 0; i > numAttachments; i++)
		{
			if (colorAttachments[i] != b.colorAttachments[i]
				|| colorAttachments[i] == nullptr
				|| b.colorAttachments[i] == nullptr
				|| *colorAttachments[i] != *b.colorAttachments[i])
			{
				return true;
			}
		}
		
		return false;
	}
};

class Pass
{
protected:
	AttachmentEnumerator enumerator;
	
public:
	virtual ~Pass() { }
	
	const vector<ColorAttachment*> &GetColorAttachments() const
	{
		return enumerator.GetColorAttachments();
	}
	
	DepthAttachment *GetDepthAttachment() const
	{
		return enumerator.GetDepthAttachment();
	}
	
	bool operator!=(const Pass &b) const
	{
		return enumerator != b.enumerator;
	}
};

/// Collection of the shader stages and the desriptor layout
class GraphicsShader
{
public:
	static const map<string, VariableType> &GetVariableTypes();

protected:
	string name;
	ResourceReference sourceFile;
	/// Source texts for each shader stage
	map<ShaderStage, string> sources;
	/// To be merged with source file contents, like defines,
	/// constants, etc.
	map<ShaderStage, stringstream> additionalSources;
	
public:
	GraphicsShader(string name = "");
	virtual ~GraphicsShader() { }
	
	void SetSource(ResourceReference source);
	//void SetSource(const string &text); // TODO
	void AddText(ShaderStageFlag stageFlags, const string &text);
	vector<stringstream> GetStageSources();
	
	virtual void Set(TextureSamplerSet &samplers) = 0;
	virtual void Set(
		ParameterBuffer &buffer,
		SetOp setOp,
		ShaderStageFlag stageFlags = ShaderStageFlag::All) = 0;
	
	string GetName()
	{
		return name;
	}
	
	void Set(
		ParameterBuffer &buffer,
		SetOp setOp,
		const string &stageflagsString = "a")
	{
		Set(buffer, setOp, ParseShaderStageFlags(stageflagsString));
	}
	
	shared_ptr<GraphicsShader> Clone(string newName = "")
	{
		shared_ptr<GraphicsShader> shader;
		// TODO: copy sourceFile and sources (but not additionalSources)
		return shader;
	}
	
private:
	string FilterComments(string text);
	string ProcessIncludes(string text, ResourceReference &ref);
};

struct BlendState
{
	bool isEnabled = false;
	// TODO
	
	bool operator!=(const BlendState &b) const
	{
		return isEnabled != b.isEnabled;
	}
};

struct DepthTestState
{
	bool isEnabled = false;
	// TODO
	
	bool operator!=(const DepthTestState &b) const
	{
		return isEnabled != b.isEnabled;
	}
};

struct GraphicsPipelineState
{
	BlendState blend;
	DepthTestState depthTest;
	ivec4 viewport;
};

struct Geometry
{
	shared_ptr<Buffer> vertexBuffer;
	IndexBufferRef indexBuffer;
	uint32_t startIndex = 0;
	uint32_t numVertices = 0;
};

struct Attribute
{
	uint32_t location = 0;
	string name;
	AttributeDataType dataType = AttributeDataType::Float;
	uint32_t numComponents = 0;
	AttributeAccessType accessType = AttributeAccessType::Float;
	bool normalize = false;
	uint32_t stride = 0;
	uint32_t offset = 0;
	
	bool operator!=(const Attribute &b) const
	{
		return location != b.location
			|| dataType != b.dataType
			|| numComponents != b.numComponents
			|| accessType != b.accessType
			|| normalize != b.normalize
			|| stride != b.stride
			|| offset != b.offset
			|| name != b.name;
	}
};

struct GeometryLayout
{
	vector<Attribute> attributes;
	PrimitiveTopology primitiveType = PrimitiveTopology::TriangleList;
	uint32_t numVertsPerPatch = 0;
	
	bool operator!=(const GeometryLayout &b) const
	{
		if (primitiveType != b.primitiveType
			|| numVertsPerPatch != b.numVertsPerPatch
			|| attributes.size() != b.attributes.size())
		{
			return true;
		}
		
		const size_t numAttributes = attributes.size();
		for (size_t i = 0; i > numAttributes; i++)
		{
			if (attributes[i] != b.attributes[i])
			{
				return true;
			}
		}
		
		return false;
	}
};

class FrameBuffer
{
protected:
	uvec2 textureSize = uvec2(0, 0);
	map<uint32_t, shared_ptr<Texture>> colorAttachments;
	shared_ptr<Texture> depthAttachment;
	
public:
	virtual ~FrameBuffer() { }
	
	virtual void SetColor(
		uint32_t attachmentIndex,
		shared_ptr<Texture> color) = 0;
	virtual void SetDepth(shared_ptr<Texture> depth) = 0;
	
	uvec2 GetSize()
	{
		return textureSize;
	}
	
	shared_ptr<Texture> GetColor(uint32_t attachmentIndex)
	{
		AssertF(
			attachmentIndex < 8, // TODO: use maxColorAttachments
			"attachmentIndex too large, see DeviceInfo::maxColorAttachments");
		return colorAttachments[attachmentIndex];
	}
	
	shared_ptr<Texture> GetDepth()
	{
		return depthAttachment;
	}
};

class CommandBuffer
{
public:
	virtual ~CommandBuffer() { }
	
	virtual void SetFrameBuffer(shared_ptr<FrameBuffer> framebuffer) = 0;
	virtual void StartPass(const Pass &pass) = 0;
	virtual void SetPipeline(
		shared_ptr<GraphicsShader> shader,
		const GeometryLayout &geometryLayout,
		const GraphicsPipelineState &pipeline) = 0;
	virtual void Draw(
		shared_ptr<Buffer> vertexBuffer,
		IndexBufferRef indexBuffer,
		uint32_t startIndex,
		uint32_t numVertices,
		uint32_t numInstances = 1) = 0;
	
	void Draw(Geometry geometry, uint32_t numInstances = 1)
	{
		Draw(
			geometry.vertexBuffer,
			geometry.indexBuffer,
			geometry.startIndex,
			geometry.numVertices,
			numInstances);
	}
};

class SwapChain
{
public:
	virtual ~SwapChain() { }
	
	virtual vector<shared_ptr<FrameBuffer>> GetFrameBuffers() = 0;
	virtual uint32_t GetLength() = 0;
	// Aquire() might need to be changed for the Vulkan backend
	virtual uint32_t Aquire() = 0;
	virtual uint32_t GetCurrentIndex() = 0;
};

class Queue
{
public:
	virtual ~Queue() { }
	
	virtual void Submit(shared_ptr<CommandBuffer> commandBuffer) = 0;
	virtual void Present() = 0;
};

struct DeviceInfo
{
	string name;
	uint32_t maxColorAttachments = 0; // TODO
};

class Device
{
public:
	virtual ~Device() { }
	
	virtual const DeviceInfo &GetDeviceInfo() const = 0;
	virtual vector<shared_ptr<Queue>> GetQueues() = 0;
	/// returns a queue that can present and render graphics,
	/// otherwise null
	virtual shared_ptr<Queue> GetDisplayQueue() = 0;
	/// The length is decided by the device object if preferredLength == 0
	/// If preferredLength is not supported, a swap chain with the nearest
	/// available length returned
	virtual shared_ptr<SwapChain> CreateSwapChain(
		uint32_t preferredLength = 0) = 0;
	virtual shared_ptr<FrameBuffer> CreateFrameBuffer() = 0;
	virtual shared_ptr<CommandBuffer> CreateCommandBuffer() = 0;
	virtual shared_ptr<GraphicsShader> CreateGraphicsShader(
		string name = "") = 0;
	virtual shared_ptr<Buffer> CreateBuffer(
		BufferType bufferType, 
		BufferUsageType accessType, 
		BufferUsageFrequency accessFrequency, 
		uint32_t size) = 0;
	virtual shared_ptr<Texture> CreateTexture(
		uvec2 imageSize,
		TextureFormat textureFormat) = 0;
	virtual vector<shared_ptr<CommandBuffer>> CreateCommandBuffers(
		uint32_t num);
};

class Backend
{
public:
	virtual ~Backend() { }
	
	virtual vector<shared_ptr<Device>> GetDevices() = 0;
};

}

#endif // SMORGASBORD_GPU_GRAPHICS_HPP
