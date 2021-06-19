#include "viewport.hpp"

#include <smorgasbord/util/log.hpp>

Smorgasbord::Viewport::Viewport()
	: viewport(1.0f), area(1.0f)
{ }

Smorgasbord::Viewport::Viewport(
	int width,
	int height,
	float targetAspectRatio,
	Smorgasbord::ViewportMode mode)
{
	SetViewport(width, height, targetAspectRatio, mode);
}

Smorgasbord::Viewport::Viewport(glm::vec2 offset, glm::vec2 areaSize, glm::vec2 windowSize)
{
	SetArea(offset, areaSize, windowSize);
}

void Smorgasbord::Viewport::SetViewport(
	int width,
	int height,
	float targetAspectRatio,
	Smorgasbord::ViewportMode mode)
{
	this->width = width;
	this->height = height;
	this->targetAspectRatio = targetAspectRatio;
	this->mode = mode;
	
	viewportAspectRatio = (float)width / (float)height;
	
	switch (mode)
	{
	case ViewportMode::Stretch:
		factor = glm::vec2(1.0f, 1.0f);
		break;
		
	case ViewportMode::Letterbox:
		if (viewportAspectRatio > targetAspectRatio)
		{
			factor = glm::vec2(targetAspectRatio / viewportAspectRatio,1);
		}
		else
		{
			factor = glm::vec2(1, viewportAspectRatio / targetAspectRatio);
		}
		break;
		
	case ViewportMode::Crop: // TODO
		LogF("CROP mode not implemented");
		break;
	}
	
	viewport = glm::mat4(1.0f);
	viewport[0][0] = factor.x;
	viewport[1][1] = factor.y;
}

void Smorgasbord::Viewport::ResetViewport()
{
	viewport = glm::mat4(1.0f);
}

void Smorgasbord::Viewport::SetArea(
	glm::vec2 offset, glm::vec2 areaSize, glm::vec2 windowSize)
{
	area = glm::mat4(1.0f);
	area[0][0] = areaSize.x / windowSize.x;
	area[1][1] = areaSize.y / windowSize.y;
	area[3][0] = areaSize.x / windowSize.x - 1.0f + offset.x / windowSize.x;
	area[3][1] = areaSize.y / windowSize.y - 1.0f + offset.y / windowSize.y;
}

void Smorgasbord::Viewport::SetArea(glm::vec2 scale, glm::vec2 offset)
{
	area = Scale(glm::vec3(scale, 1)) * Translate(glm::vec3(offset, 0));
}

void Smorgasbord::Viewport::ResetArea()
{
	area = glm::mat4(1.0f);
}



