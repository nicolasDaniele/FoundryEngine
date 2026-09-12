#pragma once

#include "Core/Vectors.h"
#include "GraphicsTypes.h"

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

/// Public-facing description of a light, used to create/update lights via IGraphics.
/// Angles (cutOff/outerCutOff) are expressed in degrees for readability;
/// the engine converts them to cosines internally before uploading to the GPU
/// (see GraphicsEngine/LightData.h - ToGPULight).
struct LightParams
{
	LightType type = L_POINT;
	Vec3 position = Vec3(0.0f);
	Vec3 direction = Vec3(0.0f, -1.0f, 0.0f);
	Vec3 color = Vec3(1.0f);
	float intensity = 1.0f;

	// Point/spot attenuation coefficients.
	float constant = 1.0f;
	float linear = 0.09f;
	float quadratic = 0.032f;

	// Spot light cone angles, in degrees.
	float cutOff = 12.5f;
	float outerCutOff = 17.5f;
};

/// Per-object lighting properties. Only used by MeshRenderers created with a
/// *_LIT ShaderType (S_COLOR_LIT / S_TEXTURE_LIT); ignored otherwise.
struct Material
{
	float ambientStrength  = 0.1f;
	float diffuseStrength  = 1.0f;
	float specularStrength = 0.5f;
	float shininess        = 32.0f;
};
