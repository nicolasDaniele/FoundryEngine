#include <glad/glad.h>
#include "Camera.h"
#include "MathDefinitions.h"

Camera::Camera(CameraParams cameraParams)
{
	position = cameraParams.position;

	forward = Vec3(0.0f, 0.0f, -1.0f);
	up = Vec3(0.0f, 1.0f, 0.0f);
	worldUp = up;

	projectionMat = CoreMath::Projection(cameraParams.fieldOfView,
		cameraParams.width / cameraParams.height,
		cameraParams.nearPlane, cameraParams.farPlane);

	yaw = -90.0f;
	pitch = 0.0f;
	movementSpeed = 2.5f;
	sensitivity = 0.5f;
	zoomAmount = 45.0f;

	UpdateVectors();
}

void Camera::MoveCamera(Utils::Direction direction, float frameTime)
{
	float velocity = movementSpeed * frameTime;

	switch (direction)
	{
	case Utils::FORWARD:
		position = position + forward * velocity; break;
	case Utils::BACKWARD:
		position = position - forward * velocity; break;
	case Utils::UP:
		position = position + up * velocity; break;
	case Utils::DOWN:
		position = position - up * velocity; break;
	case Utils::LEFT:
		position = position - right * velocity; break;
	case Utils::RIGHT:
		position = position + right * velocity; break;
	default:
		break;
	}
}

void Camera::RotateCamera(float xOffset, float yOffset, GLboolean constrainPitch)
{
	xOffset *= sensitivity;
	yOffset *= sensitivity;

	yaw += xOffset;
	pitch += yOffset;

	if (constrainPitch)
		Utils::Clamp(pitch, -89.0f, 89.f);

	UpdateVectors();
}

void Camera::Zoom(const float yOffset)
{
	zoomAmount -= yOffset;
	Utils::Clamp(zoomAmount, 1.0f, 45.0f);
}

void Camera::UpdateVectors()
{
	Vec3 fwd;
	fwd.x = COS(DEG2RAD(yaw)) * COS(DEG2RAD(pitch));
	fwd.y = SIN(DEG2RAD(pitch));
	fwd.z = SIN(DEG2RAD(yaw)) * COS(DEG2RAD(pitch));
	forward = CoreMath::Normalized(fwd);

	right = CoreMath::Normalized(CoreMath::Cross(forward, worldUp));
	up = CoreMath::Normalized(CoreMath::Cross(right, forward));
}

Mat4 Camera::GetViewMatrix()
{
	return CoreMath::LookAt(position, position + forward, up);
}

Mat4 Camera::GetProjectionMatrix() const
{
	return projectionMat;
}

Vec3 Camera::GetCameraPosition() const
{
	return position;
}

float Camera::GetZoomAmount() const
{
	return zoomAmount;
}

void Camera::SetMovementSpeed(const float _speed)
{
	movementSpeed = _speed;
}

void Camera::SetSensitivity(const float _sensitivity)
{
	sensitivity = _sensitivity;
}

void Camera::SetZoomAmount(const float _zoomAmount)
{
	zoomAmount = _zoomAmount;
}