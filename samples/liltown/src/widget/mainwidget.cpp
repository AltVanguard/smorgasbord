#include "mainwidget.hpp"

#include <smorgasbord/gpu/gpuapi.hpp>
#include <smorgasbord/import/loadobj.hpp>
#include <smorgasbord/import/loadtexture.hpp>
#include <smorgasbord/rendering/camera.hpp>
#include <smorgasbord/util/resourcemanager.hpp>
#include <smorgasbord/window/cameracontroller.hpp>

#include <memory>

/*

# Explanations for shortened variables #

r - ResourceManager
m - Member variables
d - Device
q - Queue
c - CommandBuffer
t - Time

Shader variable prefixes:

c_* - Constant (from a constant/uniform buffer or a root/push constant)
s_* - Sampler
a_* - Attachment

Vertex Attributes:

v_p - Position
v_n - Normal
v_uv* - Texture UV coordniate
v_c - Color

*/

using namespace std;
using namespace glm;
using namespace Smorgasbord;

extern shared_ptr<ResourceManager> dataRM;
shared_ptr<ResourceManager> dataRM = make_shared<ResourceManager>("data/");

struct MainWidget::Internal : InternalBase
{
	struct MeshParams : public ParameterBuffer
	{
		SMORGASBORD_FIELD(mat4, c_mvp);
		SMORGASBORD_FIELD(float, c_t) = 0;
	} meshParams;
	
	struct MeshSamplers : public TextureSamplerSet
	{
		SMORGASBORD_SAMPLER(float 4, s_texture, stages: f, filter: llrrr);
	} meshSamplers;
	
	struct BlitSamplers : public TextureSamplerSet
	{
		SMORGASBORD_SAMPLER(float 4, s_texture, stages: f, filter: llccc);
	} blitSamplers;

	struct MeshPass : public Pass
	{
		SMORGASBORD_COLOR_ATTACHMENT(vec4, a_color);
		SMORGASBORD_DEPTH_ATTACHMENT(float, a_depth);
	} meshPass;
	
	struct BlitPass : public Pass
	{
		SMORGASBORD_COLOR_ATTACHMENT(vec4, a_color);
	} blitPass;
	
	shared_ptr<RasterizationShader> meshShader =
		d->CreateRasterizationShader("mesh");
	shared_ptr<RasterizationShader> blitShader =
		d->CreateRasterizationShader("blit");
	
	shared_ptr<FrameBuffer> offscreenFrame = d->CreateFrameBuffer();
	
	shared_ptr<StaticMesh> mesh;
	shared_ptr<Texture> testTexture;
	
	Camera camera;
	FlyCameraController camController = &camera;
	
	shared_ptr<Queue> q;
	vector< shared_ptr<CommandBuffer> > commandBuffers;
	shared_ptr<SwapChain> swapChain;
	vector<shared_ptr<FrameBuffer>> displayFrames;
	
	FrameScheduler scheduler;
	
	float t = 0.0f;
	
	Internal(shared_ptr<ResourceManager> _r, shared_ptr<Device> _d)
		: InternalBase(_r, _d)
	{ }
};

MainWidget::MainWidget(ivec2 logicalSize)
{
	this->size = logicalSize;
}

MainWidget::~MainWidget()
{ }

