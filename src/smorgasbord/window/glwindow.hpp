#ifndef SMORGASBORD_GLWINDOW_HPP
#define SMORGASBORD_GLWINDOW_HPP

#include "window.hpp"

#include <smorgasbord/gpu/gl4.hpp>

using namespace std;
using namespace glm;

namespace Smorgasbord {

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
	
public:
	SDLGL4Device(SDL_Window *_window)
		: window(_window)
	{ }
	
	virtual shared_ptr<Queue> GetDisplayQueue() override
	{
		return make_shared<SDLGL4Queue>(this->window);
	}
};


class GLWindow : public Window
{
	SDL_GLContext openglContext;
	
public:
	virtual void Init(
		int argc,
		char *argv[],
		uvec2 windowSize,
		const string &title) override;
	
	shared_ptr<Device> GetGL4Device()
	{
		return make_shared<SDLGL4Device>(this->window);
	}
	
protected:
	virtual void HandleEvent(SDL_Event windowEvent) override;
	virtual void CleanupLibs() override;
};

}

#endif // SMORGASBORD_GLWINDOW_HPP
