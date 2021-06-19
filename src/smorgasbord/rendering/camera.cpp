#include "camera.hpp"

#include <glm/ext.hpp>

#include <iostream>

Smorgasbord::Camera::Camera()
	: projection(1.0f), view(1.0f)
{ }

glm::mat4 Smorgasbord::Camera::GetProjectionMatrix()
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

glm::mat4 Smorgasbord::Camera::GetViewMatrix()
{
	return view;
}

void Smorgasbord::Camera::SetView(glm::vec3 location, glm::vec3 rotation)
{
	view = transpose(RotateXYZ(rotation)) * Translate(-location);
}

void Smorgasbord::Camera::SetView(glm::vec3 location, glm::vec3 up, glm::vec3 direction)
{
	// TODO
}

void Smorgasbord::Camera::SetViewMatrix(const glm::mat4 &mat)
{
	view = mat;
}
