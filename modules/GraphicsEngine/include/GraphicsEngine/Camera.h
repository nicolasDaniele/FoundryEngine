#pragma once

#include "Core/Utils.h"
#include "Core/Vectors.h"
#include "Core/Matrices.h"
#include "EngineInterfaces/GraphicsPublicData.h"

using Vec3 = CoreMath::Vec3;
using Mat4 = CoreMath::Mat4;

class Camera
{
public:
	Camera(CameraParams cameraParams);
	~Camera() {}

	void Move(Utils::Direction direction, float frameTime);
	void Follow(Vec3 target, float distance, float frameTime, float smoothSpeed);
	void Rotate(float xOffset, float yOffset, GLboolean constrainPitch = true);
	void Orbit(Vec3 target, float distance, float xOffset, float yOffset, float frameTime, float smoothSpeed);
	void Zoom(float yOffset);

	Mat4 GetViewMatrix();
	Mat4 GetProjectionMatrix() const;
	Vec3 GetCameraPosition() const;
	float GetZoomAmount() const;
	void SetMovementSpeed(const float _speed);
	void SetSensitivity(const float _sensitivity);
	void SetZoomAmount(const float _zoomAmount);

private:
	Mat4 projectionMat;
	Vec3 position;
	Vec3 forward;
	Vec3 up;
	Vec3 right;
	Vec3 worldUp;
	float yaw;
	float targetYaw;
	float pitch;
	float targetPitch;
	float movementSpeed;
	float sensitivity;
	float zoomAmount;

	void UpdateVectors();
	void UpdateVectors(Vec3 newForward);
};

