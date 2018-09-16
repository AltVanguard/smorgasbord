#ifndef SMORGASBORD_TRANSFORM_HPP
#define SMORGASBORD_TRANSFORM_HPP

#include <glm/glm.hpp>

using namespace glm;

namespace Smorgasbord {

// Axis Angle rotation (Rodriguez formula), takes radians
mat4 RotateAxisAngle(vec3 v, float angle);

// Euler rotation by XYZ, takes radians
mat4 RotateXYZ(vec3 angles);

// Translation
mat4 Translate(vec3 v);

// Scale
mat4 Scale(vec3 v);

// Center object
mat4 Center(vec3 boundingMin, vec3 boundingMax);

// Normal transform matching the world transform given
mat4 NormalTransform(mat4& world);

mat4 GetPerspectiveProjection(
	float sensorWidth,
	float sensorHeight,
	float focalLength,
	float clipNear,
	float clipFar);

// aspectRatio = sensorWidth/sensorHeight
mat4 GetPerspectiveProjection(
	float aspectRatio,
	float horizontalFov,
	float clipNear,
	float clipFar);

mat4 GetOrthographicProjection(
	float sensorWidth,
	float sensorHeight,
	float orthoSensorScale,
	float clipNear,
	float clipFar);

mat4 GetOrthographicProjection(
	float aspectRatio,
	float orthoSensorScale,
	float clipNear,
	float clipFar);

}

#endif // SMORGASBORD_TRANSFORM_HPP
