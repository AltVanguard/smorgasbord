#pragma once

#include "scenegraph.hpp"
#include "transform.hpp"
#include "viewport.hpp"

#include <glm/glm.hpp>

#include <cmath>
#include <string>
#include <vector>

// TODO: implement SetView(vec3 location, vec3 up, vec3 direction)

namespace Smorgasbord {

class Camera : ScenePlacedObject
{
private:
	std::string name;
	bool isValid = false;
	bool isPerspective = true;
	
	glm::mat4 projection;
	glm::mat4 view;
	
public:
	// common physical parameters
	SMORGASBORD_PROPERTY_FIELD(float, SensorWidth) = 0.032f;
	SMORGASBORD_PROPERTY_FIELD(float, SensorHeight) = 0.018f;
	// physical parameters for perspective camera
	SMORGASBORD_PROPERTY_FIELD(float, FocalLength) = 0.035f;
	// ortho camera parameters
	SMORGASBORD_PROPERTY_FIELD(float, OrthoSensorScale) = 1.0f;
	// common virtual parameters
	SMORGASBORD_PROPERTY_FIELD(float, ClipNear) = 1.0f;
	SMORGASBORD_PROPERTY_FIELD(float, ClipFar) = 100.0f;
	// physical parameters for DoF
	SMORGASBORD_PROPERTY_FIELD(float, DofFocalDistance) = -1.0f;
	SMORGASBORD_PROPERTY_FIELD(float, DofAperture) = -1.0f;
	
	SMORGASBORD_PROPERTY_GETSET(bool, IsPerspective,
		GetIsPerspective, SetIsPerspective) = true;
	
public:
	Camera();
	
	glm::mat4 GetProjectionMatrix();
	glm::mat4 GetViewMatrix();
	
	void SetView(glm::vec3 location, glm::vec3 rotation);
	void SetView(glm::vec3 location, glm::vec3 up, glm::vec3 direction); // TODO
	void SetViewMatrix(const glm::mat4& mat);
	
	// SceneObject interface
	virtual std::string GetType() override
	{
		return "Camera";
	}
	
	virtual std::string GetName() override
	{
		return name;
	}
	
	void SetPerspective(
		float sensorWidth,
		float sensorHeight,
		float focalLength,
		float clipNear,
		float clipFar)
	{
		SensorWidth = sensorWidth;
		SensorHeight = sensorHeight;
		FocalLength = focalLength;
		ClipNear = clipNear;
		ClipFar = clipFar;
		
		IsPerspective = true;
	}
	
	// aspectRatio = sensorWidth/sensorHeight
	void SetPerspective(
		float aspectRatio,
		float horizontalFov,
		float clipNear,
		float clipFar)
	{
		horizontalFov = (horizontalFov / 180.0f) * glm::pi<float>();
		
		SensorWidth = aspectRatio;
		SensorHeight = 1.0f;
		FocalLength = aspectRatio / (2.0f * glm::tan(horizontalFov / 2.0f));
		ClipNear = clipNear;
		ClipFar = clipFar;
		
		IsPerspective = true;
	}
	
	void SetOrthographic(
		float sensorWidth,
		float sensorHeight,
		float orthoSensorScale,
		float clipNear,
		float clipFar)
	{
		SensorWidth = sensorWidth;
		SensorHeight = sensorHeight;
		OrthoSensorScale = orthoSensorScale;
		ClipNear = clipNear;
		ClipFar = clipFar;
		
		IsPerspective = false;
	}
	
	void SetOrthographic(
		float aspectRatio,
		float orthoSensorScale,
		float clipNear,
		float clipFar)
	{
		SensorWidth = aspectRatio > 1.0f ? aspectRatio : 1.0f;
		SensorHeight = aspectRatio > 1.0f ? 1.0f : 1.0f / aspectRatio;
		OrthoSensorScale = orthoSensorScale;
		ClipNear = clipNear;
		ClipFar = clipFar;
		
		IsPerspective = false;
	}
	
	bool GetIsPerspective() const
	{
		return isPerspective;
	}
	
	void SetIsPerspective(const bool &value)
	{
		isPerspective = value;
		isValid = false;
	}
};

}
