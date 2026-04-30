#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <iostream>
#include "Core/Geometry3D.h"
#include "Core/Vectors.h"
#include "Debugger/DebugRenderer.h"
#include "EngineInterfaces/IGraphics.h"
#include "EngineInterfaces/IPhysics.h"
#include "EngineInterfaces/GraphicsPublicData.h"
#include "GameplayObjects/PlayerObject.h"

using Vec2 = CoreMath::Vec2;
using Vec3 = CoreMath::Vec3;

const int WIDTH = 960;
const int HEIGHT = 600;

const char* FLAT_VS_PATH = "Assets/Shaders/FlatModel.vs";
const char* FLAT_FS_PATH = "Assets/Shaders/FlatModel.fs";
const char* TEXTURED_VS_PATH = "Assets/Shaders/TexturedModel.vs";
const char* TEXTURED_FS_PATH = "Assets/Shaders/TexturedModel.fs";

const char* WOOD_TEXTURE_PATH = "Assets/Textures/wood.jpg";

float lastMouseXPos = WIDTH / 2.0f;
float lastMouseYPos = HEIGHT / 2.0f;
bool firstMouse = true;
float mouseXOffset = 0.0f;
float mouseYOffset = 0.0f;

float frameTime = 0.0f;
float lastFrame = 0.0f;

RigidbodyHandle ballVolume;
RigidbodyHandle floorVolume;
CoreGeometry::Sphere ballDebug;
CoreGeometry::OBB floorDebug;

Vec3 ballPosition;

IGraphics* graphics = nullptr;
IPhysics* physics = nullptr;

float playerSpeed = 10.0f;
float playerJumpImpulse = 2500.0f;
PlayerObject* player = nullptr;

void HandleInput(GLFWwindow* window, float frameTime);
void OrbitCamera_Callback(GLFWwindow* window, double xposIn, double yposIn);

static void glfwError(int id, const char* description)
{
	std::cout << description << std::endl;
}

int main()
{
	glfwSetErrorCallback(&glfwError);
	glfwInit();

	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Main Engine", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "[MainEngine] Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwSetCursorPosCallback(window, OrbitCamera_Callback);
	glfwMakeContextCurrent(window);

	// ------------ Objects Variables ------------- \\

	ballPosition = Vec3(0.0f, 1.0f, 1.0f);
	Vec3 ballSize = Vec3(0.5f, 0.5f, 0.5f);

	Vec3 floorPosition = Vec3(0.0f, -2.0f, 0.0f);
	Vec3 floorSize = Vec3(6.0f, 0.2f, 300.0f);

	// -------------------- GRAHICS SETUP -------------------- \\

	CameraParams cameraParams;
	cameraParams.fieldOfView = 45.0f;
	cameraParams.width = WIDTH;
	cameraParams.height = HEIGHT;
	cameraParams.nearPlane = 0.1f;
	cameraParams.farPlane = 100.0f;
	cameraParams.position = Vec3(0.0f, 1.5f, 16.0f);

	graphics = GetGraphicsEngine(cameraParams, (GLADloadproc)glfwGetProcAddress);
	if (!graphics)
	{
		std::cout << "[MainEngine] GraphicsEngine is NULL" << std::endl;
		std::cin.get();
		return -1;
	}

	MeshRendererHandle ballRenderer = graphics->CreateMeshRenderer(MeshType::M_SPHERE, ShaderType::S_COLOR,
		ballPosition,
		ballSize,
		Vec3(0.4f, 0.4f, 0.4f), // Color
		FLAT_VS_PATH, FLAT_FS_PATH);

	MeshRendererHandle floorRenderer = graphics->CreateMeshRenderer(MeshType::M_CUBE, ShaderType::S_TEXTURE,
		floorPosition,
		floorSize,
		Vec3(0.9f, 0.2f, 0.0f), // Color
		TEXTURED_VS_PATH, TEXTURED_FS_PATH);

	int textureId = graphics->LoadTextureToMeshRenderer(WOOD_TEXTURE_PATH, floorRenderer);
	if (textureId == -1)
	{
		std::cout << "[MainEngine] Texture could not be loaded for floorRenderer." << std::endl;
		std::cin.get();
		return -1;
	}

	graphics->SetTextureTilingToMeshRenderer(floorRenderer, Vec2(1.0f, 50.0f));


	// ------------------- END GRAHICS SETUP ------------------- \\


	// ----------------------- DEBUG SETUP ----------------------- \\

	Debugger::DebugRenderer* debugRenderer = new Debugger::DebugRenderer(graphics);

	ballDebug.center = ballPosition;
	ballDebug.radius = ballSize.y;
	
	floorDebug.center = floorPosition;
	floorDebug.halfExtents = floorSize * 0.5f;
	floorDebug.orientation = CoreMath::Mat3();

	// ---------------------- END DEBUG SETUP ---------------------- \\


	// ---------------------- PHYSICS SETUP ---------------------- \\
	
	physics = GetPhysicsEngine();
	if (!physics)
	{
		std::cout << "[MainEngine] PhysicsEngine is NULL" << std::endl;
		std::cin.get();
		return -1;
	}

	float collisionRestitution = 0.7f;

	ballVolume = physics->CreateRigidbody(2, ballPosition, 1.0f, 1.0f, collisionRestitution);
	physics->SetRigidbodySphereRadius(ballVolume, ballDebug.radius);

	floorVolume = physics->CreateRigidbody(3, floorPosition, 0.0f, 1.0f, collisionRestitution);
	physics->SetRigidbodyBoxHalfExtents(floorVolume, floorDebug.halfExtents);
	physics->SetRigidbodyBoxCenter(floorVolume, floorDebug.center);
	physics->SetRigidbodyBoxOrientation(floorVolume, floorDebug.orientation);

	// --------------------- END PHYSICS SETUP --------------------- \\


	player = new PlayerObject(ballVolume, ballRenderer, physics, graphics);

	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = static_cast<float>(glfwGetTime());
		frameTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// Physics update
		physics->Update(frameTime);

		// Graphics update
		ballPosition = physics->GetRigidbodyPosition(ballVolume);
		graphics->UpdateMeshRendererPosition(ballRenderer, ballPosition);
		
		floorPosition = physics->GetRigidbodyPosition(floorVolume);
		graphics->UpdateMeshRendererPosition(floorRenderer, floorPosition);
		
		glfwPollEvents();
		HandleInput(window, frameTime);
		
		if (player != nullptr)
			graphics->CameraFollow(player->GetPosition(), 10.0f, frameTime, 6.0f);

		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS && player != nullptr)
			graphics->CameraOrbit(player->GetPosition(), 10.0f, mouseXOffset, mouseYOffset, frameTime, 6.0f);
		
		mouseXOffset = 0.0f;
		mouseYOffset = 0.0f;
			
		// Debug update
		ballDebug.center = ballPosition;
		floorDebug.center = floorPosition;
		
		
		player->Update(frameTime);
		
		graphics->Render();
		debugRenderer->Clear();
		debugRenderer->AddSphere(ballDebug);
		debugRenderer->AddBox(floorDebug);
		debugRenderer->DrawDebug(Vec3(1.0f, 1.0f, 0.0f));
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
		std::cout << "[MainEngine] GraphicsEngine is NULL" << std::endl;
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