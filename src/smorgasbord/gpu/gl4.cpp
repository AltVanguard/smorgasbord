#include "gl4.hpp"

#include <smorgasbord/image/image.hpp>
#include <smorgasbord/util/log.hpp>

#include <cstring>
#include <array>

/*

# Notes #

## Buffer parameters' possible values

bufferType = GL_ARRAY_BUFFER, GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER,
	GL_ELEMENT_ARRAY_BUFFER, GL_PIXEL_PACK_BUFFER, GL_PIXEL_UNPACK_BUFFER,
	GL_TEXTURE_BUFFER, GL_TRANSFORM_FEEDBACK_BUFFER, GL_UNIFORM_BUFFER,
	GL_SHADER_STORAGE_BUFFER

usageHint = GL_STREAM_DRAW, GL_STREAM_READ, GL_STREAM_COPY, GL_STATIC_DRAW,
	GL_STATIC_READ, GL_STATIC_COPY, GL_DYNAMIC_DRAW, GL_DYNAMIC_READ,
	GL_DYNAMIC_COPY

From the Opengl Wiki on usageHint:
(the wording is less ambiguous than the spec)

DRAW: The user will be writing data to the buffer,
	but the user will not read it.
READ: The user will not be writing data, but the user will be reading it
	back.
COPY: The user will be neither writing nor reading the data.

STATIC:  The user will set the data once.
DYNAMIC: The user will set the data occasionally.
STREAM:  The user will be changing the data after every use.
	Or almost every use.

*/

using namespace Smorgasbord;

inline GLenum GetBufferType(Smorgasbord::BufferType type)
{
	switch (type)
	{
	case BufferType::Vertex:
		return GL_ARRAY_BUFFER;
	case BufferType::Index:
		return GL_ELEMENT_ARRAY_BUFFER;
	case BufferType::Global:
		return GL_SHADER_STORAGE_BUFFER;
	case BufferType::Constant:
		return GL_UNIFORM_BUFFER;
	}
	
	LogF("Invalid enum value");
}

inline GLenum GetBufferUsageSpecifier(
	BufferUsageType type,
	BufferUsageFrequency frequency)
{
	switch (type)
	{
	case BufferUsageType::Draw:
		switch (frequency)
		{
		case BufferUsageFrequency::Static:
			return GL_STATIC_DRAW;
		case BufferUsageFrequency::Dynamic:
			return GL_DYNAMIC_DRAW;
		case BufferUsageFrequency::Stream:
			return GL_STREAM_DRAW;
		}
	case BufferUsageType::Read:
		switch (frequency)
		{
		case BufferUsageFrequency::Static:
			return GL_STATIC_READ;
		case BufferUsageFrequency::Dynamic:
			return GL_DYNAMIC_READ;
		case BufferUsageFrequency::Stream:
			return GL_STREAM_READ;
		}
	case BufferUsageType::Copy:
		switch (frequency)
		{
		case BufferUsageFrequency::Static:
			return GL_STATIC_COPY;
		case BufferUsageFrequency::Dynamic:
			return GL_DYNAMIC_COPY;
		case BufferUsageFrequency::Stream:
			return GL_STREAM_COPY;
		}
	}
	
	LogF("Invalid enum value");
}

inline GLenum GetMappedDataAccessType(MappedDataAccessType type)
{
	switch (type)
	{
	case MappedDataAccessType::Read:
		return GL_READ_ONLY;
	case MappedDataAccessType::Write:
		return GL_WRITE_ONLY;
	case MappedDataAccessType::ReadWrite:
		return GL_READ_WRITE;
	}
	
	LogF("Invalid enum value");
}

inline GLenum GetIndexDataType(IndexDataType type)
{
	switch (type)
	{
	case IndexDataType::UInt8:
		return GL_UNSIGNED_BYTE;
	case IndexDataType::UInt16:
		return GL_UNSIGNED_SHORT;
	case IndexDataType::UInt32:
		return GL_UNSIGNED_INT;
	}
	
	LogF("Invalid enum value");
}

inline GLenum GetAttributeDataType(AttributeDataType type)
{
	switch (type)
	{
	case AttributeDataType::Float:
		return GL_FLOAT;
	case AttributeDataType::Double:
		return GL_DOUBLE;
	case AttributeDataType::Int8:
		return GL_BYTE;
	case AttributeDataType::Int16:
		return GL_SHORT;
	case AttributeDataType::Int32:
		return GL_INT;
	case AttributeDataType::UInt8:
		return GL_UNSIGNED_BYTE;
	case AttributeDataType::UInt16:
		return GL_UNSIGNED_SHORT;
	case AttributeDataType::UInt32:
		return GL_UNSIGNED_INT;
	}
	
	LogF("Invalid enum value");
}

inline GLenum GetPrimitiveTopology(PrimitiveTopology topology)
{
	switch (topology)
	{
	case PrimitiveTopology::PointList:
		return GL_POINTS;
	case PrimitiveTopology::LineList:
		return GL_LINES;
	case PrimitiveTopology::LineStrip:
		return GL_LINE_STRIP;
	case PrimitiveTopology::LineStripWithReset:
		return GL_LINE_STRIP;
	case PrimitiveTopology::TriangleList:
		return GL_TRIANGLES;
	case PrimitiveTopology::TriangleStrip:
		return GL_TRIANGLE_STRIP;
	case PrimitiveTopology::TriangleStripWithReset:
		return GL_TRIANGLE_STRIP;
	case PrimitiveTopology::TriangleFan:
		return GL_TRIANGLE_FAN;
	case PrimitiveTopology::TriangleFanWithReset:
		return GL_TRIANGLE_FAN;
	case PrimitiveTopology::PatchList:
		return GL_PATCHES;
	}
	
	LogF("Invalid enum value");
}

