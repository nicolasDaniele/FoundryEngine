#include <glad/glad.h>
#include <iostream>
#include "GraphicsEngine/GraphicsApi.h"
#include "GraphicsEngine/Mesh.h"
#include "GraphicsEngine/MeshBuffer.h"
#include "GraphicsEngine/MeshFactory.h"
#include "GraphicsEngine/Camera.h"
#include "GraphicsEngine/ShaderLoader.h"
#include "GraphicsEngine/TextureLoader.h"
#include "GraphicsEngine/GraphicsData.h"
#include "GraphicsEngine/LightData.h"
#include "EngineInterfaces/GraphicsPublicData.h"

Graphics::Graphics(CameraParams cameraParams, GLADloadproc loadProc)
{
	if (!gladLoadGLLoader(loadProc))
	{
		std::cout << "[GraphicsEngine] Failed to initialize GLAD." << std::endl;
		return;
	}

	if (!glad_glClear)
	{
		std::cout << "[GraphicsEngine] GLAD not loaded." << std::endl;
		return;
	}

	glEnable(GL_DEPTH_TEST);

	camera = new Camera(cameraParams);

	// FOR DEBUG
	glGenVertexArrays(1, &debugVAO);
	glGenBuffers(1, &debugVBO);

	glBindVertexArray(debugVAO);
	glBindBuffer(GL_ARRAY_BUFFER, debugVBO);

	// position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(
		0, 3, GL_FLOAT, GL_FALSE,
		sizeof(DebugVertex),
		(void *)offsetof(DebugVertex, position));

	// color
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(
		1, 3, GL_FLOAT, GL_FALSE,
		sizeof(DebugVertex),
		(void *)offsetof(DebugVertex, color));

	glBindVertexArray(0);

	// Per-frame uniform buffer (camera + lights), shared by every lit shader.
	// Filled once per frame in Render() instead of being re-uploaded per MeshRenderer.
	glGenBuffers(1, &frameUBO);
	glBindBuffer(GL_UNIFORM_BUFFER, frameUBO);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(FrameUBOData), nullptr, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, frameUBO); // fixed binding point for the engine's lifetime
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

Graphics::~Graphics()
{
	delete camera;

	if (frameUBO)
		glDeleteBuffers(1, &frameUBO);
	if (debugVBO)
		glDeleteBuffers(1, &debugVBO);
	if (debugVAO)
		glDeleteVertexArrays(1, &debugVAO);
}

uint32_t Graphics::CreateShaderProgram(const char* vertexShaderPath, const char* fragmentShaderPath)
{
	ShaderLoader shaderLoader;
	uint32_t program = shaderLoader.CreateProgram(vertexShaderPath, fragmentShaderPath);

	// Bind this program's "FrameData" uniform block (if it declares one) to the
	// same binding point frameUBO is bound to. Shaders that don't declare
	// FrameData (unlit shaders) get GL_INVALID_INDEX back and are simply skipped.
	GLuint blockIndex = glGetUniformBlockIndex(program, "FrameData");
	if (blockIndex != GL_INVALID_INDEX)
		glUniformBlockBinding(program, blockIndex, 0);

	return program;
}

void Graphics::DrawDebugLines(const Vec3* vertices, int vertexCount,
							  uint32_t shaderProgram, Vec3 color)
{
	if (vertexCount <= 0)
		return;

	std::vector<DebugVertex> debugVerts;
	debugVerts.reserve(vertexCount);

	for (int i = 0; i < vertexCount; ++i)
		debugVerts.push_back({vertices[i], color});

	Mat4 vp = camera->GetProjectionMatrix() * camera->GetViewMatrix();

	glUseProgram(shaderProgram);

	GLint vpLoc = glGetUniformLocation(shaderProgram, "vp");
	glUniformMatrix4fv(vpLoc, 1, GL_FALSE, vp.asArray);

	glBindVertexArray(debugVAO);
	glBindBuffer(GL_ARRAY_BUFFER, debugVBO);

	glBufferData(
		GL_ARRAY_BUFFER,
		debugVerts.size() * sizeof(DebugVertex),
		debugVerts.data(),
		GL_DYNAMIC_DRAW);

	glDrawArrays(GL_LINES, 0, vertexCount);

	glBindVertexArray(0);
}

