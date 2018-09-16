#ifndef SMORGASBORD_CAMERA_HPP
#define SMORGASBORD_CAMERA_HPP

#include "scenegraph.hpp"
#include "transform.hpp"
#include "viewport.hpp"

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <SDL.h>

#include <cmath>
#include <string>
#include <vector>

// TODO: implement SetView(vec3 location, vec3 up, vec3 direction)

using namespace glm;
using namespace std;

namespace Smorgasbord {

class Camera : ScenePlacedObject
{
private:
	string name;
	bool isValid = false;
	bool isPerspective = true;
	
	mat4 projection;
	mat4 view;
	
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
	
	mat4 GetProjectionMatrix();
	mat4 GetViewMatrix();
	
	void SetView(vec3 location, vec3 rotation);
	void SetView(vec3 location, vec3 up, vec3 direction); // TODO
	void SetViewMatrix(const mat4& mat);
	
	// SceneObject interface
	virtual string GetType() override
	{
		return "Camera";
	}
	
	virtual string GetName() override
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

class CameraController
{
protected:
	Camera *camera;
	
public:
	CameraController(Camera *camera);
	virtual ~CameraController();
	
	virtual bool HandleEvent(SDL_Event windowEvent) = 0;
};

class FlyCameraController : public CameraController
{
private:
	mat4 oldView;
	
public:
	FlyCameraController(Camera *camera);
	
	// CameraController interface
	bool HandleEvent(SDL_Event windowEvent);
};

}

#endif // SMORGASBORD_CAMERA_HPP
