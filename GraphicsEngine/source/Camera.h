#pragma once

#include "Utils.h"
#include "GraphicsData.h"
#include "Vectors.h"
#include "Matrices.h"

using Vec3 = CoreMath::Vec3;
using Mat4 = CoreMath::Mat4;

class Camera
{
public:
	Camera(CameraParams cameraParams);
	~Camera() {}
	void MoveCamera(Utils::Direction direction, float frameTime);
	void RotateCamera(float xOffset, float yOffset, GLboolean constrainPitch = true);
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
	float pitch;
	float movementSpeed;
	float sensitivity;
	float zoomAmount;

	void UpdateVectors();
};

