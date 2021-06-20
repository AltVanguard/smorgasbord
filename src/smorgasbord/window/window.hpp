#pragma once

#include <SDL2/SDL.h>
#include <glm/glm.hpp>

#include <memory>
#include <string>
#include <functional>

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
	glm::uvec2 windowSize;

	std::unique_ptr<SDL_Window, SdlWindowDeleter> window;
	
public:
	Window(glm::uvec2 _windowSize, const std::string &title);
	virtual ~Window();
	
	void EnterMainLoop(
		std::function<void()> onDraw,
		std::function<void(SDL_Event windowEvent)> onEvent);
	
	glm::uvec2 GetWindowSize()
	{
		return windowSize;
	}
};

}
