#include "cameracontroller.hpp"

Smorgasbord::CameraController::CameraController(Smorgasbord::Camera *_camera)
	: camera(_camera)
{ }

Smorgasbord::CameraController::~CameraController()
{ }

Smorgasbord::FlyCameraController::FlyCameraController(
	Smorgasbord::Camera *_camera)
	: CameraController(_camera)
{ }

bool Smorgasbord::FlyCameraController::HandleEvent(SDL_Event windowEvent)
{
	switch (windowEvent.type)
	{
	case SDL_KEYDOWN:
	{
		switch (windowEvent.key.keysym.scancode)
		{
		case SDL_SCANCODE_W:
		case SDL_SCANCODE_UP:
			camera->SetViewMatrix(
				Translate(glm::vec3(0, 0, 0.1f)) * camera->GetViewMatrix());
			break;
		case SDL_SCANCODE_S:
		case SDL_SCANCODE_DOWN:
			camera->SetViewMatrix(
				Translate(glm::vec3(0, 0, -0.1f)) * camera->GetViewMatrix());
			break;
		case SDL_SCANCODE_A:
		case SDL_SCANCODE_LEFT:
			camera->SetViewMatrix(
				Translate(glm::vec3(0.1f, 0, 0)) * camera->GetViewMatrix());
			break;
		case SDL_SCANCODE_D:
		case SDL_SCANCODE_RIGHT:
			camera->SetViewMatrix(
				Translate(glm::vec3(-0.1f, 0, 0)) * camera->GetViewMatrix());
			break;
		case SDL_SCANCODE_SPACE:
			// invalidate on space
			break;
		default:
			return false;
		}
		return true;
	}
	
	///case SDL_KEYUP:
	///	switch (windowEvent.key.keysym.scancode)
	///	{
	///	case SDL_SCANCODE_W:
	///		break;
	///	default:
	///		return false;
	///	}
	///	return true;
	///
	///case SDL_MOUSEWHEEL:
	///	scale *= glm::pow(1.5f, (float)windowEvent.wheel.y);
	///	break;
		
	case SDL_MOUSEBUTTONDOWN:
		switch (windowEvent.button.button)
		{
		case SDL_BUTTON_LEFT:
		{
			glm::vec2 mousePos = glm::vec2(
				(float)windowEvent.button.x, (float)windowEvent.button.y);
			break;
		}
		default:
			return false;
		}
		return true;
		
	case SDL_MOUSEBUTTONUP:
		switch (windowEvent.button.button)
		{
		case SDL_BUTTON_LEFT:
			break;
		case SDL_BUTTON_RIGHT:
			break;
		default:
			return false;
		}
		return true;
		
	case SDL_MOUSEMOTION:
	{
		glm::vec2 mouseMotion = glm::vec2(
			(float)windowEvent.motion.x, (float)windowEvent.motion.y);
		glm::vec2 mousePos = glm::vec2(
			(float)windowEvent.button.x, (float)windowEvent.button.y);
		///cout << mousePos.x << " " << mousePos.y << " "
		/// << mouseMotion.x << " " << mouseMotion.y << endl;
		return false;
	}
	default:
		return false;
	}
	return true;
}