MeshRendererHandle Graphics::CreateMeshRenderer(MeshType meshType, ShaderType shaderType, Vec3 position, Vec3 scale, Vec3 color,
											  const char *vertexShaderPath, const char *fragmentShaderPath)
{
	Mesh mesh = MeshFactory::CreateMesh(meshType, color);
	std::unique_ptr<MeshBuffer> meshBuffer = std::make_unique<MeshBuffer>();
	meshBuffer->LoadMeshData(mesh, shaderType);

	uint32_t shaderProgram = CreateShaderProgram(vertexShaderPath, fragmentShaderPath);

	uint32_t index;

	// Reuse a freed slot in O(1) instead of scanning MRSlots for one.
	if (!freeMRIndices.empty())
	{
		index = freeMRIndices.back();
		freeMRIndices.pop_back();

		MRSlots[index].alive = true;
		MRSlots[index].generation++;
		MRSlots[index].renderer = std::make_unique<MeshRenderer>(std::move(meshBuffer), position, scale);
	}
	else
	{
		MRSlots.emplace_back(
			std::make_unique<MeshRenderer>(std::move(meshBuffer), position, scale),
			0,
			true
		);
		index = (uint32_t)(MRSlots.size() - 1);
	}

	MRSlots[index].renderer->SetShaderProgram(shaderProgram);
	MRSlots[index].renderer->InitUniforms();

	return { index, MRSlots[index].generation };
}

void Graphics::DestroyMeshRenderer(MeshRendererHandle meshHandle)
{
    if (!IsValidMeshRenderer(meshHandle))
	{
		std::cout << "[GraphicsEngine] Invalid MeshRendererHandle." << std::endl;
		return;
	}

    auto& slot = MRSlots[meshHandle.index];
    slot.renderer.reset();
    slot.alive = false;
    slot.generation++;

    freeMRIndices.push_back(meshHandle.index);
}

bool Graphics::IsValidMeshRenderer(MeshRendererHandle meshHandle)
{
    return meshHandle.index < MRSlots.size() &&
           MRSlots[meshHandle.index].alive &&
           MRSlots[meshHandle.index].generation == meshHandle.generation;
}

void Graphics::UpdateMeshRendererPosition(MeshRendererHandle meshHandle, Vec3 newPosition)
{
	if (!IsValidMeshRenderer(meshHandle))
	{
		std::cout << "[GraphicsEngine] Invalid MeshRendererHandle." << std::endl;
		return;
	}

    MRSlots[meshHandle.index].renderer->SetPosition(newPosition);
}

int Graphics::LoadTextureToMeshRenderer(const char *textureFilePath, MeshRendererHandle meshHandle)
{
	if (!IsValidMeshRenderer(meshHandle))
	{
		std::cout << "[GraphicsEngine] Invalid MeshRendererHandle." << std::endl;
		return -1;
	}
	
	TextureLoader textureLoader;
	int textureID = textureLoader.LoadTexture(textureFilePath);
	if (textureID == -1)
	{
		std::cout << "[GraphicsEngine] Failed to load texture: " << textureFilePath << std::endl;
		return -1;
	}

	MRSlots[meshHandle.index].renderer->SetTexture(textureID);

	return textureID;
}

void Graphics::SetTextureTilingToMeshRenderer(MeshRendererHandle meshHandle, Vec2 tiling)
{
	if (!IsValidMeshRenderer(meshHandle))
	{
		std::cout << "[GraphicsEngine] Invalid MeshRendererHandle." << std::endl;
		return;
	}

	MRSlots[meshHandle.index].renderer->SetTextureTiling(tiling);
}

void Graphics::SetMeshRendererMaterial(MeshRendererHandle meshHandle, Material material)
{
	if (!IsValidMeshRenderer(meshHandle))
	{
		std::cout << "[GraphicsEngine] Invalid MeshRendererHandle." << std::endl;
		return;
	}

	MRSlots[meshHandle.index].renderer->SetMaterial(material);
}

Vec3 Graphics::GetMeshRendererPosition(MeshRendererHandle meshHandle)
{
	if (!IsValidMeshRenderer(meshHandle))
	{
		std::cout << "[GraphicsEngine] Invalid MeshRendererHandle." << std::endl;
		return Vec3();
	}

	return MRSlots[meshHandle.index].renderer->GetPosition();
}

Vec3 Graphics::GetMeshRendererScale(MeshRendererHandle meshHandle)
{
	if (!IsValidMeshRenderer(meshHandle))
	{
		std::cout << "[GraphicsEngine] Invalid MeshRendererHandle." << std::endl;
		return Vec3();
	}

	return MRSlots[meshHandle.index].renderer->GetScale();
}

LightHandle Graphics::CreateLight(LightParams lightParams)
{
	if (freeLightIndices.empty() && lightSlots.size() >= MAX_LIGHTS)
	{
		std::cout << "[GraphicsEngine] Max light count (" << MAX_LIGHTS << ") reached." << std::endl;
		return { (uint32_t)-1, 0 };
	}

	uint32_t index;

	// Same O(1) freelist reuse pattern as CreateMeshRenderer.
	if (!freeLightIndices.empty())
	{
		index = freeLightIndices.back();
		freeLightIndices.pop_back();

		lightSlots[index].alive = true;
		lightSlots[index].generation++;
		lightSlots[index].params = lightParams;
	}
	else
	{
		lightSlots.push_back({ lightParams, 0, true });
		index = (uint32_t)(lightSlots.size() - 1);
	}

	return { index, lightSlots[index].generation };
}