inline bool GetIsWithReset(PrimitiveTopology topology)
{
	/// Don't use default, so if a new topology is added, the compiler
	/// ensures that the return value is properly specified
	switch (topology)
	{
	case PrimitiveTopology::LineStripWithReset:
	case PrimitiveTopology::TriangleStripWithReset:
	case PrimitiveTopology::TriangleFanWithReset:
		return true;
	case PrimitiveTopology::PointList:
	case PrimitiveTopology::LineList:
	case PrimitiveTopology::LineStrip:
	case PrimitiveTopology::TriangleList:
	case PrimitiveTopology::TriangleStrip:
	case PrimitiveTopology::TriangleFan:
	case PrimitiveTopology::PatchList:
		return false;
	}
	
	return false;
}

inline GL4TextureFormat GetTextureFormat(Smorgasbord::TextureFormat format)
{
/*

From: https://www.khronos.org/opengl/wiki/Image_Format

OpenGL has a particular syntax for writing its color format enumerants. It
looks like this:

 GL_[components​][size​][type​]

The components​ field is the list of components that the format stores.
OpenGL only allows "R", "RG", "RGB", or "RGBA"; other combinations are not
allowed as internal image formats. The size​ is the bitdepth for each
component. The type​ indicates which of the 5 types mentioned above the
format is stored as. The following suffixes are used:
	"": No type suffix means unsigned normalized integer format.
	"_SNORM": Signed normalized integer format.
	"F": Floating-point. Thus, GL_RGBA32F is a floating-point format where
		each component is a 32-bit IEEE floating-point value.
	"I": Signed integral format. Thus GL_RGBA8I gives a signed integer
		format where each of the four components is an integer on the range
		[-128, 127].
	"UI": Unsigned integral format. The values go from [0, MAX_INT] for the
		integer size.

*/
	switch (format)
	{
	case TextureFormat::RGBA_8_8_8_8_UNorm:
		return { GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE };
	case TextureFormat::R_16_UNorm:
		return { GL_R16, GL_RED, GL_UNSIGNED_SHORT };
	case TextureFormat::Depth_24_UNorm:
		return { GL_DEPTH_COMPONENT24, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT };
	case TextureFormat::Depth_32_UNorm:
		return { GL_DEPTH_COMPONENT32, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT };
	}
	
	return { GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE };
}

inline GLenum GetSamplerFilter(SamplerFilter filter)
{
	switch (filter)
	{
	case SamplerFilter::Nearest:
		return GL_NEAREST;
	case SamplerFilter::Linear:
		return GL_LINEAR;
	}
	
	LogF("Invalid enum value");
}

inline GLenum GetSamplerWrap(SamplerWrap wrap)
{
	// TODO: GL_MIRRORED_REPEAT, GL_CLAMP_TO_BORDER, GL_MIRROR_CLAMP_TO_EDGE
	switch (wrap)
	{
	case SamplerWrap::Clamp:
		return GL_CLAMP_TO_EDGE;
	case SamplerWrap::Repeat:
		return GL_REPEAT;
	case SamplerWrap::MirroredRepeat:
		return GL_MIRRORED_REPEAT;
	}
	
	LogF("Invalid enum value");
}

inline GLenum GetShaderStage(RasterizationStage stage)
{
	switch (stage)
	{
	case RasterizationStage::Vertex:
		return GL_VERTEX_SHADER;
	case RasterizationStage::TesselationControl:
		return GL_TESS_CONTROL_SHADER;
	case RasterizationStage::TesselationEvaluation:
		return GL_TESS_EVALUATION_SHADER;
	case RasterizationStage::Geometry:
		return GL_GEOMETRY_SHADER;
	case RasterizationStage::Fragment:
		return GL_FRAGMENT_SHADER;
	case RasterizationStage::Num:
		LogF("Invalid enum value");
		return GL_INVALID_ENUM;
	}
	
	LogF("Invalid enum value");
}

inline string GetShaderTypeString(GLenum stageType)
{
	switch (stageType)
	{
	case GL_VERTEX_SHADER:
		return "vertex";
	case GL_TESS_CONTROL_SHADER:
		return "tesselation control";
	case GL_TESS_EVALUATION_SHADER:
		return "tesselation evaluation";
	case GL_GEOMETRY_SHADER:
		return "geometry";
	case GL_FRAGMENT_SHADER:
		return "fragment";
	case GL_COMPUTE_SHADER:
		return "compute";
		
	default:
		LogF("Couldn't recognize GL shader type");
		return "undetermined";
	}
}

Smorgasbord::GL4Buffer::GL4Buffer(
	GL4Device& device,
	BufferType bufferType,
	BufferUsageType accessType,
	BufferUsageFrequency accessFrequency,
	int size)
	: Buffer(bufferType, accessType, accessFrequency, size)
	, gl(device.GetLoader())
{
	const GLenum nativeBufferType = ::GetBufferType(this->bufferType);
	const GLenum nativeUsageSpecifier =
		GetBufferUsageSpecifier(this->accessType, this->accessFrequency);
	gl.glGenBuffers(1, &(this->nativeDeviceBufferID));
	gl.glBindBuffer(nativeBufferType, this->nativeDeviceBufferID);
	gl.glBufferData(nativeBufferType, size, NULL, nativeUsageSpecifier);
	
#ifdef SMORGASBORD_GL4_UNBIND
	gl.glBindBuffer(nativeBufferType, 0);
#endif
}

uint8_t *Smorgasbord::GL4Buffer::GetMappedData()
{
	AssertF(mappedData != nullptr, "Buffer isn't mapped");
	return (uint8_t*)mappedData;
}

void Smorgasbord::GL4Buffer::Map(MappedDataAccessType mapAccessType)
{
	const GLenum nativeBufferType = ::GetBufferType(this->bufferType);
	const GLenum nativeMappedDataAccessType =
		GetMappedDataAccessType(mapAccessType);
	gl.glBindBuffer(nativeBufferType, nativeDeviceBufferID);
	mappedData = reinterpret_cast<uint8_t*>(
			gl.glMapBuffer(nativeBufferType, nativeMappedDataAccessType));
}

