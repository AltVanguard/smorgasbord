#ifndef SMORGASBORD_GLWINDOW_HPP
#define SMORGASBORD_GLWINDOW_HPP

#include <smorgasbord/window/window.hpp>

using namespace std;
using namespace glm;

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
	uvec2 windowSize;

	unique_ptr<SDL_Window, SdlWindowDeleter> window;
	unique_ptr<remove_pointer_t<SDL_GLContext>, SdlGLContextDeleter>
		glContext;
	
public:
	GLWindow(uvec2 _windowSize, const string &title);
	virtual ~GLWindow();
	
	void EnterMainLoop(
		std::function<void()> onDraw,
		std::function<void(SDL_Event windowEvent)> onEvent);
	
	shared_ptr<Device> GetGL4Device();
};

}

#endif // SMORGASBORD_GLWINDOW_HPP
