#ifndef MAINSCENEWIDGET_H
#define MAINSCENEWIDGET_H

#include <smorgasbord/gpu/gpuapi.hpp>
#include <smorgasbord/rendering/transform.hpp>
#include <smorgasbord/util/log.hpp>
#include <smorgasbord/util/timer.hpp>
#include <smorgasbord/window/framescheduler.hpp>

#include <SDL.h>
#include <glm/glm.hpp>

#include <iostream>
#include <map>
#include <memory>

using namespace std;
using namespace glm;
using namespace Smorgasbord;

class MainWidget
{
	struct Internal;
	unique_ptr<Internal> internal;
	
	ivec2 size;
	
public:
	MainWidget(ivec2 logicalSize);
	~MainWidget();
	
public:
	void Setup(shared_ptr<Device> device);
	
	void Draw();
	void HandleEvent(SDL_Event windowEvent);
	
private:
	void Render();
};

#endif // MAINSCENEWIDGET_H