void Smorgasbord::GL4Buffer::Unmap()
{
	mappedData = nullptr;
	
	const GLenum nativeBufferType = ::GetBufferType(this->bufferType);
	gl.glUnmapBuffer(nativeBufferType);
	
#ifdef SMORGASBORD_GL4_UNBIND
	gl.glBindBuffer(nativeBufferType, 0);
#endif
}


Smorgasbord::GL4Texture::GL4Texture(
	GL4Device& device, uvec2 _size, TextureFormat _format)
	: Texture(_size, _format)
	, gl(device.GetLoader())
{
	if (id != 0)
	{
		LogE("already inited");
		return;
	}
	
	if (size.x == 0 || size.y == 0)
	{
		LogE("neither dimension can be 0");
		return;
	}
	
	GL4TextureFormat nativeFormat = GetTextureFormat(format); 
	
	// Upload texture
	
	gl.glGenTextures(1, &id);
	
	Bind(0);
	
	gl.glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	
	/// OpenGL 3.2+ does not accept color component count as internalFormat,
	/// must use one of the predifined constants
	gl.glTexImage2D(
		GL_TEXTURE_2D, 0,
		nativeFormat.internalFormat,
		size.x, size.y,
		0,
		nativeFormat.format, nativeFormat.dataType,
		NULL);
	
	/// Default mimpmap levels are 1000 and default tex filter is 
	/// GL_NEAREST_MIPMAP_LINEAR, which makes the texture incomplete if
	/// mipmap levels are not set properly or if the tex filter is not set
	/// to a filter which does not reference non existent mipmap levels
	///
	/// Symptom: texture will render pure black
	///
	/// Further info:
	/// http://www.opengl.org/wiki/
	/// Common_Mistakes#Creating_a_complete_texture
	gl.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
	/// GL_TEXTURE_MAX_LEVEL is the index of the highest level,
	/// not the number of level
	gl.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
	SetTextureFilter(SamplerFilter::Nearest, SamplerFilter::Nearest);
	SetTextureWrap(
		SamplerWrap::Clamp, SamplerWrap::Clamp, SamplerWrap::Clamp);
	
	Unbind();
}

Smorgasbord::GL4Texture::~GL4Texture()
{
	if (id == 0)
	{
		LogE("texture already empty");
		return;
	}
	
	gl.glDeleteTextures(1, &id);
	id = 0;
}

void Smorgasbord::GL4Texture::Bind(int slot)
{
	if (bindSlot != -1)
		Unbind();
		
	bindSlot = slot;
	gl.glActiveTexture(GL_TEXTURE0 + slot);
	gl.glBindTexture(GL_TEXTURE_2D, id);
}

void Smorgasbord::GL4Texture::Unbind()
{
	if (bindSlot == -1)
	{
		LogE("texture is not bound");
		return;
	}
	
	gl.glActiveTexture(GL_TEXTURE0 + bindSlot);
	gl.glBindTexture(GL_TEXTURE_2D, 0);
	bindSlot = -1;
}

void Smorgasbord::GL4Texture::SetTextureFilter(
	SamplerFilter minify, SamplerFilter magnify)
{
	if (bindSlot == -1)
	{
		LogE("cannot set filter, texture is not bound to a texturing unit");
		return;
	}
	
	//glActiveTexture(GL_TEXTURE0 + bindSlot);
	gl.glTexParameteri(
		GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GetSamplerFilter(minify));
	gl.glTexParameteri(
		GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GetSamplerFilter(magnify));
}

void Smorgasbord::GL4Texture::SetTextureWrap(
	SamplerWrap s, SamplerWrap t, SamplerWrap r)
{
	if (bindSlot == -1)
	{
		LogE("cannot set wrap, texture is not bound to a texturing unit");
		return;
	}
	
	//glActiveTexture(GL_TEXTURE0 + bindSlot);
	gl.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GetSamplerWrap(s));
	gl.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GetSamplerWrap(t));
	gl.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GetSamplerWrap(r));
}

void Smorgasbord::GL4Texture::Upload(Smorgasbord::Image &image)
{
	if (size.x != image.imageSize.x || size.y != image.imageSize.y)
	{
		LogE("image dimensions do not match with texture dimensions");
		return;
	}
	
	Bind(0);
	
	GL4TextureFormat nativeFormat = GetTextureFormat(format);
	
	gl.glTexSubImage2D(
		GL_TEXTURE_2D, 0,
		0, 0,
		size.x, size.y,
		nativeFormat.format, nativeFormat.dataType,
		image.data.data());
	
	Unbind();
}

void Smorgasbord::GL4Texture::Verify(Smorgasbord::Image &image)
{
	Bind(0);
	
	vector<unsigned char> returnedImage;
	returnedImage.resize(image.data.size());
	
	GL4TextureFormat nativeFormat = GetTextureFormat(format);
	
	gl.glGetTexImage(
		GL_TEXTURE_2D, 0,
		nativeFormat.format, nativeFormat.dataType,
		returnedImage.data()
	);
	
	size_t i = 0;
	for (; i < image.data.size(); i++)
	{
		if (image.data[i] != returnedImage[i])
		{
			LogE("Verification failed");
			break;
		}
	}
	
	if (i >= image.data.size())
	{
		LogI("Verification successful!");
	}
	
	Unbind();
}

shared_ptr<Smorgasbord::Image> Smorgasbord::GL4Texture::Download()
{
	Bind(0);
	
	shared_ptr<Image> image = make_shared<Image>(size, 4);
	
	GL4TextureFormat nativeFormat = GetTextureFormat(format);
	
	gl.glGetTexImage(
		GL_TEXTURE_2D, 0,
		nativeFormat.format, nativeFormat.dataType,
		image->data.data());
	
	Unbind();
	
	return image;
}

