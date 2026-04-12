#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <iostream>
#include "GraphicsEngine/GraphicsApi.h"
#include "GraphicsEngine/Mesh.h"
#include "GraphicsEngine/MeshBuffer.h"
#include "GraphicsEngine/MeshRenderer.h"
#include "GraphicsEngine/MeshFactory.h"
#include "GraphicsEngine/Camera.h"
#include "GraphicsEngine/ShaderLoader.h"
#include "GraphicsEngine/TextureLoader.h"

Graphics::Graphics(CameraParams cameraParams, GLADloadproc loadProc)
{
	if (!gladLoadGLLoader(loadProc))
	{
		std::cout << "[GraphicsEngine] Failed to initialize GLAD\n";
		return;
	}

	if (!glad_glClear) 
	{
		std::cout << "[GraphicsEngine] GLAD not loaded!\n" << std::endl;
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
		(void*)offsetof(DebugVertex, position)
	);

	// color
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(
		1, 3, GL_FLOAT, GL_FALSE,
		sizeof(DebugVertex),
		(void*)offsetof(DebugVertex, color)
	);

	glBindVertexArray(0);
}

Graphics::~Graphics()
{
	delete camera;
	
	for (MeshRenderer* renderer : meshRenderers)
		delete renderer;

	if (debugVBO) glDeleteBuffers(1, &debugVBO);
	if (debugVAO) glDeleteVertexArrays(1, &debugVAO);
}

uint32_t Graphics::CreateShaderProgram(const char* vertexShaderPath, const char* fragmentShaderPath)
{
	ShaderLoader shaderLoader;
	return shaderLoader.CreateProgram(vertexShaderPath,
		fragmentShaderPath);
}

void Graphics::DrawDebugLines(const Vec3* vertices, int vertexCount,
	uint32_t shaderProgram, Vec3 color)
{
	if (vertexCount <= 0)
		return;

	std::vector<DebugVertex> debugVerts;
	debugVerts.reserve(vertexCount);

	for (int i = 0; i < vertexCount; ++i)
		debugVerts.push_back({ vertices[i], color });


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
		GL_DYNAMIC_DRAW
	);

	glDrawArrays(GL_LINES, 0, vertexCount);

	glBindVertexArray(0);
}

void Graphics::Render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.1f, 0.0f, 0.12f, 1.0f);

	if (camera == nullptr)
	{
		std::cout << "[Grahics::Render] Camera is null." << std::endl;
		return;
	}

	for (MeshRenderer* renderer : meshRenderers)
		renderer->Draw(camera->GetProjectionMatrix() * camera->GetViewMatrix());
}

Camera* Graphics::GetCamera()
{
	return camera;
}

void Graphics::AddMeshRenderer(MeshRenderer* meshRenderer)
{
	meshRenderers.push_back(meshRenderer);
}

extern "C"
{
	GRAPHICS_API Graphics* GetGraphicsEngine(CameraParams cameraParams, GLADloadproc loadProc)
	{
		return new Graphics(cameraParams, loadProc);
	}

	GRAPHICS_API void DestroyGraphicsEngine(Graphics* graphicsEngine)
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
}