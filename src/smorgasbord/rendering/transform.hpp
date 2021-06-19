#ifndef SMORGASBORD_TRANSFORM_HPP
#define SMORGASBORD_TRANSFORM_HPP

#include <glm/glm.hpp>

namespace Smorgasbord {

// Axis Angle rotation (Rodriguez formula), takes radians
glm::mat4 RotateAxisAngle(glm::vec3 v, float angle);

// Euler rotation by XYZ, takes radians
glm::mat4 RotateXYZ(glm::vec3 angles);

// Translation
glm::mat4 Translate(glm::vec3 v);

// Scale
glm::mat4 Scale(glm::vec3 v);

// Center object
glm::mat4 Center(glm::vec3 boundingMin, glm::vec3 boundingMax);

// Normal transform matching the world transform given
glm::mat4 NormalTransform(glm::mat4& world);

glm::mat4 GetPerspectiveProjection(
	float sensorWidth,
	float sensorHeight,
	float focalLength,
	float clipNear,
	float clipFar);

// aspectRatio = sensorWidth/sensorHeight
glm::mat4 GetPerspectiveProjection(
	float aspectRatio,
	float horizontalFov,
	float clipNear,
	float clipFar);

glm::mat4 GetOrthographicProjection(
	float sensorWidth,
	float sensorHeight,
	float orthoSensorScale,
	float clipNear,
	float clipFar);

glm::mat4 GetOrthographicProjection(
	float aspectRatio,
	float orthoSensorScale,
	float clipNear,
	float clipFar);

}

#endif // SMORGASBORD_TRANSFORM_HPP
