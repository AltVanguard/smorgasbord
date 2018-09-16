#include "transform.hpp"

mat4 Smorgasbord::RotateAxisAngle(vec3 v, float angle)
{
	mat4 result(1.0f);
	
	float s = sin(angle);
	float c = cos(angle);
	float ci = 1 - c;
	
	result[0][0] = c        + ci * v[0] * v[0];
	result[1][0] = -v.z * s + ci * v[0] * v[1];
	result[2][0] =  v.y * s + ci * v[0] * v[2];
	
	result[0][1] =  v.z * s + ci * v[1] * v[0];
	result[1][1] = c        + ci * v[1] * v[1];
	result[2][1] = -v.x * s + ci * v[1] * v[2];
	
	result[0][2] = -v.y * s + ci * v[2] * v[0];
	result[1][2] =  v.x * s + ci * v[2] * v[1];
	result[2][2] = c        + ci * v[2] * v[2];
	
	return result;
}

mat4 Smorgasbord::RotateXYZ(vec3 angles)
{
	mat4 result(1.0f);
	
	float sa = sin(angles.x);
	float ca = cos(angles.x);
	float sb = sin(angles.y);
	float cb = cos(angles.y);
	float sc = sin(angles.z);
	float cc = cos(angles.z);
	
	// A_z * A_y * A_x
	
	result[0][0] = cb*cc;
	result[1][0] = cb*sc;
	result[2][0] = -sb;
	
	result[0][1] = -ca*sc + sa*sb*cc;
	result[1][1] = ca*cc + sa*sb*sc;
	result[2][1] = sa*cb;
	
	result[0][2] = sa*sc + ca*sb*cc;
	result[1][2] = -sa*cc + ca*sb*sc;
	result[2][2] = ca*cb;
	
	return result;
}

mat4 Smorgasbord::Translate(vec3 v)
{
	mat4 result(1.0f);
	
	result[3][0] = v.x;
	result[3][1] = v.y;
	result[3][2] = v.z;
	
	return result;
}

mat4 Smorgasbord::Scale(vec3 v)
{
	mat4 result(1.0f);
	
	result[0][0] = v.x;
	result[1][1] = v.y;
	result[2][2] = v.z;
	
	return result;
}


mat4 Smorgasbord::Center(vec3 boundingMin, vec3 boundingMax)
{
	vec3 boundingCenter = (boundingMin + boundingMax) / 2.0f;
	vec3 boundingDiff = boundingMax - boundingMin;
	float boundingScale = glm::max(
		glm::max(boundingDiff.x, boundingDiff.y), boundingDiff.z);
	
	return Scale(vec3(1.0f/boundingScale)) * Translate(-boundingCenter);
}


mat4 Smorgasbord::NormalTransform(mat4 &world)
{
	// normal = world.transpose().invert()
	mat4 normal = world;
	float det = determinant(normal);
	normal[3] = vec4(0, 0, 0, det);
	normal = transpose(normal) / det;
	return normal;
}

mat4 Smorgasbord::GetPerspectiveProjection(
	float sensorWidth,
	float sensorHeight,
	float focalLength,
	float clipNear,
	float clipFar)
{
	mat4 projection = mat4(0.0f);
	
	float w = sensorWidth;
	float h = sensorHeight;
	float l = focalLength;
	float n = clipNear;
	float f = clipFar;
	
	projection[0][0] = (2.0f * l) / w;
	projection[1][1] = (2.0f * l) / h;
	projection[2][2] = -1.0f * (f + n) / (f - n);
	projection[2][3] = -1.0f;
	projection[3][2] = -1.0f * (2.0f * f * n) / (f - n);
	
	return projection;
}

mat4 Smorgasbord::GetPerspectiveProjection(
	float aspectRatio,
	float horizontalFov,
	float clipNear,
	float clipFar)
{
	float focalLength =
		aspectRatio / (2.0f * glm::tan(horizontalFov / 2.0f));
	return GetPerspectiveProjection(
		aspectRatio, 1.0f, focalLength, clipNear, clipFar);
}

mat4 Smorgasbord::GetOrthographicProjection(
	float sensorWidth,
	float sensorHeight,
	float orthoSensorScale,
	float clipNear,
	float clipFar)
{
	mat4 projection = mat4(1.0f);
	float n = clipNear;
	float f = clipFar;
	projection[0][0] = (2.0f / orthoSensorScale) / sensorWidth;
	projection[1][1] = (2.0f / orthoSensorScale) / sensorHeight;
	projection[2][2] = 2.0f / (n - f);
	projection[3][2] = (n + f) / (n - f);
	
	return projection;
}

mat4 Smorgasbord::GetOrthographicProjection(
	float aspectRatio,
	float orthoSensorScale,
	float clipNear,
	float clipFar)
{
	mat4 projection = mat4(1.0f);
	
	float w = aspectRatio > 1.0f ? aspectRatio : 1.0f;
	float h = aspectRatio > 1.0f ? 1.0f : 1.0f / aspectRatio;
	float n = clipNear;
	float f = clipFar;
	projection[0][0] = (1.0f / w) / orthoSensorScale;
	projection[1][1] = (1.0f / h) / orthoSensorScale;
	projection[2][2] = 2.0f / (n - f);
	projection[3][2] = (n + f) / (n - f);
	
	return projection;
}
