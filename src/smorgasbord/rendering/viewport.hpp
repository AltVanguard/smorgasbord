#ifndef SMORGASBORD_VIEWPORT_HPP
#define SMORGASBORD_VIEWPORT_HPP

#include "transform.hpp"

#include <glm/glm.hpp>

// TODO: viewport offset
// TODO: CROP mode

using namespace glm;
using namespace std;

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
	vec2 factor;
	mat4 viewport;
	mat4 area;
	
public:
	Viewport();
	Viewport(
		int width,
		int height,
		float targetAspectRatio,
		ViewportMode mode = ViewportMode::Letterbox);
	Viewport(vec2 offset, vec2 areaSize, vec2 windowSize);
	
	void SetViewport(
		int width,
		int height,
		float targetAspectRatio,
		ViewportMode mode = ViewportMode::Letterbox);
	void ResetViewport();
	void SetArea(vec2 offset, vec2 areaSize, vec2 windowSize);
	void SetArea(vec2 scale, vec2 offset);
	void ResetArea();
	
	inline mat4 GetMatrix()
	{
		return viewport * area;
	}
};

}

#endif // SMORGASBORD_VIEWPORT_HPP
