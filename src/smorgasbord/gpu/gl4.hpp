#ifndef SMORGASBORD_GL4_BACKEND_H
#define SMORGASBORD_GL4_BACKEND_H

#include <smorgasbord/gpu/gpuapi.hpp>

#include <GL/glew.h>

#include <unordered_map>
#include <set>

/// TODO: GL4CommandBuffer::Draw() to support different GeometryLayouts over
///			the same vertex buffer. Need to expand GL4VAOKey
/// TODO: stencil buffer handling/clearing
/// TODO: set some debug name to GraphicsShader::name if possible

using namespace glm;
using namespace std;

#define SMORGASBORD_GL4_UNBIND

namespace Smorgasbord {

class GL4Device;

class GL4Buffer : public Buffer
{
	GLuint nativeDeviceBufferID = 0;
	void *mappedData = nullptr;// uint8_t
	
public:
	GL4Buffer(
		BufferType bufferType, 
		BufferUsageType accessType, 
		BufferUsageFrequency accessFrequency, 
		int size);
		
	// ShaderBuffer interface
	virtual uint8_t *GetMappedData() override;
	virtual void Map(MappedDataAccessType mapAccessType) override;
	virtual void Unmap() override;
	
	GLuint GetID() const
	{
		return nativeDeviceBufferID;
	}
	
	bool IsValid() const
	{
		return nativeDeviceBufferID > 0;
	}
};

struct GL4IndexBufferRef
{
	shared_ptr<GL4Buffer> buffer;
	GLenum dataType = GL_NONE;
	
	GL4IndexBufferRef();
	GL4IndexBufferRef(IndexBufferRef &indexBuffer);
	
	bool IsValid()
	{
		return buffer != nullptr;
	}
};

struct GL4TextureFormat
{
	GLint internalFormat = GL_RGBA;
	GLenum format = GL_RGBA;
	GLenum dataType = GL_UNSIGNED_BYTE;
	
	GL4TextureFormat(GLint _internalFormat, GLenum _format, GLenum _dataType)
		: internalFormat(_internalFormat)
		, format(_format)
		, dataType(_dataType)
	{ }
};

class GL4Texture : public Texture
{
private:
	GLuint id = 0;
	int bindSlot = -1;
	
public:
	GL4Texture(uvec2 imageSize, TextureFormat textureFormat);
	~GL4Texture();
	
	void Init();
	void Free();
	
	void Bind(int slot);
	void Unbind();
	
	/// Wrap and filter are properties of the texturing unit, not the texture,
	/// thus they need to be set every time a texture is bound to a slot.
	/// Possible values: GL_NEAREST, GL_LINEAR
	void SetTextureFilter(SamplerFilter minify, SamplerFilter magnify);
	/// Possible values: GL_CLAMP, GL_REPEAT, GL_MIRRORED_REPEAT
	void SetTextureWrap(SamplerWrap s, SamplerWrap t, SamplerWrap r);
	
	// Texture interface
	virtual void Upload(Image &image) override;
	virtual void Verify(Image &image) override;
	virtual shared_ptr<Image> Download() override;
	
	GLuint GetID()
	{
		return id;
	}
	
	bool IsReady()
	{
		return id != 0;
	}
};

struct GL4BindCommand
{
	SetOp setOp = SetOp::Invalidate;
	RasterizationStageFlag stageFlag = RasterizationStageFlag::None;
	
	GL4BindCommand(
		SetOp _setOp,
		RasterizationStageFlag _stageFlag)
		: setOp(_setOp), stageFlag(_stageFlag)
	{ }
};

class GL4RasterizationShader : public RasterizationShader
{
private:
	TextureSamplerSet *samplers = nullptr;
	map<ParameterBuffer*, GL4BindCommand> parameterBuffers;
	map<Buffer*, string> buffers;
	map<RasterizationStage, GLuint> sourceIDs;
	GLuint programID = 0;
	set<string> enumerableConstants;
	/// Changes to the source are no longer allowed after compilation, such as
	/// calling Set() with a new ParameterBuffer of calling AddText()
	/// For these call Clone() or create a shader from scratch
	bool isCompiled = false;
	
public:
	GL4RasterizationShader(string name = "");
	
	void ResetBindings();
	void ApplyBindings(GL4Device *device);
	void Compile(const Pass &pass, const GeometryLayout &geometryLayout);
	void PrintErrorLog(GLuint sourceID, GLuint programID);
	void Use();
	
	// GraphicsShader interface
	virtual void Set(TextureSamplerSet &samplers) override;
	virtual void Set(
		ParameterBuffer &buffer,
		SetOp setOp,
		RasterizationStageFlag stageFlag) override;
	
	uint32_t GetNumSamplers()
	{
		return samplers != nullptr && isCompiled
			? (uint32_t)samplers->GetSamplers().size()
			: 0;
	}
};

struct GL4VAOKey
{
	GLuint vertexBufferID = 0;
	GLuint indexBufferID = 0;
	
	GL4VAOKey()
	{ }
	
