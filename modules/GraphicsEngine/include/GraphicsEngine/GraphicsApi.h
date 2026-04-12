#pragma once

#ifdef GRAPHICSENGINE_EXPORTS
	#define GRAPHICS_API __declspec(dllexport)
#else
	#define GRAPHICS_API __declspec(dllimport)
#endif

#include <vector>
#include "EngineInterfaces/IGraphics.h"
#include "GraphicsEngine/GraphicsTypes.h"
#include "GraphicsEngine/GraphicsData.h"
#include "Core/Utils.h"

using Vec3 = CoreMath::Vec3;

class MeshRenderer;
class Camera;

class GRAPHICS_API Graphics : public IGraphics
{
public:
	Graphics(CameraParams cameraParams, GLADloadproc loadProc);
	~Graphics();

	uint32_t CreateShaderProgram(const char* vertexShaderPath,
		const char* fragmentShaderPath) override;
	void DrawDebugLines(const Vec3* vertices, int vertexCount,
		uint32_t shaderProgram, Vec3 color = Vec3(1.0f, 1.0f, 1.0f)) override;

	void Render();
	Camera* GetCamera();
	void AddMeshRenderer(MeshRenderer* meshRenderer);

private:
	Camera* camera = nullptr;
	std::vector<MeshRenderer*> meshRenderers;

	uint32_t debugVAO = 0;
	uint32_t debugVBO = 0;
};

extern "C"
{
	using namespace Utils;
	GRAPHICS_API Graphics* GetGraphicsEngine(CameraParams cameraParams, GLADloadproc loadProc);
	GRAPHICS_API void DestroyGraphicsEngine(Graphics* graphicsEngineToDestroy);

	GRAPHICS_API MeshRenderer* CreateMeshRenderer(Graphics* graphics, MeshType meshType, ShaderType shaderType,
										Vec3 position = Vec3(0.0f, 0.0f, 0.0f),
										Vec3 scale = Vec3(1.0f, 1.0f, 1.0f),
										Vec3 color = Vec3(1.0f, 1.0f, 1.0f),
										const char* vertexShaderPath = "", 
										const char* fragmentShaderPath = "");
	GRAPHICS_API void UpdateMeshRendererPosition(MeshRenderer* meshRenderer, Vec3 newPosition);

	GRAPHICS_API void RotateCamera(Graphics* graphics, float xOffset, float yOffset);
	GRAPHICS_API void CameraOrbit(Graphics* graphics, Vec3 target, float distance, float xOffset, float yOffset, float frameTime, float smoothSpeed);
	GRAPHICS_API void MoveCamera(Graphics* graphics, Utils::Direction direction, float franeTime);
	GRAPHICS_API void CameraFollow(Graphics* graphics, Vec3 target, float distance, float frameTime, float smoothSpeed);

	GRAPHICS_API int LoadTextureToMeshRenderer(const char* textureFileName, MeshRenderer* meshRenderer);
}