#ifndef SMORGASBORD_WINDOW_HPP
#define SMORGASBORD_WINDOW_HPP

#include "smorgasbord/util/timer.hpp"

#include <SDL.h>
#include <glm/glm.hpp>

#include <memory>

using namespace std;
using namespace glm;

namespace Smorgasbord {

class Widget;

class Window
{
protected:
	uvec2 windowSize;
	
	SDL_Window *window = nullptr;
	bool quitSignaled = false;
	
	Widget *mainWidget = nullptr;
	
public:
	virtual void Init(
		int argc, char *argv[], uvec2 windowSize, const string &title);
	void EnterMainLoop(shared_ptr<Widget> mainWidget);
	
	uvec2 GetWindowSize()
	{
		return windowSize;
	}
	
protected:
	virtual void HandleEvent(SDL_Event windowEvent);
	virtual void CleanupLibs();
};

}

#endif // SMORGASBORD_WINDOW_HPP
