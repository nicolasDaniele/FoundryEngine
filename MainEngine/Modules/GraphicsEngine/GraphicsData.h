#pragma once

#include "Vectors.h"

using Vec3 = CoreMath::Vec3;

struct CameraParams
{
	float fieldOfView;
	float width;
	float height;
	float nearPlane;
	float farPlane;
	Vec3 position;
};