	GL4VAOKey(GLuint _vertexBufferID, GLuint _indexBufferID)
		: vertexBufferID(_vertexBufferID), indexBufferID(_indexBufferID)
	{ }
	
	bool operator==(const GL4VAOKey &b) const
	{
		return vertexBufferID == b.vertexBufferID
			&& indexBufferID == b.indexBufferID;
	}
};

struct GL4VAOKeyHash
{
	size_t operator()(const GL4VAOKey &key) const
	{
		return key.indexBufferID ^ (key.vertexBufferID << 10);
	}
};

class IGL4FrameBuffer : public FrameBuffer
{
public:
	virtual GLuint GetID() = 0;
	virtual void Use() = 0;
	virtual void SetDrawBuffers() = 0;
};

class GL4CommandBuffer : public CommandBuffer
{
	bool isFirstRun = true;
	unordered_map<GL4VAOKey, GLuint, GL4VAOKeyHash> vaos;
	GL4Device *device = nullptr; // TODO: change to shared_ptr
	shared_ptr<IGL4FrameBuffer> frameBuffer;
	const Pass *passAddress = nullptr;
	shared_ptr<GL4RasterizationShader> shader;
	GeometryLayout geometryLayout;
	GraphicsPipelineState pipelineState;
	
public:
	GL4CommandBuffer(GL4Device *device);
	~GL4CommandBuffer();
	
	// CommandBuffer interface
	virtual void SetFrameBuffer(shared_ptr<FrameBuffer> framebuffer) override;
	virtual void StartPass(const Pass &pass) override;
	virtual void SetPipeline(
		shared_ptr<RasterizationShader> shader,
		const GeometryLayout &geometryLayout,
		const GraphicsPipelineState &pipelineState) override;
	virtual void Draw(
		shared_ptr<Buffer> vertexBuffer,
		IndexBufferRef indexBuffer,
		uint32_t startIndex,
		uint32_t numVertices,
		uint32_t numInstances) override;
};

class GL4FrameBuffer : public IGL4FrameBuffer
{
private:
	GLuint id = 0;
	
public:
	GL4FrameBuffer();
	~GL4FrameBuffer();
	
	bool IsReady(); // TODO?: add to IGL4FrameBuffer
	
	// FrameBuffer interface
	virtual void SetColor(
		uint32_t attachementIndex,
		shared_ptr<Texture> color) override;
	virtual void SetDepth(shared_ptr<Texture> depth) override;
	
	// IGL4FrameBuffer interface
	virtual GLuint GetID() override;
	virtual void Use() override;
	virtual void SetDrawBuffers() override;
};

class GL4SystemFrameBuffer : public IGL4FrameBuffer
{
public:
	// FrameBuffer interface
	virtual void SetColor(
		uint32_t attachmentIndex,
		shared_ptr<Texture> color) override;
	virtual void SetDepth(shared_ptr<Texture> depth) override;
	
	// IGL4FrameBuffer interface
	virtual GLuint GetID() override;
	virtual void Use() override;
	virtual void SetDrawBuffers() override;
};

class GL4SwapChain : public SwapChain
{
	vector<shared_ptr<FrameBuffer>> frameBuffers;
	
public:
	GL4SwapChain();
	
	// SwapChain interface
	virtual vector<shared_ptr<FrameBuffer>> GetFrameBuffers() override;
	virtual uint32_t GetLength() override;
	virtual uint32_t Aquire() override;
	virtual uint32_t GetCurrentIndex() override;
};

class GL4Queue : public Queue
{
public:
	// Queue interface
	virtual void Submit(shared_ptr<CommandBuffer> commandBuffer) override;
	//virtual void Present() override { }
};

class GL4Device : public Device
{
protected:
	DeviceInfo deviceInfo;
	
public:
	// Device interface
	virtual const DeviceInfo &GetDeviceInfo() const override;
	virtual vector<shared_ptr<Queue>> GetQueues() override;
	//virtual shared_ptr<Queue> GetDisplayQueue() override;
	virtual shared_ptr<SwapChain> CreateSwapChain(
		uint32_t preferredLength = 0) override;
	virtual shared_ptr<FrameBuffer> CreateFrameBuffer() override;
	virtual shared_ptr<CommandBuffer> CreateCommandBuffer() override;
	virtual shared_ptr<RasterizationShader> CreateRasterizationShader(
		string name = "") override;
	virtual shared_ptr<Buffer> CreateBuffer(
		BufferType bufferType,
		BufferUsageType accessType,
		BufferUsageFrequency accessFrequency,
		uint32_t size) override;
	virtual shared_ptr<Texture> CreateTexture(
		uvec2 imageSize,
		TextureFormat textureFormat) override;
};

class GL4Backend : public Backend
{
	/// For now, OpenGL initialization is handled by GLWindow through SDL.
	/// See GLWindow::GetGL4Device()
	// TODO: also add manual GL4Device instantiation not relying on SDL
};

}

#endif // SMORGASBORD_GL4_BACKEND_H
