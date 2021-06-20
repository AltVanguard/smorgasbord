#pragma once

#include <smorgasbord/window/window.hpp>

class SDLGL4Device;

namespace Smorgasbord {

class Device;

struct SdlGLContextDeleter
{
	void operator() (SDL_GLContext context)
	{
		SDL_GL_DeleteContext(context);
	}
};

class GLWindow
{
	glm::uvec2 windowSize;

	std::unique_ptr<SDL_Window, SdlWindowDeleter> window;
	std::unique_ptr<std::remove_pointer_t<SDL_GLContext>, SdlGLContextDeleter>
		glContext;
	std::shared_ptr<SDLGL4Device> device;
	
	bool quitSignaled = false;
	
public:
	GLWindow(glm::uvec2 _windowSize, const std::string &title);
	virtual ~GLWindow();
	
	void EnterMainLoop(
		std::function<void()> onDraw,
		std::function<void(SDL_Event windowEvent)> onEvent);
	
	std::shared_ptr<Device> GetGL4Device();
	
	void QuitMainLoop()
	{
		quitSignaled = true;
	}
};

}
