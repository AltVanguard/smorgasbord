#ifndef MAINSCENEWIDGET_H
#define MAINSCENEWIDGET_H

#include <smorgasbord/gpu/graphics.hpp>
#include <smorgasbord/rendering/transform.hpp>
#include <smorgasbord/util/log.hpp>
#include <smorgasbord/util/timer.hpp>
#include <smorgasbord/window/framescheduler.hpp>
#include <smorgasbord/window/widget.hpp>

#include <SDL.h>
#include <glm/glm.hpp>

#include <iostream>
#include <map>
#include <memory>

using namespace std;
using namespace glm;
using namespace Smorgasbord;

class MainWidget : public Widget
{
	struct Internal;
	unique_ptr<Internal> internal;
	
	ivec2 size;
	
	FrameScheduler scheduler;
	FrameTimeCounter counter;
	GL4TimeQuery timeQuery;
	
public:
	MainWidget(ivec2 logicalSize);
	~MainWidget();
	
public:
	void Setup(shared_ptr<Window> window, shared_ptr<Device> device);
	
	// Widget interface
	virtual void Draw() override;
	virtual void HandleEvent(SDL_Event windowEvent) override;
	
private:
	void Render();
};

#endif // MAINSCENEWIDGET_H