void MainWidget::Setup(shared_ptr<Device> device)
{
	internal = make_unique<MainWidget::Internal>(dataRM, device);
	Internal &m = *internal;
	
	m.q = m.d->GetDisplayQueue();
	AssertF(m.q != nullptr, "No display queue");
	
	m.camera.SetPerspective(16.0f / 9.0f, 90.0f, 0.01f, 100.0f);
	m.camera.SetView(vec3(0,1,0), vec3(0,0,0));
	
	m.mesh = make_shared<StaticMesh>(
		m.d, LoadOBJ(m.r->Get("town.obj")));
	m.testTexture = LoadTexture(m.d, m.r->GetPath("wtf.png"));
	
	m.meshShader->SetSource(m.r->Get("mesh.shader"));
	m.blitShader->SetSource(m.r->Get("blit.shader"));
	
	uvec2 rtSize = uvec2(1280, 720);
	m.offscreenFrame->SetColor(
		0, m.d->CreateTexture(rtSize, TextureFormat::RGBA_8_8_8_8_UNorm));
	m.offscreenFrame->SetDepth(
		m.d->CreateTexture(rtSize, TextureFormat::Depth_24_UNorm));
	
	m.swapChain = m.d->CreateSwapChain();
	m.commandBuffers = m.d->CreateCommandBuffers(m.swapChain->GetLength());
	m.displayFrames = m.swapChain->GetFrameBuffers();
	
	// Try pressing or holding the Space key when in FadeOnDemand mode
	m.scheduler.SetFrameScheduleMode(FrameScheduleMode::FadeOnDemand);
}

void MainWidget::Render()
{
	Internal &m = *internal;
	
	uint32_t swapChainIndex = m.swapChain->Aquire();
	CommandBuffer &c = *m.commandBuffers[swapChainIndex];
	shared_ptr<FrameBuffer> displayFrame = m.displayFrames[swapChainIndex];
	
	m.t += 0.06f;
	
	
	c.SetFrameBuffer(m.offscreenFrame);
	
	m.meshPass.a_color.SetToClear(vec4(0.7f, 0.7f, 1.0f, 1.0f));
	m.meshPass.a_depth.SetToClear();
	c.StartPass(m.meshPass);
	
	RasterizationPipelineState meshPS;
	meshPS.depthTest.isEnabled = true;
	meshPS.blend.isEnabled = false;
	meshPS.viewport = {
		0, 0, m.offscreenFrame->GetSize().x, m.offscreenFrame->GetSize().y };
	
	c.SetPipeline(m.meshShader, m.mesh->GetGeometryLayout(), meshPS);
	
	m.meshSamplers.s_texture = m.testTexture;
	m.meshShader->Set(m.meshSamplers);
	
	m.meshParams.c_mvp =
		m.camera.GetProjectionMatrix()
		* RotateAxisAngle(vec3(0,1,0), m.t * -0.02f)
		* m.camera.GetViewMatrix();
	m.meshParams.c_t = m.t;
	m.meshShader->Set(m.meshParams, SetOp::Constants, "a");
	
	c.Draw(m.mesh->GetGeometry());
	
	
	c.SetFrameBuffer(displayFrame);
	
	m.blitPass.a_color.SetToClear(vec4(1.0f, 0.7f, 0.7f, 1.0f));
	c.StartPass(m.blitPass);
	
	RasterizationPipelineState blitPS;
	blitPS.depthTest.isEnabled = false;
	blitPS.blend.isEnabled = false;
	blitPS.viewport = { 0, 0, this->size.x, this->size.y };
	
	c.SetPipeline(m.blitShader, { }, blitPS);
	
	m.blitSamplers.s_texture = m.offscreenFrame->GetColor(0);
	m.blitShader->Set(m.blitSamplers);
	
	c.Draw(nullptr, { }, 0, 3);
	
	
	m.q->Submit(m.commandBuffers[m.swapChain->GetCurrentIndex()]);
}

void MainWidget::Draw()
{
	Internal &m = *internal;
	
	if (m.scheduler.CanDraw())
	{
		Render();
		m.d->GetDisplayQueue()->Present();
	}
}

void MainWidget::HandleEvent(SDL_Event windowEvent)
{
	Internal &m = *internal;
	
	if (static_cast<bool>(internal)
		&& m.camController.HandleEvent(windowEvent))
	{
		m.scheduler.Invalidate();
		return;
	}
	
	switch (windowEvent.type)
	{
	case SDL_KEYDOWN:
		switch (windowEvent.key.keysym.scancode)
		{
		case SDL_SCANCODE_T:
			break;
		default:
			break;
		}
		break;
	}
}