GL4RasterizationShader::GL4RasterizationShader(
	GL4Device& device, string name)
	: RasterizationShader(name)
	, gl(device.GetLoader())
{ }

void GL4RasterizationShader::ResetBindings()
{
	// TODO: unset sampler bindings?
}

void GL4RasterizationShader::SetConstantField(
	uint32_t location,
	const VariableType &type,
	void *p)
{
	switch (type.baseType)
	{
	case VariableBaseType::Float:
		switch (type.numRows)
		{
		case 1:
			gl.glUniform1fv(location, 1, (GLfloat*)p);
			break;
		case 2:
			gl.glUniform2fv(location, 1, (GLfloat*)p);
			break;
		case 3:
			gl.glUniform3fv(location, 1, (GLfloat*)p);
			break;
		case 4:
			gl.glUniform4fv(location, 1, (GLfloat*)p);
			break;
		default:
			LogF("Field numComponents must be in the [1,4] range");
		}
		break;
		
	case VariableBaseType::Int:
	case VariableBaseType::UInt:
		LogF("Not implemented");
		break;
		
	case VariableBaseType::Matrix:
		switch (type.numRows)
		{
		case 2:
			switch (type.numColumns)
			{
			case 2:
				gl.glUniformMatrix2fv(location, 1, false, (GLfloat*)p);
				break;
			case 3:
				gl.glUniformMatrix2x3fv(location, 1, false, (GLfloat*)p);
				break;
			case 4:
				gl.glUniformMatrix2x4fv(location, 1, false, (GLfloat*)p);
				break;
			default:
				LogF("Field numColumns must be in the [2,4] range");
			}
			break;
		case 3:
			switch (type.numColumns)
			{
			case 2:
				gl.glUniformMatrix3x2fv(location, 1, false, (GLfloat*)p);
				break;
			case 3:
				gl.glUniformMatrix3fv(location, 1, false, (GLfloat*)p);
				break;
			case 4:
				gl.glUniformMatrix3x4fv(location, 1, false, (GLfloat*)p);
				break;
			default:
				LogF("Field numColumns must be in the [2,4] range");
			}
			break;
		case 4:
			switch (type.numColumns)
			{
			case 2:
				gl.glUniformMatrix4x2fv(location, 1, false, (GLfloat*)p);
				break;
			case 3:
				gl.glUniformMatrix4x3fv(location, 1, false, (GLfloat*)p);
				break;
			case 4:
				gl.glUniformMatrix4fv(location, 1, false, (GLfloat*)p);
				break;
			default:
				LogF("Field numColumns must be in the [2,4] range");
			}
			break;
		default:
			LogF("Field numRows must be in the [2,4] range");
		}
		break;
	}
}

void GL4RasterizationShader::ApplyBindings(GL4Device *device)
{
	(void)device;
	
	// TODO: create buffer for parameterBuffer if buffer backed
	///for (auto &b : parameterBuffers)
	///{
	///	if (b.first->gpuBuffer == nullptr)
	///	{
	///		b.first->gpuBuffer = device->CreateBuffer(
	///			BufferType::Constant,
	///			BufferUsageType::Draw,
	///			BufferUsageFrequency::Stream,
	///			b.first->GetBufferSize());
	///	}
	/// 
	/// AssertF(gpuBuffer != nullptr, "Given buffer isn't valid");
	/// AssertF(gpuBuffer->GetSize() >= b.GetBufferSize(),
	/// 	"Given buffer isn't large enough");
	///}
	
	// Upload constant buffers
	// TODO
	// Bind constant buffers
	// TODO
	
	// Bind constants
	
	for (const auto &buffer : parameterBuffers)
	{
		if (buffer.second.setOp != SetOp::Constants)
		{
			continue;
		}
		
		uint32_t i = 0;
		const auto &types = GetVariableTypes();
		for (const auto &field : buffer.first->GetFields())
		{
			auto result = types.find(field.type);
			AssertF(
				result != types.end(),
				"Unrecognised parameter type");
				
			/// Avoid mat3 in ParameterBuffers: mat3 must be uploaded as a
			/// mat4 according to the OpenGL std140​ layout specification, so
			/// it will ruin field alignment and you won't even be able to
			/// read the mat3 from the shader properly
			AssertE(
				string(field.type) != "mat3",
				"Look at that! A mat3 in a ParameterBuffer? That won't do!");
			
			// TODO: check if variable is used/enumerated
			// see enumerableConstants
			
			SetConstantField(i, result->second, field.p);
			
			i++;
		}
		
		//break;
	}
}

inline void AddToStages(
	map<RasterizationStage, stringstream> &stages,
	const string &text,
	Smorgasbord::RasterizationStageFlag stageMask)
{
	for (auto &stage : stages)
	{
		if ((StageToFlag(stage.first) & stageMask) > 0)
		{
			stage.second << text << endl;
		}
	}
}

inline string GetAttributeTypeName(
	AttributeAccessType accessType, uint32_t numComponents)
{
	string result;
	
	switch(accessType)
	{
	case AttributeAccessType::Float:
		result += "vec";
		break;
	case AttributeAccessType::Double:
		result += "dvec";
		break;
	case AttributeAccessType::Int:
		result += "ivec";
		break;
	}
	
	return result + fmt::format("{0}", numComponents);
}

