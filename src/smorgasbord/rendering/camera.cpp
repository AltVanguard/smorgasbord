#include "camera.hpp"

#include <glm/ext.hpp>

#include <iostream>

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
