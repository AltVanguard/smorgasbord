#ifndef SMORGASBORD_WIDGET_HPP
#define SMORGASBORD_WIDGET_HPP

#include <SDL.h>

/// TODO: widgets are supposed to be in a hierarchy. Setup() shouldn't refer
///		to Window
/// TODO: rework HandleEvent(), currently:
///		- does not signal if the event is already handled
///		- does not support bubbling of other event flow

namespace Smorgasbord {

class Window;

class Widget
{
public:
	//virtual void Setup(Window *window) = 0;
	virtual void Draw() = 0;
	virtual void HandleEvent(SDL_Event windowEvent) = 0;
};

}

#endif // SMORGASBORD_WIDGET_HPP