void GL4RasterizationShader::Compile(
	const Pass &pass, const GeometryLayout &geometryLayout)
{
	if (isCompiled)
	{
		return;
	}
	
	map<RasterizationStage, stringstream> stages;
	//ShaderStageFlag activeStageMask = ShaderStageFlag::None;
	
	for (auto &source : sources)
	{
		if (source.second.length() > 0)
		{
			stages.emplace(source.first, stringstream());
			//activeStageMask |= StageToFlag(source.first);
		}
	}
	
	// TODO: add additionalsources to stage mask
	
	AddToStages(stages, "#version 450", RasterizationStageFlag::All);
	
	// Add input layout
	
	if (geometryLayout.attributes.size() > 0)
	{
		for (const auto &attribute : geometryLayout.attributes)
		{
			string attributeString = fmt::format(
				"layout(location = {0}) in {1} {2};",
				attribute.location,
				GetAttributeTypeName(
					attribute.accessType, attribute.numComponents),
				attribute.name);
			
			AddToStages(
				stages, attributeString, RasterizationStageFlag::Vertex);
		}
	}
	
	// Add samplers
	
	if (samplers != nullptr)
	{
		uint32_t i = 0;
		for (auto& sampler : samplers->GetSamplers())
		{
			string bindingString =
				fmt::format("layout(binding = {0}) uniform sampler2D {1};",
				i, sampler->name);
			
			AddToStages(stages, bindingString, sampler->GetStageMask());
			i++;
		}
	}
	
	// Add buffered parameterBuffers
	
	// TODO
	
	// Add constants
	
	for (auto &buffer : parameterBuffers)
	{
		if (buffer.second.setOp == SetOp::Constants)
		{
			uint32_t i = 0;
			for (auto &field : buffer.first->GetFields())
			{
				/// Buffer backed parameter buffers have their fields
				/// aliased. We add the defines for the constants too,
				/// so in case of name collisions we don't get weird
				/// behavior
				string unformString =
					fmt::format(
						"layout(location = {0}) uniform {1} {2};\n"
						"#ifdef {2}\n"
						"#error Parameter variable already declared\n"
						"#endif\n",
					i, field.type, field.name);
				
				AddToStages(
					stages, unformString, RasterizationStageFlag::All);
				i++;
			}
			break;
		}
	}
	
	// Add Pass attachments
	
	for (auto &color : pass.GetColorAttachments())
	{
		string colorString =
			fmt::format("out {0} {1};",
			color->GetType(), color->GetName());
		
		AddToStages(stages, colorString, RasterizationStageFlag::Fragment);
	}
	
	// TODO: depth attachments
	
	// Add remaining sources
	
	// TODO: Add additionalSources
	
	for (auto &stage : stages)
	{
		stage.second << sources[stage.first];
	}
	
	// Compile shader
	
	for	(auto &stage : stages)
	{
		GLenum stageType = GetShaderStage(stage.first);
		GLuint sourceID = gl.glCreateShader(stageType);
		string s = stage.second.str();
		const char *sp = s.c_str();
		gl.glShaderSource(sourceID, 1, &sp, NULL);
		sourceIDs.emplace(stage.first, sourceID);
	}
	
	GLuint newProgramID = gl.glCreateProgram();
	for (const auto &i : sourceIDs)
	{
		GLuint sourceID = i.second;
		gl.glCompileShader(sourceID);
		
		GLint compileStatus = GL_FALSE;
		gl.glGetShaderiv(sourceID, GL_COMPILE_STATUS, &compileStatus);
		if (compileStatus == GL_FALSE)
		{
			PrintErrorLog(sourceID, newProgramID);
			//ReleaseResources();
			return;
		}
		
		gl.glAttachShader(newProgramID, sourceID);
	}
	
	gl.glLinkProgram(newProgramID);
	
	GLint linkStatus = GL_FALSE;
	gl.glGetProgramiv(newProgramID, GL_LINK_STATUS, &linkStatus);
	if (linkStatus == GL_FALSE)
	{
		for (const auto &stage : stages)
		{
			LogI(
				"{0} shader: \n{1}",
				StageToString(stage.first),
				stage.second.str());
		}
		
		LogE("GL program LINK failed.");
		
		//ReleaseResources();
		return;
	}
	
	// TODO: enumerate constants into enumerableConstants
	
	isCompiled = true;
	this->programID = newProgramID;
}

void GL4RasterizationShader::PrintErrorLog(GLuint sourceID, GLuint programID)
{
	stringstream errorLog;
	GLint stageType = GL_NONE;
	
	gl.glGetShaderiv(sourceID, GL_SHADER_TYPE, &stageType);
	
	errorLog << fmt::format(
		"Shader compilation failed\n"
		"Compile errors ({0} stage):\n",
		GetShaderTypeString((GLenum)stageType));
	
	int programInfoLogLength = 0;
	gl.glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &programInfoLogLength);
	if (programInfoLogLength > 0)
	{
		string infoLog;
		infoLog.resize(programInfoLogLength);
		
		int charCount = 0;
		gl.glGetProgramInfoLog(
			programID, programInfoLogLength, &charCount, &infoLog[0]);
		
		if (charCount > 0)
		{
			errorLog << "Program errors:\n";
			errorLog << infoLog.substr(0, charCount) << endl;
		}
	}
	
	GLint sourceLength = -1;
	gl.glGetShaderiv(sourceID, GL_SHADER_SOURCE_LENGTH, &sourceLength);
	
	vector<char> source(sourceLength);
	GLsizei actualSourceLength;
	gl.glGetShaderSource(
		sourceID, sourceLength, &actualSourceLength, source.data());
	errorLog << "SOURCE LISTING Retrieved source:\n";
	errorLog << source.data() << endl;
	errorLog << "SOURCE LISTING END\n\n";

	int sourceInfoLogLength = 0;
	gl.glGetShaderiv(sourceID, GL_INFO_LOG_LENGTH, &sourceInfoLogLength);
	if (sourceInfoLogLength > 0)
	{
		string infoLog;
		infoLog.resize(sourceInfoLogLength);
		
		int charCount = 0;
		gl.glGetShaderInfoLog(
			sourceID, sourceInfoLogLength, &charCount, &infoLog[0]);
		
		if (charCount > 0)
		{
			errorLog << infoLog.substr(0, charCount) << endl;
		}
	}
	
	errorLog << "BUILD ERROR LOG END\n\n";
	LogE("{0}", errorLog.str());
}

void GL4RasterizationShader::Use()
{
	AssertF(programID != 0, "Cannot use uninitialized program");
	gl.glUseProgram(programID);
}

