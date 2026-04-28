#pragma once

#ifdef GRAPHICSENGINE_EXPORTS
	#define GRAPHICS_API __declspec(dllexport)
#else
	#define GRAPHICS_API __declspec(dllimport)
#endif

#include "Core/Vectors.h"
#include "Core/Matrices.h"
#include "Core/Utils.h"
#include "GraphicsTypes.h"
#include "GraphicsPublicData.h"

using Vec2 = CoreMath::Vec2;
using Vec3 = CoreMath::Vec3;
using Mat4 = CoreMath::Mat4;

struct MeshRendererHandle
{
    uint32_t index;
    uint32_t generation;
};

class IGraphics
{
public:
    virtual ~IGraphics() = default;

    virtual uint32_t CreateShaderProgram(const char* vertexShaderPath,
        const char* fragmentShaderPath) = 0;

    virtual void DrawDebugLines(const Vec3* vertices, int vertexCount,
        uint32_t shaderProgram, Vec3 color = Vec3(1.0f, 1.0f, 1.0f)) = 0;

    virtual void RotateCamera(float xOffset, float yOffset) = 0;
	virtual void CameraOrbit(Vec3 target, float distance, float xOffset, float yOffset, float frameTime, float smoothSpeed) = 0;
	virtual void MoveCamera(Utils::Direction direction, float frameTime) = 0;
	virtual void CameraFollow(Vec3 target, float distance, float frameTime, float smoothSpeed) = 0;

    virtual MeshRendererHandle CreateMeshRenderer(MeshType meshType, ShaderType shaderType,
										Vec3 position = Vec3(0.0f, 0.0f, 0.0f),
										Vec3 scale = Vec3(1.0f, 1.0f, 1.0f),
										Vec3 color = Vec3(1.0f, 1.0f, 1.0f),
										const char* vertexShaderPath = "", 
										const char* fragmentShaderPath = "") = 0;
	virtual void UpdateMeshRendererPosition(MeshRendererHandle meshHandle, Vec3 newPosition) = 0;
	virtual int LoadTextureToMeshRenderer(const char* textureFileName, MeshRendererHandle meshHandle) = 0;
	virtual void SetTextureTilingToMeshRenderer(MeshRendererHandle meshHandle, Vec2 tiling) = 0;

    virtual void Render() = 0;
};

extern "C"
{
	GRAPHICS_API IGraphics* GetGraphicsEngine(CameraParams cameraParams, GLADloadproc loadProc);
	GRAPHICS_API void DestroyGraphicsEngine(IGraphics* graphicsEngine);
}