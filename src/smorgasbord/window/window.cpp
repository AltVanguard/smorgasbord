#include "window.hpp"

#include "framescheduler.hpp"
#include "widget.hpp"

#include <smorgasbord/util/log.hpp>
#include <smorgasbord/util/timer.hpp>

#include <iostream>

void Smorgasbord::Window::Init(
	int argc, char *argv[], uvec2 windowSize, const string &title)
{
	(void)argc;
	(void)argv;
	this->windowSize = windowSize;
	
	// Init SDL
	
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		LogF("Cannot init SDL");
	}
	
	window = SDL_CreateWindow(
			title.c_str(),
			SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
			windowSize.x, windowSize.y,
			SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
	
	if (!window)
	{
		string errorMessage = string("SDL Error: ") + SDL_GetError();
		SDL_Quit();
		LogF(errorMessage);
	}
}

void Smorgasbord::Window::EnterMainLoop(shared_ptr<Widget> mainWidget)
{
	try
	{
		while (!quitSignaled)
		{
			mainWidget->Draw();
			
			SDL_Event windowEvent;
			while(SDL_PollEvent(&windowEvent))
			{
				// Handle widget events
				mainWidget->HandleEvent(windowEvent);
				
				HandleEvent(windowEvent);
			}
		}
		
		CleanupLibs();
	}
	catch (exception& e)
	{
		CleanupLibs();
		
		cout << "Fatal exception thrown" << endl;
		assert(false);
		throw e;
	}
}

void Smorgasbord::Window::CleanupLibs()
{
	SDL_DestroyWindow(window);
	SDL_Quit();
}

void Smorgasbord::Window::HandleEvent(SDL_Event windowEvent)
{
	switch (windowEvent.type)
	{
	case SDL_QUIT:
		quitSignaled = true;
		return;
	}
}
