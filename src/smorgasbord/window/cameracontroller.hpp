#ifndef SMORGASBORD_CAMERACONTROLLER_HPP
#define SMORGASBORD_CAMERACONTROLLER_HPP

#include <smorgasbord/rendering/camera.hpp>

#include <SDL2/SDL.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <cmath>
#include <string>
#include <vector>

namespace Smorgasbord {

class CameraController
{
protected:
	Camera *camera;
	
public:
	CameraController(Camera *camera);
	virtual ~CameraController();
	
	virtual bool HandleEvent(SDL_Event windowEvent) = 0;
};

class FlyCameraController : public CameraController
{
private:
	glm::mat4 oldView;
	
public:
	FlyCameraController(Camera *camera);
	
	// CameraController interface
	bool HandleEvent(SDL_Event windowEvent);
};

}

#endif // SMORGASBORD_CAMERACONTROLLER_HPP
