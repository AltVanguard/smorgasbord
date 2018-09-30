#ifndef SMORGASBORD_GLWINDOW_HPP
#define SMORGASBORD_GLWINDOW_HPP

#include "window.hpp"

using namespace std;
using namespace glm;

namespace Smorgasbord {

class Device;

class GLWindow : public Window
{
	SDL_GLContext openglContext;
	
public:
	virtual void Init(
		int argc,
		char *argv[],
		uvec2 windowSize,
		const string &title) override;
	
	shared_ptr<Device> GetGL4Device();
	
protected:
	virtual void HandleEvent(SDL_Event windowEvent) override;
	virtual void CleanupLibs() override;
};

}

#endif // SMORGASBORD_GLWINDOW_HPP
