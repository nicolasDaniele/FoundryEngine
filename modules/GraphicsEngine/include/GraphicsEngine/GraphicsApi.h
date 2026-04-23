#pragma once

#include <vector>
#include "Core/Utils.h"
#include "EngineInterfaces/IGraphics.h"
#include "GraphicsEngine/MeshRenderer.h"

using Vec3 = CoreMath::Vec3;

class Camera;

struct MeshRendererSlot
{
    MeshRenderer renderer;

	// CHECK THIS: generation in MeshRendererSlot AND in MeshRendererHandle?
    uint32_t generation;
    bool alive;

	MeshRendererSlot(MeshRenderer&& r, uint32_t gen, bool a)
        : renderer(std::move(r)), generation(gen), alive(a) {}

	MeshRendererSlot(std::unique_ptr<MeshBuffer> buffer, Vec3 pos, Vec3 scale,
                 uint32_t gen, bool alive)
    : renderer(std::move(buffer), pos, scale),
      generation(gen),
      alive(alive) {}

	MeshRendererSlot(const MeshRendererSlot&) = delete;
    MeshRendererSlot& operator=(const MeshRendererSlot&) = delete;

    MeshRendererSlot(MeshRendererSlot&&) = default;
    MeshRendererSlot& operator=(MeshRendererSlot&&) = default;
};

class GRAPHICS_API Graphics : public IGraphics
{
public:
	Graphics(CameraParams cameraParams, GLADloadproc loadProc);
	~Graphics();


	Graphics(const Graphics&) = delete;
    Graphics& operator=(const Graphics&) = delete;

    Graphics(Graphics&&) = default;
    Graphics& operator=(Graphics&&) = default;


	uint32_t CreateShaderProgram(const char* vertexShaderPath,
		const char* fragmentShaderPath) override;
	void DrawDebugLines(const Vec3* vertices, int vertexCount,
		uint32_t shaderProgram, Vec3 color = Vec3(1.0f, 1.0f, 1.0f)) override;

	MeshRendererHandle CreateMeshRenderer(MeshType meshType, ShaderType shaderType,
										Vec3 position = Vec3(0.0f, 0.0f, 0.0f),
										Vec3 scale = Vec3(1.0f, 1.0f, 1.0f),
										Vec3 color = Vec3(1.0f, 1.0f, 1.0f),
										const char* vertexShaderPath = "", 
										const char* fragmentShaderPath = "") override;
	void UpdateMeshRendererPosition(MeshRendererHandle meshHandle, Vec3 newPosition) override;
	int LoadTextureToMeshRenderer(const char* textureFileName, MeshRendererHandle meshHandle) override;

	void RotateCamera(float xOffset, float yOffset) override;
	void CameraOrbit(Vec3 target, float distance, float xOffset, float yOffset, float frameTime, float smoothSpeed) override;
	void MoveCamera(Utils::Direction direction, float frameTime) override;
	void CameraFollow(Vec3 target, float distance, float frameTime, float smoothSpeed) override;

	void Render() override;
	
private:
	bool IsValidMeshRenderer(MeshRendererHandle mrHandle);

	Camera* camera = nullptr;
	std::vector<MeshRendererSlot> MRSlots;

	uint32_t debugVAO = 0;
	uint32_t debugVBO = 0;
};