void GL4RasterizationShader::Set(TextureSamplerSet &_samplers)
{
	samplers = &_samplers;
	
	uint32_t i = 0;
	for (const TextureSampler* sampler : samplers->GetSamplers())
	{
		AssertE(sampler->texture != nullptr,
			"Sampler to be set has no texture");
		
		shared_ptr<GL4Texture> texture =
			dynamic_pointer_cast<GL4Texture>(sampler->texture);
		
		texture->Bind(i);
		texture->SetTextureFilter(sampler->minify, sampler->magnify);
		texture->SetTextureWrap(sampler->s, sampler->t, sampler->r);
		
		i++;
	}
	
	// TODO: unbind slots we don't use,but was used in the last shader
	//		 See: GL4RasterizationShader::GetNumSamplers()
}

void GL4RasterizationShader::Set(
	ParameterBuffer &buffer,
	SetOp setOp,
	RasterizationStageFlag stageFlags)
{
	// TODO: check that only one buffer is bound with SetOp::Constants
	
	auto findResult = parameterBuffers.find(&buffer);
	if (findResult == parameterBuffers.end())
	{
		parameterBuffers.emplace(
			&buffer, GL4BindCommand(setOp, stageFlags));
	}
	else
	{
		AssertE(findResult->second.stageFlag == stageFlags,
			"Cannot change ParameterBuffer stageFlags once set");
		AssertE(
			(findResult->second.setOp == SetOp::Constants)
				== (setOp == SetOp::Constants),
			"Cannot change ParameterBuffer setOp type once set");
		AssertE(findResult->second.setOp != SetOp::None,
			"Invalid argument SetOp::BindOnly. "
			"ParameterBuffer already bound");
		
		if (findResult->second.stageFlag == stageFlags
			&& ((findResult->second.setOp == SetOp::Constants)
				== (setOp == SetOp::Constants))
			&& findResult->second.setOp != SetOp::None)
		{
			findResult->second.setOp = setOp;
		}
	}
}

GL4CommandBuffer::GL4CommandBuffer(GL4Device& _device)
	: device(_device)
	, gl(_device.GetLoader())
{
	
}

GL4CommandBuffer::~GL4CommandBuffer()
{
	// TODO?: Could be collected into an array and deleted at ones.
	//		Probably wouldn't make much difference
	for (pair<GL4VAOKey, GLuint> vao : vaos)
	{
		gl.glDeleteVertexArrays(1, &vao.second);
	}
	vaos.clear();
}

void GL4CommandBuffer::SetFrameBuffer(shared_ptr<FrameBuffer> _frameBuffer)
{
	shared_ptr<IGL4FrameBuffer> fb =
		dynamic_pointer_cast<IGL4FrameBuffer>(_frameBuffer);
	AssertE(fb != nullptr, "Can only set a GL framebuffer");
	fb->Use();
	this->frameBuffer = fb;
}

void GL4CommandBuffer::StartPass(const Pass &_pass)
{
	// Set Pass
	
	const vector<ColorAttachment*> &colorAttachments =
		_pass.GetColorAttachments();
	
	vector<GLenum> clears;
	vector<vec4> clearColors;
	for (const ColorAttachment *colorAttachment : colorAttachments)
	{
		if (colorAttachment != nullptr
			&& colorAttachment->GetLoadOp() == LoadOp::Clear)
		{
			clears.push_back(
				GL_COLOR_ATTACHMENT0 + colorAttachment->GetIndex());
			clearColors.push_back(colorAttachment->GetClearColor());
		}
	}
	
	if (clears.size() > 0)
	{
		const size_t numColors = clearColors.size();
		for (size_t i = 0; i < numColors; i++)
		{
			gl.glClearBufferfv(GL_COLOR, i, &clearColors[i].x);
		}
	}
	
	DepthAttachment *depthAttachment = _pass.GetDepthAttachment();
	
	if (depthAttachment != nullptr
		&& depthAttachment->GetLoadOp() == LoadOp::Clear)
	{
		float depthClearValue = depthAttachment->GetDepthClearValue();
		gl.glClearBufferfv(
			GL_DEPTH,
			0, // must be 0 for GL_DEPTH
			&depthClearValue);
		
		///glClearBufferfi(
		///	GL_DEPTH_STENCIL,
		///	0, // must be 0 for GL_DEPTH_STENCIL
		///	depthAttachment->GetDepthClearValue(),
		///	depthAttachment->GetStencilClearValue());
	}
	
	// TODO: stencil clear
	
	// we need to store it, so it's available for shader compilation
	passAddress = &_pass;
}

void GL4CommandBuffer::SetPipeline(
	shared_ptr<RasterizationShader> _shader,
	const GeometryLayout &_geometryLayout,
	const RasterizationPipelineState &_pipelineState)
{
	// Set Shader
	
	AssertF(_shader != nullptr, "Uninitialized shader was given");
	shared_ptr<GL4RasterizationShader> s =
			dynamic_pointer_cast<GL4RasterizationShader>(_shader);
	AssertF(s != nullptr, "Given shader is not a GL4GraphicsShader");
	if (shader == nullptr
		|| shader != _shader)
	{
		shader = s;
		shader->ResetBindings();
	}
	
	// Set GeometryLayout
	
	/// Store if for later. The index buffer is part of VAO state,
	/// so we can't create the VAO yet
	geometryLayout = _geometryLayout;
	
	// Set pipeline state
	
	if (pipelineState.blend != _pipelineState.blend || isFirstRun)
	{
		gl.SetIsEnabled(GL_BLEND, _pipelineState.blend.isEnabled);
	}
	
	if (pipelineState.depthTest != _pipelineState.depthTest || isFirstRun)
	{
		gl.SetIsEnabled(GL_DEPTH_TEST, _pipelineState.depthTest.isEnabled);
	}
	
	if (pipelineState.viewport != _pipelineState.viewport || isFirstRun)
	{
		const ivec4 &viewport = _pipelineState.viewport;
		gl.glViewport(viewport.x, viewport.y, viewport.z, viewport.w);
	}
	
	pipelineState = _pipelineState;
	isFirstRun = false;
}

