#pragma once

#include <cstddef>
#include "Core/Vectors.h"
#include "Core/Matrices.h"
#include "Core/MathDefinitions.h"
#include "EngineInterfaces/GraphicsTypes.h"
#include "EngineInterfaces/GraphicsPublicData.h"

using Vec3 = CoreMath::Vec3;
using Mat4 = CoreMath::Mat4;

/// GPU-side mirror of a light, matching the std140 layout of the "Light"
/// struct declared in the lit shaders (see Assets/Shaders/Lit.fs). Field
/// order matters: each vec3 is paired with a trailing float so the compiler
/// naturally packs it into 16 bytes without extra padding.
struct GPULight
{
	Vec3 position;    float type;      // type: 0 = directional, 1 = point, 2 = spot
	Vec3 direction;   float intensity;
	Vec3 color;       float constant;
	float linear, quadratic, cutOff, outerCutOff; // cutOff/outerCutOff stored as cos(radians)
};
static_assert(sizeof(GPULight) == 64, "GPULight layout must match std140 (16-byte aligned, 64 bytes total)");

/// GPU-side mirror of the per-frame uniform block ("FrameData" in shaders).
/// Filled once per frame in Graphics::Render() and uploaded via glBufferSubData.
///
/// IMPORTANT (std140 packing): do NOT add manual padding fields between
/// viewPos and lightCount, or between lightCount and lights[]. In std140, a
/// scalar (lightCount) immediately following a vec3 (viewPos) packs into
/// that vec3's own 16-byte slot - it does NOT get a separate one. Adding an
/// explicit pad field there shifts lightCount to the wrong offset and the
/// shader silently reads 0 for it (which is exactly what happened here: the
/// light loop never ran, so every mesh rendered fully black). The
/// static_asserts below catch this class of bug at compile time instead of
/// at "why is everything black" time.
struct FrameUBOData
{
	Mat4 vp;
	Vec3 viewPos;
	int  lightCount;
	GPULight lights[MAX_LIGHTS];
};

static_assert(offsetof(FrameUBOData, viewPos) == 64,
	"viewPos must start right after a 64-byte mat4, matching std140.");
static_assert(offsetof(FrameUBOData, lightCount) == 76,
	"lightCount must pack into viewPos's std140 slot (offset 76), not a new 16-byte block.");
static_assert(offsetof(FrameUBOData, lights) == 80,
	"lights[] must start at offset 80 (already 16-byte aligned) to match the shader's FrameData block.");

/// Converts a public LightParams into its GPU-ready representation.
inline GPULight ToGPULight(const LightParams& params)
{
	GPULight gpu{};
	gpu.position = params.position;
	gpu.type = static_cast<float>(params.type);
	gpu.direction = CoreMath::Normalized(params.direction);
	gpu.intensity = params.intensity;
	gpu.color = params.color;
	gpu.constant = params.constant;
	gpu.linear = params.linear;
	gpu.quadratic = params.quadratic;
	gpu.cutOff = COS(DEG2RAD(params.cutOff));
	gpu.outerCutOff = COS(DEG2RAD(params.outerCutOff));
	return gpu;
}
