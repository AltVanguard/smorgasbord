#ifndef SMORGASBORD_VIEWPORT_HPP
#define SMORGASBORD_VIEWPORT_HPP

#include "transform.hpp"

#include <glm/glm.hpp>

// TODO: viewport offset
// TODO: CROP mode

namespace Smorgasbord {

enum class ViewportMode 
{
	Stretch,
	Letterbox,
	Crop
};

class Viewport
{
public:
	ViewportMode mode;
	int width;
	int height;
	float targetAspectRatio;
	
	// calculated
	float viewportAspectRatio;
	glm::vec2 factor;
	glm::mat4 viewport;
	glm::mat4 area;
	
public:
	Viewport();
	Viewport(
		int width,
		int height,
		float targetAspectRatio,
		ViewportMode mode = ViewportMode::Letterbox);
	Viewport(glm::vec2 offset, glm::vec2 areaSize, glm::vec2 windowSize);
	
	void SetViewport(
		int width,
		int height,
		float targetAspectRatio,
		ViewportMode mode = ViewportMode::Letterbox);
	void ResetViewport();
	void SetArea(glm::vec2 offset, glm::vec2 areaSize, glm::vec2 windowSize);
	void SetArea(glm::vec2 scale, glm::vec2 offset);
	void ResetArea();
	
	inline glm::mat4 GetMatrix()
	{
		return viewport * area;
	}
};

}

#endif // SMORGASBORD_VIEWPORT_HPP