void GL4CommandBuffer::Draw(
	shared_ptr<Buffer> _vertexBuffer,
	IndexBufferRef _indexBuffer,
	uint32_t startIndex,
	uint32_t numVertices,
	uint32_t numInstances)
{
	AssertF(shader != nullptr, "Cannot draw without a shader");
	AssertF(passAddress != nullptr, "Cannot draw without a pass set");
	shader->Compile(*passAddress, geometryLayout);
	shader->Use();
	shader->ApplyBindings(&device);
	
	// TODO: validate shader for every unique pipiline state
	///int validationStatus = 0;
	///glValidateProgram(programID);
	///glGetProgramiv(programID, GL_VALIDATE_STATUS, &validationStatus);
	///if (validationStatus == GL_FALSE)
	///{
	///	LogE("GL program validation failed.");
	///}
	
	shared_ptr<GL4Buffer> vertexBuffer =
		dynamic_pointer_cast<GL4Buffer>(_vertexBuffer);
	GL4IndexBufferRef indexBuffer = _indexBuffer;
	
	if (_vertexBuffer != nullptr && vertexBuffer == nullptr)
	{
		LogE("Provided vertexBuffer is not a GL4Buffer");
	}
	
	// Init VAO
	
	GL4VAOKey key = {
		vertexBuffer != nullptr ? vertexBuffer->GetID() : 0,
		indexBuffer.IsValid() ? indexBuffer.buffer->GetID() : 0 };
	auto vaoResult = vaos.find(key);
	if (vaoResult != vaos.end())
	{
		GLuint vao = vaoResult->second;
		gl.glBindVertexArray(vao);
	}
	else // cache new VAO
	{
		GLuint vao = 0;
		gl.glGenVertexArrays(1, &vao);
		gl.glBindVertexArray(vao);
		
		/// We need to bind an VAO even if we don't have geometry to bind.
		/// see: https://www.khronos.org/opengl/wiki/Vertex_Rendering
		/// In that case, we don't enable any buffers, but still can invoke
		/// the pipeline and generate geometry in vertex shader
		
		if (key.vertexBufferID != 0)
		{
			gl.glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer->GetID());
			
			for	(Attribute &attribute : geometryLayout.attributes)
			{
				gl.glEnableVertexAttribArray(attribute.location);
				
				switch (attribute.accessType)
				{
				case AttributeAccessType::Float:
					gl.glVertexAttribPointer(
						attribute.location,
						attribute.numComponents, 
						GetAttributeDataType(attribute.dataType),
						attribute.normalize ? GL_TRUE : GL_FALSE, 
						attribute.stride,
						&static_cast<uint8_t*>(nullptr)[attribute.offset]
						);
					break;
					
				case AttributeAccessType::Double:
					gl.glVertexAttribLPointer(
						attribute.location,
						attribute.numComponents, 
						GetAttributeDataType(attribute.dataType),
						attribute.stride,
						&static_cast<uint8_t*>(nullptr)[attribute.offset]
						);
					break;
					
				case AttributeAccessType::Int:
					gl.glVertexAttribIPointer(
						attribute.location,
						attribute.numComponents, 
						GetAttributeDataType(attribute.dataType),
						attribute.stride,
						&static_cast<uint8_t*>(nullptr)[attribute.offset]
						);
					break;
				}
			}
		}
		
		if (key.indexBufferID != 0)
		{
			/// ELEMENT_ARRAY_BUFFER is part of VAO state. See Table 23.4
			/// of the 4.6 Core spec or Table 6.8 of the 2.1 spec
			gl.glBindBuffer(
				GL_ELEMENT_ARRAY_BUFFER, indexBuffer.buffer->GetID());
		}
		
		vaos.emplace(key, vao);
	}
	
	// Draw
	
	if (geometryLayout.primitiveType == PrimitiveTopology::PatchList 
		&& geometryLayout.numVertsPerPatch > 3)
	{
		gl.glPatchParameteri(
			GL_PATCH_VERTICES,
			static_cast<GLint>(geometryLayout.numVertsPerPatch));
	}
	
	// TODO: primitive restart (not part of VAO,
	// see Table 24.5 of the 4.6 Core spec)
	
	if (indexBuffer.IsValid())
	{
		gl.glDrawElementsInstanced(
			GetPrimitiveTopology(geometryLayout.primitiveType),
			static_cast<GLsizei>(numVertices),
			indexBuffer.dataType,
			reinterpret_cast<char*>(startIndex *
				GetIndexDataTypeSize(_indexBuffer.dataType)),
			numInstances
			);
	}
	else
	{
		gl.glDrawArraysInstanced(
			GetPrimitiveTopology(geometryLayout.primitiveType),
			static_cast<GLint>(startIndex),
			static_cast<GLsizei>(numVertices),
			numInstances
			);
	}
}

GL4IndexBufferRef::GL4IndexBufferRef()
{ }

GL4IndexBufferRef::GL4IndexBufferRef(IndexBufferRef &_indexBuffer)
	: buffer(dynamic_pointer_cast<GL4Buffer>(_indexBuffer.buffer))
	, dataType(GetIndexDataType(_indexBuffer.dataType))
{ }

Smorgasbord::GL4FrameBuffer::GL4FrameBuffer(GL4Device& device)
	: gl(device.GetLoader())
{
	gl.glGenFramebuffers(1, &id);
}

Smorgasbord::GL4FrameBuffer::~GL4FrameBuffer()
{
	if (id > 0)
	{
		gl.glDeleteFramebuffers(1, &id);
	}
}

