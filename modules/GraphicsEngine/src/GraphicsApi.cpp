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
#include "EngineInterfaces/GraphicsPublicData.h"

Graphics::Graphics(CameraParams cameraParams, GLADloadproc loadProc)
{
	if (!gladLoadGLLoader(loadProc))
	{
		std::cout << "[GraphicsEngine] Failed to initialize GLAD\n";
		return;
	}

	if (!glad_glClear)
	{
		std::cout << "[GraphicsEngine] GLAD not loaded!\n"
				  << std::endl;
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
}

Graphics::~Graphics()
{
	delete camera;

	if (debugVBO)
		glDeleteBuffers(1, &debugVBO);
	if (debugVAO)
		glDeleteVertexArrays(1, &debugVAO);
}

// @TODO: Print error message if MeshRenderer is not valid (in every function)
uint32_t Graphics::CreateShaderProgram(const char *vertexShaderPath, const char *fragmentShaderPath)
{
	ShaderLoader shaderLoader;
	return shaderLoader.CreateProgram(vertexShaderPath,
									  fragmentShaderPath);
}

void Graphics::DrawDebugLines(const Vec3 *vertices, int vertexCount,
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

// @TODO: Receive int instead of MeshType and ShaderType, and break if the received value is invalid
MeshRendererHandle Graphics::CreateMeshRenderer(MeshType meshType, ShaderType shaderType, Vec3 position, Vec3 scale, Vec3 color,
											  const char *vertexShaderPath, const char *fragmentShaderPath)
{
	Mesh mesh = MeshFactory::CreateMesh(meshType, color);
	std::unique_ptr<MeshBuffer> meshBuffer = std::make_unique<MeshBuffer>();
	meshBuffer->LoadMeshData(mesh, shaderType);

	uint32_t shaderProgram = CreateShaderProgram(vertexShaderPath, fragmentShaderPath);
	for (uint32_t i = 0; i < MRSlots.size(); i++)
	{
		if (!MRSlots[i].alive)
		{
			MRSlots[i].alive = true;
			MRSlots[i].generation++;

			MRSlots[i].renderer = MeshRenderer(std::move(meshBuffer), position, scale);
			MRSlots[i].renderer.SetShaderProgram(shaderProgram);

			return { i, MRSlots[i].generation };
		}
	}

	MRSlots.emplace_back(
		MeshRenderer(std::move(meshBuffer), position, scale),
		0,
		true
	);

	uint32_t index = (uint32_t)(MRSlots.size() - 1);
	MRSlots[index].renderer.SetShaderProgram(shaderProgram);

	return { index, 0 };
}

bool Graphics::IsValidMeshRenderer(MeshRendererHandle mHandle)
{
    return mHandle.index < MRSlots.size() &&
           MRSlots[mHandle.index].alive &&
           MRSlots[mHandle.index].generation == mHandle.generation;
}

void Graphics::UpdateMeshRendererPosition(MeshRendererHandle meshHandle, Vec3 newPosition)
{
	if (!IsValidMeshRenderer(meshHandle)) return;

    MRSlots[meshHandle.index].renderer.SetPosition(newPosition);
}

int Graphics::LoadTextureToMeshRenderer(const char *textureFileName, MeshRendererHandle meshHandle)
{
	if (!IsValidMeshRenderer(meshHandle)) return -1;
	
	TextureLoader textureLoader;
	int textureID = textureLoader.LoadTexture(textureFileName);
	if (textureID == -1)
	{
		std::cout << "[GraphicsApi] Failed to load texture: " << textureFileName << std::endl;
		return -1;
	}

	MRSlots[meshHandle.index].renderer.SetTexture(textureID);

	return textureID;
}

void Graphics::RotateCamera(float xOffset, float yOffset)
{
	if (camera == nullptr) return;
	camera->Rotate(xOffset, yOffset);
}

void Graphics::CameraOrbit(Vec3 target, float distance, float xOffset, float yOffset, float frameTime, float smoothSpeed)
{
	if (camera == nullptr) return;
	camera->Orbit(target, distance, xOffset, yOffset, frameTime, smoothSpeed);
}

void Graphics::MoveCamera(Utils::Direction direction, float frameTime)
{
	if (camera == nullptr) return;
	camera->Move(direction, frameTime);
}

void Graphics::CameraFollow(Vec3 target, float distance, float frameTime, float smoothSpeed)
{
	if (camera == nullptr) return;
	camera->Follow(target, distance, frameTime, smoothSpeed);
}

void Graphics::Render()
{
	if (camera == nullptr) return;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.1f, 0.0f, 0.12f, 1.0f);

	for (uint32_t i = 0; i < MRSlots.size(); i++)
	{
		if (MRSlots[i].alive)
			MRSlots[i].renderer.Draw(camera->GetProjectionMatrix() * camera->GetViewMatrix());
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

	// @TODO: Receive int instead of MeshType and ShaderType, and break if the received value is invalid
	GRAPHICS_API MeshRenderer* CreateMeshRenderer(Graphics* graphics, MeshType meshType,
		ShaderType shaderType, Vec3 position, Vec3 scale, Vec3 color,
		const char* vertexShaderPath, const char* fragmentShaderPath)
	{
		Mesh mesh = MeshFactory::CreateMesh(meshType, color);
		MeshBuffer* meshBuffer = new MeshBuffer();
		meshBuffer->LoadMeshData(mesh, shaderType);

		MeshRenderer* newMeshRenderer = new MeshRenderer(meshBuffer, position, scale);

		if (graphics == nullptr)
			return newMeshRenderer;

		uint32_t shaderProgram = graphics->CreateShaderProgram(vertexShaderPath, fragmentShaderPath);
		newMeshRenderer->SetShaderProgram(shaderProgram);
		newMeshRenderer->InitUniforms();
		graphics->AddMeshRenderer(newMeshRenderer);

		return newMeshRenderer;
	}

	GRAPHICS_API void UpdateMeshRendererPosition(MeshRenderer* meshRenderer, Vec3 newPosition)
	{
		meshRenderer->SetPosition(newPosition);
	}

	GRAPHICS_API void RotateCamera(Graphics* graphics, float xOffset, float yOffset)
	{
		graphics->GetCamera()->Rotate(xOffset, yOffset);
	}

	GRAPHICS_API void CameraOrbit(Graphics* graphics, Vec3 target, float distance, float xOffset, float yOffset, float frameTime, float smoothSpeed)
	{
		graphics->GetCamera()->Orbit(target, distance, xOffset, yOffset, frameTime, smoothSpeed);
	}

	GRAPHICS_API void MoveCamera(Graphics* graphics, Utils::Direction direction, float franeTime)
	{
		graphics->GetCamera()->Move(direction, franeTime);
	}

	GRAPHICS_API void CameraFollow(Graphics* graphics, Vec3 target, float distance, float frameTime, float smoothSpeed)
	{
		graphics->GetCamera()->Follow(target, distance, frameTime, smoothSpeed);
	}

	GRAPHICS_API int LoadTextureToMeshRenderer(const char* textureFileName, MeshRenderer* meshRenderer)
	{
		if(meshRenderer == nullptr)
		{
			std::cout << "[GraphicsApi] MeshRenderer is null. Cannot load texture." << std::endl;
			return -1;
		}

		TextureLoader textureLoader;
		 int textureID = textureLoader.LoadTexture(textureFileName);
		 if (textureID == -1)
		 {
			 std::cout << "[GraphicsApi] Failed to load texture: " << textureFileName << std::endl;
			 return -1;
		 }

		 meshRenderer->SetTexture(textureID);
		 
		 return textureID;
	}
	void SetTextureTilingToMeshRenderer(MeshRenderer* meshRenderer, Vec2 tiling)
	{
		if(meshRenderer == nullptr)
		 {
			 std::cout << "[GraphicsApi] MeshRenderer is null. Cannot set texture tiling." << std::endl;
			 return;
		}

		meshRenderer->SetTextureTiling(tiling);
	}
}