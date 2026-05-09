#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <iostream>
#include "Core/Geometry3D.h"
#include "Core/Vectors.h"
#include "EngineInterfaces/IGraphics.h"
#include "EngineInterfaces/IPhysics.h"
#include "EngineInterfaces/GraphicsPublicData.h"
#include "GameplayObjects/PlayerObject.h"

using Vec3 = CoreMath::Vec3;
using Vec2 = CoreMath::Vec2;

const int WIDTH = 960;
const int HEIGHT = 600;

const char* FLAT_VS_PATH = "Assets/Shaders/FlatColor.vs";
const char* FLAT_FS_PATH = "Assets/Shaders/FlatColor.fs";
const char* TEXTURED_VS_PATH = "Assets/Shaders/Textured.vs";
const char* TEXTURED_FS_PATH = "Assets/Shaders/Textured.fs";

const char* WOOD_TEXTURE_PATH = "Assets/Textures/wood.jpg";

float lastMouseXPos = WIDTH / 2.0f;
float lastMouseYPos = HEIGHT / 2.0f;
bool firstMouse = true;
float mouseXOffset = 0.0f;
float mouseYOffset = 0.0f;

float frameTime = 0.0f;
float lastFrame = 0.0f;

std::vector<MeshRendererHandle> floorRenderers;
std::vector<RigidbodyHandle> floorVolumes;

IGraphics* graphics = nullptr;
IPhysics* physics = nullptr;

float playerSpeed = 10.0f;
float playerJumpImpulse = 2500.0f;
PlayerObject* player = nullptr;

void HandleInput(GLFWwindow* window, float frameTime);
void OrbitCamera_Callback(GLFWwindow* window, double xposIn, double yposIn);
void SetupFloorLayout();

static void glfwError(int id, const char* description)
{
	std::cout << description << std::endl;
}

int main()
{
	glfwSetErrorCallback(&glfwError);
	glfwInit();

	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Foundry Engine", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "[App] Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwSetCursorPosCallback(window, OrbitCamera_Callback);
	glfwMakeContextCurrent(window);

	// -------------------- Engines Initialization -------------------- \\
	
	CameraParams cameraParams;
	cameraParams.fieldOfView = 45.0f;
	cameraParams.width = WIDTH;
	cameraParams.height = HEIGHT;
	cameraParams.nearPlane = 0.1f;
	cameraParams.farPlane = 100.0f;
	cameraParams.position = Vec3(0.0f, 1.0f, 40.0f);
	
	graphics = GetGraphicsEngine(cameraParams, (GLADloadproc)glfwGetProcAddress);
	if (!graphics)
	{
		std::cout << "[App] GraphicsEngine is null." << std::endl;
		std::cin.get();
		return -1;
	}
	
	physics = GetPhysicsEngine();
	if (!physics)
	{
		std::cout << "[App] PhysicsEngine is null." << std::endl;
		std::cin.get();
		return -1;
	}

	// ------------------ End Engines Initialization ------------------ \\


	// ------------------------ Player Setup ------------------------ \\

	Vec3 ballPosition = Vec3(0.0f, 15.0f, 30.0f);
	Vec3 ballSize = Vec3(0.5f, 0.5f, 0.5f);

	MeshRendererHandle ballRenderer = graphics->CreateMeshRenderer(MeshType::M_SPHERE, ShaderType::S_COLOR,
		ballPosition, ballSize,
		Vec3(0.4f, 0.4f, 0.4f), // Color
		FLAT_VS_PATH, FLAT_FS_PATH);
	
	RigidbodyHandle ballBody = physics->CreateRigidbody(BodyType::B_SPHERE, ballPosition);
	physics->SetRigidbodySphereRadius(ballBody, ballSize.y);
	
	player = new PlayerObject(ballBody, ballRenderer, physics, graphics);
	if (!player)
	{
		std::cout << "[App] PlayerObject is null." << std::endl;
		std::cin.get();
		return -1;
	}

	// ---------------------- End Player Setup ---------------------- \\

	// Level Setup	
	SetupFloorLayout();

	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = static_cast<float>(glfwGetTime());
		frameTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		glfwPollEvents();
		HandleInput(window, frameTime);

		physics->Update(frameTime);		
		player->Update(frameTime);
		
		// Camera Movement
		graphics->CameraFollow(player->GetPosition(), 10.0f, frameTime, 6.0f);

		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS && player != nullptr)
			graphics->CameraOrbit(player->GetPosition(), 10.0f, mouseXOffset, mouseYOffset, frameTime, 6.0f);
		
		mouseXOffset = 0.0f;
		mouseYOffset = 0.0f;
		
		
		graphics->Render();
		glfwSwapBuffers(window);
	}

	delete player;

	DestroyGraphicsEngine(graphics);
	DestroyPhysicsEngine(physics);
	glfwTerminate();

	return 0;
}