void Graphics::DestroyLight(LightHandle lightHandle)
{
	if (!IsValidLight(lightHandle))
	{
		std::cout << "[GraphicsEngine] Invalid LightHandle." << std::endl;
		return;
	}

	lightSlots[lightHandle.index].alive = false;
	lightSlots[lightHandle.index].generation++;

	freeLightIndices.push_back(lightHandle.index);
}

bool Graphics::IsValidLight(LightHandle lightHandle)
{
	return lightHandle.index < lightSlots.size() &&
	       lightSlots[lightHandle.index].alive &&
	       lightSlots[lightHandle.index].generation == lightHandle.generation;
}

void Graphics::SetLightPosition(LightHandle lightHandle, Vec3 newPosition)
{
	if (!IsValidLight(lightHandle))
	{
		std::cout << "[GraphicsEngine] Invalid LightHandle." << std::endl;
		return;
	}

	lightSlots[lightHandle.index].params.position = newPosition;
}

void Graphics::SetLightDirection(LightHandle lightHandle, Vec3 newDirection)
{
	if (!IsValidLight(lightHandle))
	{
		std::cout << "[GraphicsEngine] Invalid LightHandle." << std::endl;
		return;
	}

	lightSlots[lightHandle.index].params.direction = newDirection;
}

void Graphics::SetLightColor(LightHandle lightHandle, Vec3 newColor)
{
	if (!IsValidLight(lightHandle))
	{
		std::cout << "[GraphicsEngine] Invalid LightHandle." << std::endl;
		return;
	}

	lightSlots[lightHandle.index].params.color = newColor;
}

void Graphics::SetLightIntensity(LightHandle lightHandle, float newIntensity)
{
	if (!IsValidLight(lightHandle))
	{
		std::cout << "[GraphicsEngine] Invalid LightHandle." << std::endl;
		return;
	}

	lightSlots[lightHandle.index].params.intensity = newIntensity;
}

void Graphics::RotateCamera(float xOffset, float yOffset)
{
	if (camera == nullptr)
	{
		std::cout << "[GraphicsEngine] Camera is null." << std::endl;
		return;
	}

	camera->Rotate(xOffset, yOffset);
}

void Graphics::CameraOrbit(Vec3 target, float distance, float xOffset, float yOffset, float frameTime, float smoothSpeed)
{
	if (camera == nullptr)
	{
		std::cout << "[GraphicsEngine] Camera is null." << std::endl;
		return;
	}

	camera->Orbit(target, distance, xOffset, yOffset, frameTime, smoothSpeed);
}

void Graphics::MoveCamera(Utils::Direction direction, float frameTime)
{
	if (camera == nullptr)
	{
		std::cout << "[GraphicsEngine] Camera is null." << std::endl;
		return;
	}

	camera->Move(direction, frameTime);
}

void Graphics::CameraFollow(Vec3 target, float distance, float frameTime, float smoothSpeed)
{
	if (camera == nullptr)
	{
		std::cout << "[GraphicsEngine] Camera is null." << std::endl;
		return;
	}

	camera->Follow(target, distance, frameTime, smoothSpeed);
}

void Graphics::Render()
{
	if (camera == nullptr)
	{
		std::cout << "[GraphicsEngine] Camera is null." << std::endl;
		return;
	}

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.1f, 0.0f, 0.12f, 1.0f);

	// Fill and upload the per-frame data (view-projection, camera position and
	// active lights) once, instead of once per MeshRenderer.
	FrameUBOData frameData{};
	frameData.vp = camera->GetProjectionMatrix() * camera->GetViewMatrix();
	frameData.viewPos = camera->GetCameraPosition();

	frameData.lightCount = 0;
	for (auto& slot : lightSlots)
	{
		if (slot.alive && frameData.lightCount < MAX_LIGHTS)
			frameData.lights[frameData.lightCount++] = ToGPULight(slot.params);
	}

	glBindBuffer(GL_UNIFORM_BUFFER, frameUBO);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(FrameUBOData), &frameData);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	for (uint32_t i = 0; i < MRSlots.size(); i++)
	{
		if (MRSlots[i].alive)
			MRSlots[i].renderer->Draw(frameData.vp);
	}
}

extern "C"
{
	GRAPHICS_API IGraphics* GetGraphicsEngine(CameraParams cameraParams, GLADloadproc loadProc)
	{
		return new Graphics(cameraParams, loadProc);
	}

	GRAPHICS_API void DestroyGraphicsEngine(IGraphics *graphicsEngine)
	{
		delete graphicsEngine;
	}
}
