#ifndef SMORGASBORD_WINDOW_HPP
#define SMORGASBORD_WINDOW_HPP

#include <SDL2/SDL.h>
#include <glm/glm.hpp>

#include <memory>
#include <string>
#include <functional>

using namespace std;
using namespace glm;

namespace Smorgasbord {

struct SdlWindowDeleter
{
	void operator() (SDL_Window* window)
	{
		SDL_DestroyWindow(window);
	}
};

class Window
{
	uvec2 windowSize;

	unique_ptr<SDL_Window, SdlWindowDeleter> window;
	
public:
	Window(uvec2 _windowSize, const string &title);
	virtual ~Window();
	
	void EnterMainLoop(
		std::function<void()> onDraw,
		std::function<void(SDL_Event windowEvent)> onEvent);
	
	uvec2 GetWindowSize()
	{
		return windowSize;
	}
};

}

#endif // SMORGASBORD_WINDOW_HPP
