#include "glwindow.hpp"

#include <smorgasbord/gpu/gl4.hpp>
#include <smorgasbord/util/log.hpp>
#include <smorgasbord/util/timer.hpp>

#include <smorgasbord/gpu/gl4_loader.hpp>

#include <iostream>

using namespace Smorgasbord;

#ifdef WIN32

static void APIENTRY ShowDebugOutput(
	GLenum source, GLenum type, GLuint id, GLenum severity,
	GLsizei length, const GLchar *message, const GLvoid *userParam)
{
	(void)source;
	(void)type;
	(void)id;
	(void)severity;
	(void)length;
	(void)userParam;
	LogI("GL MESSAGE: {0}", message);
}

#else

static void ShowDebugOutput(
	GLenum source, GLenum type, GLuint id, GLenum severity,
	GLsizei length, const GLchar *message, const GLvoid *userParam)
{
	LogI("GL MESSAGE: " + message);
}

#endif

class SDLGL4Queue : public GL4Queue
{
	SDL_Window *window = nullptr;
	
public:
	SDLGL4Queue(SDL_Window *_window)
		: window(_window)
	{ }
	
	virtual void Present() override
	{
		SDL_GL_SwapWindow(window);
	}
};

class SDLGL4Device : public GL4Device
{
	SDL_Window *window = nullptr;
	SDL_GLContext context = nullptr;
	
public:
	SDLGL4Device(SDL_Window *_window, SDL_GLContext _context)
		: window(_window)
		, context(_context)
	{ }
	
	virtual shared_ptr<Queue> GetDisplayQueue() override
	{
		return make_shared<SDLGL4Queue>(this->window);
	}
	
	void MakeCurrent()
	{
		SDL_GL_MakeCurrent(window, context);
	}
};

GLWindow::GLWindow(uvec2 _windowSize, const string &title)
	: windowSize(_windowSize)
{
	// Init SDL
	
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		LogE("Cannot init SDL");
	}
	
	//SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
	SDL_GL_SetAttribute(
		SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	// minimal depth buffer bits. defaults to 16
	//SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	
#if 1 //defined(DEBUG)
	//SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
#endif
	
	window = decltype(window){ SDL_CreateWindow(
			title.c_str(),
			SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
			windowSize.x, windowSize.y,
			SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE) };
	
	AssertE(window, string("SDL Error: ") + SDL_GetError());
	
	glContext = decltype(glContext){ SDL_GL_CreateContext(window.get()) };
	
	device = make_shared<SDLGL4Device>(window.get(), glContext.get());
	device->MakeCurrent();
	
	//SDL_GL_SetSwapInterval(1); // TODO: provide setting to enable/disable
}

GLWindow::~GLWindow()
{
	SDL_Quit();
}

shared_ptr<Device> GLWindow::GetGL4Device()
{
	return device;
}

void GLWindow::EnterMainLoop(
	std::function<void()> onDraw,
	std::function<void(SDL_Event windowEvent)> onEvent)
{
	bool quitSignaled = false;
	while (!quitSignaled)
	{
		onDraw();
		
		SDL_Event windowEvent;
		while(SDL_PollEvent(&windowEvent))
		{
			onEvent(windowEvent);
			
			if (windowEvent.type == SDL_QUIT)
			{
				quitSignaled = true;
			}
			else if (windowEvent.type == SDL_WINDOWEVENT
				&& windowEvent.window.event == SDL_WINDOWEVENT_RESIZED)
			{
				windowSize.x = windowEvent.window.data1;
				windowSize.y = windowEvent.window.data2;
				device->GetLoader().glViewport(
					0, 0, windowSize.x, windowSize.y
					);
			}
		}
	}
}