bool Smorgasbord::GL4FrameBuffer::IsReady()
{
	bool state = (gl.glCheckFramebufferStatus(GL_FRAMEBUFFER)
		== GL_FRAMEBUFFER_COMPLETE);
	return state;
}

void Smorgasbord::GL4FrameBuffer::SetColor(
	uint32_t attachementIndex,
	shared_ptr<Smorgasbord::Texture> _colorTex)
{
	shared_ptr<GL4Texture> colorTex = 
		dynamic_pointer_cast<GL4Texture>(_colorTex);
	
	if (!colorTex->IsReady())
	{
		LogE("Cannot bind an invalid texture");
		return;
	}
	
	if (textureSize.x > 0
		&& textureSize.y > 0
		&& colorTex->GetSize() != textureSize)
	{
		LogE("Texture does not match expected size");
		return;
	}
	
	if (attachementIndex >= 8) // TODO: use maxColorAttachments
	{
		LogE("Attachment index exceeds GL_MAX_COLOR_ATTACHMENTS");
		return;
	}
	
	Use();
	this->colorAttachments[attachementIndex] = colorTex;
	gl.glFramebufferTexture2D(
		GL_FRAMEBUFFER,
		GL_COLOR_ATTACHMENT0 + attachementIndex,
		GL_TEXTURE_2D,
		colorTex->GetID(),
		0);
	
	this->textureSize = colorTex->GetSize();
}

void Smorgasbord::GL4FrameBuffer::SetDepth(
	shared_ptr<Smorgasbord::Texture> _depthTex)
{
	shared_ptr<GL4Texture> depthTex = 
		dynamic_pointer_cast<GL4Texture>(_depthTex);
		
	if (!depthTex->IsReady())
	{
		LogE("fbo: cannot bind an invalid texture");
		return;
	}
	
	if (depthTex->GetSize() != textureSize)
	{
		LogE("fbo: texture does not match expected size");
		return;
	}
	
	Use();
	this->depthAttachment = depthTex;
	gl.glFramebufferTexture2D(
		GL_FRAMEBUFFER,
		GL_DEPTH_ATTACHMENT,
		GL_TEXTURE_2D,
		depthTex->GetID(),
		0);
	
	this->textureSize = depthTex->GetSize();
}

GLuint GL4FrameBuffer::GetID()
{
	return id;
}

void Smorgasbord::GL4FrameBuffer::Use()
{
	// TODO: check if already bound in debug
	gl.glBindFramebuffer(GL_FRAMEBUFFER, id);
}

void GL4FrameBuffer::SetDrawBuffers()
{
	vector<GLuint> buffers;
	for (const auto &color : colorAttachments)
	{
		buffers.push_back(
			GL_COLOR_ATTACHMENT0 + color.first);
	}
	
	gl.glDrawBuffers((GLsizei)buffers.size(), buffers.data());
}

GL4SystemFrameBuffer::GL4SystemFrameBuffer(GL4Device& device)
	: gl(device.GetLoader())
{
	
}

void GL4SystemFrameBuffer::SetColor(
	uint32_t attachmentIndex,
	shared_ptr<Texture> color)
{
	(void)attachmentIndex;
	(void)color;
	LogW("Cannot set color attachment for system attachment");
}

void GL4SystemFrameBuffer::SetDepth(shared_ptr<Texture> depth)
{
	(void)depth;
	LogW("Cannot set depth attachment for system attachment");
}

GLuint GL4SystemFrameBuffer::GetID()
{
	return 0;
}

void GL4SystemFrameBuffer::Use()
{
	gl.glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void GL4SystemFrameBuffer::SetDrawBuffers()
{
	gl.glDrawBuffer(GL_BACK);
}

GL4SwapChain::GL4SwapChain(GL4Device& device)
{
	frameBuffers.push_back(make_shared<GL4SystemFrameBuffer>(device));
}

vector<shared_ptr<FrameBuffer> > GL4SwapChain::GetFrameBuffers()
{
	return frameBuffers;
}

uint32_t GL4SwapChain::GetLength()
{
	// OpenGL driver handles the swap chain for us
	return 1;
}

uint32_t GL4SwapChain::Aquire()
{
	return 0;
}

uint32_t GL4SwapChain::GetCurrentIndex()
{
	return 0;
}

void GL4Queue::Submit(shared_ptr<CommandBuffer> commandBuffer)
{ 
	(void)commandBuffer;
}

const DeviceInfo &GL4Device::GetDeviceInfo() const
{
	return deviceInfo;
}

vector< shared_ptr<Queue> > GL4Device::GetQueues()
{
	shared_ptr<Queue> queue = GetDisplayQueue();
	if (queue != nullptr)
	{
		return { GetDisplayQueue() };
	}
	else
	{
		return { };
	}
}

shared_ptr<SwapChain> GL4Device::CreateSwapChain(uint32_t preferredLength)
{
	// Swap chain length is always 1 for the GL backend
	(void)preferredLength;
	return make_shared<GL4SwapChain>(*this);
}

shared_ptr<FrameBuffer> GL4Device::CreateFrameBuffer()
{
	return make_shared<GL4FrameBuffer>(*this);
}

shared_ptr<CommandBuffer> GL4Device::CreateCommandBuffer()
{
	return make_shared<GL4CommandBuffer>(*this);
}

shared_ptr<RasterizationShader> GL4Device::CreateRasterizationShader(
	string name)
{
	return make_shared<GL4RasterizationShader>(*this, name);
}

shared_ptr<Buffer> GL4Device::CreateBuffer(
	BufferType bufferType,
	BufferUsageType accessType,
	BufferUsageFrequency accessFrequency,
	uint32_t size)
{
	return make_shared<GL4Buffer>(
		*this, bufferType, accessType, accessFrequency, size);
}

shared_ptr<Texture> GL4Device::CreateTexture(
	uvec2 imageSize,
	TextureFormat textureFormat)
{
	return make_shared<GL4Texture>(*this, imageSize, textureFormat);
}
