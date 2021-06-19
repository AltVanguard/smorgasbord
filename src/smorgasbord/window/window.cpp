#include "window.hpp"

#include "framescheduler.hpp"

#include <smorgasbord/util/log.hpp>
#include <smorgasbord/util/timer.hpp>

#include <iostream>

Smorgasbord::Window::Window(glm::uvec2 _windowSize, const std::string &title)
	: windowSize(_windowSize)
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		LogE("Cannot init SDL");
	}
	
	window = decltype(window){ SDL_CreateWindow(
			title.c_str(),
			SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
			windowSize.x, windowSize.y,
			SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE) };
	
	AssertE(window, std::string("SDL Error: ") + SDL_GetError());
}

Smorgasbord::Window::~Window()
{
	SDL_Quit();
}

void Smorgasbord::Window::EnterMainLoop(
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
		}
	}
}
