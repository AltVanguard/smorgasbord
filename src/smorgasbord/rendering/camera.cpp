#include "camera.hpp"

#include <iostream>
using namespace std;

Smorgasbord::Camera::Camera()
	: projection(1.0f), view(1.0f)
{ }

mat4 Smorgasbord::Camera::GetProjectionMatrix()
{
	if (!isValid)
	{
		if (isPerspective)
		{
			projection = GetPerspectiveProjection(
				SensorWidth, SensorHeight, FocalLength, ClipNear, ClipFar);
		}
		else
		{
			projection = GetOrthographicProjection(
				SensorWidth,
				SensorHeight,
				OrthoSensorScale,
				ClipNear,
				ClipFar);
		}
		isValid = true;
	}
	
	return projection;
}

mat4 Smorgasbord::Camera::GetViewMatrix()
{
	return view;
}

void Smorgasbord::Camera::SetView(vec3 location, vec3 rotation)
{
	view = transpose(RotateXYZ(rotation)) * Translate(-location);
}

void Smorgasbord::Camera::SetView(vec3 location, vec3 up, vec3 direction)
{
	// TODO
}

void Smorgasbord::Camera::SetViewMatrix(const mat4 &mat)
{
	view = mat;
}

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
				Translate(vec3(0, 0, 0.1f)) * camera->GetViewMatrix());
			break;
		case SDL_SCANCODE_S:
		case SDL_SCANCODE_DOWN:
			camera->SetViewMatrix(
				Translate(vec3(0, 0, -0.1f)) * camera->GetViewMatrix());
			break;
		case SDL_SCANCODE_A:
		case SDL_SCANCODE_LEFT:
			camera->SetViewMatrix(
				Translate(vec3(0.1f, 0, 0)) * camera->GetViewMatrix());
			break;
		case SDL_SCANCODE_D:
		case SDL_SCANCODE_RIGHT:
			camera->SetViewMatrix(
				Translate(vec3(-0.1f, 0, 0)) * camera->GetViewMatrix());
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
			vec2 mousePos = vec2(
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
		vec2 mouseMotion = vec2(
			(float)windowEvent.motion.x, (float)windowEvent.motion.y);
		vec2 mousePos = vec2(
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