void HandleInput(GLFWwindow* window, float frameTime)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	// Player Movement
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS && player != nullptr)
		player->Move(Vec3(-playerSpeed, 0.0f, 0.0f) * frameTime);

	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS && player != nullptr)
		player->Move(Vec3(playerSpeed, 0.0f, 0.0f) * frameTime);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS && player != nullptr)
		player->Move(Vec3(0.0f, 0.0f, -playerSpeed) * frameTime);

	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS && player != nullptr)
		player->Move(Vec3(0.0f, 0.0f, playerSpeed) * frameTime);

	// Player Jump
	if (glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS && player != nullptr)
		player->Jump(playerJumpImpulse * frameTime);
}

void OrbitCamera_Callback(GLFWwindow* window, double xPosIn, double yPosIn)
{
	if(graphics == nullptr)
	{
		std::cout << "[App] GraphicsEngine is NULL" << std::endl;
		return;
	}

	float xPos = static_cast<float>(xPosIn);
	float yPos = static_cast<float>(yPosIn);

	if (firstMouse)
	{
		lastMouseXPos = xPos;
		lastMouseYPos = yPos;
		firstMouse = false;
	}

	mouseXOffset += xPos - lastMouseXPos;
	mouseYOffset += lastMouseYPos - yPos;

	lastMouseXPos = xPos;
	lastMouseYPos = yPos;
}

void SetupFloorLayout()
{
	// floorRenderer 0
	floorRenderers.push_back(graphics->CreateMeshRenderer(MeshType::M_CUBE, ShaderType::S_TEXTURE,
			Vec3(0.0f, 0.0f, 0.0f),		// Position
			Vec3(6.0f, 0.2f, 80.0f),	// Size
			Vec3(0.7f),					// Color
			TEXTURED_VS_PATH, TEXTURED_FS_PATH));

	// floorRenderer 1
	floorRenderers.push_back(graphics->CreateMeshRenderer(MeshType::M_CUBE, ShaderType::S_TEXTURE,
			Vec3(0.0f, -2.0f, -65.0f),	// Position
			Vec3(6.0f, 0.2f, 50.0f),	// Size
			Vec3(0.8f),					// Color
			TEXTURED_VS_PATH, TEXTURED_FS_PATH));
	
	// floorRenderer 2
	floorRenderers.push_back(graphics->CreateMeshRenderer(MeshType::M_CUBE, ShaderType::S_TEXTURE,
			Vec3(0.0f, 0.0f, -100.0f),	// Position
			Vec3(3.0f, 0.2f, 20.0f),	// Size
			Vec3(0.75f),				// Color
			TEXTURED_VS_PATH, TEXTURED_FS_PATH));

	// floorRenderer 3
	floorRenderers.push_back(graphics->CreateMeshRenderer(MeshType::M_CUBE, ShaderType::S_TEXTURE,
			Vec3(4.0f, 1.0f, -115.0f),	// Position
			Vec3(3.0f, 0.2f, 5.0f),		// Size
			Vec3(1.0f),					// Color
			TEXTURED_VS_PATH, TEXTURED_FS_PATH));

	// floorRenderer 4
	floorRenderers.push_back(graphics->CreateMeshRenderer(MeshType::M_CUBE, ShaderType::S_TEXTURE,
			Vec3(0.0f, 2.0f, -125.0f),	// Position
			Vec3(3.0f, 0.2f, 5.0f),		// Size
			Vec3(0.9f),					// Color
			TEXTURED_VS_PATH, TEXTURED_FS_PATH));

	// floorRenderer 5
	floorRenderers.push_back(graphics->CreateMeshRenderer(MeshType::M_CUBE, ShaderType::S_TEXTURE,
			Vec3(-4.0f, 3.0f, -135.0f),	// Position
			Vec3(3.0f, 0.2f, 5.0f),		// Size
			Vec3(0.85f),				// Color
			TEXTURED_VS_PATH, TEXTURED_FS_PATH));
	
	
	for(int i = 0; i < floorRenderers.size(); i++)
	{
		int textureId = graphics->LoadTextureToMeshRenderer(WOOD_TEXTURE_PATH, floorRenderers[i]);
		if (textureId == -1)
		{
			std::cout << "[App] Texture could not be loaded for floofloorRenderer in index." << i << std::endl;
			std::cin.get();
			return;
		}

		float yTiling = graphics->GetMeshRendererScale(floorRenderers[i]).z / 2.0f;
		graphics->SetTextureTilingToMeshRenderer(floorRenderers[i], Vec2(1.0f, yTiling));

		Vec3 floorPosition = graphics->GetMeshRendererPosition(floorRenderers[i]);
		CoreGeometry::OBB floorGeomery;
		floorGeomery.center = floorPosition;
		floorGeomery.halfExtents = graphics->GetMeshRendererScale(floorRenderers[i]) * 0.5f;

		floorVolumes.push_back(physics->CreateRigidbody(BodyType::B_BOX, floorPosition, 0.0f));
		physics->SetRigidbodyBoxHalfExtents(floorVolumes[i], floorGeomery.halfExtents);
		physics->SetRigidbodyBoxCenter(floorVolumes[i], floorGeomery.center);
	}
}