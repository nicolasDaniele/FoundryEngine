#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <iostream>
#include "Core/Geometry3D.h"
#include "Core/Vectors.h"
<<<<<<< HEAD
#include "Debugger/DebugRenderer.h"
#include "EngineInterfaces/IGraphics.h"
#include "EngineInterfaces/IPhysics.h"
#include "EngineInterfaces/GraphicsPublicData.h"
=======
#include "GraphicsEngine/GraphicsApi.h"
#include "Debugger/DebugRenderer.h"
#include "PhysicsEngine/PhysicsApi.h"
#include "EngineInterfaces/PhysicsInterfaces.h"
#include "PhysicsEngine/Rigidbody.h"
#include "GameplayObjects/PlayerObject.h"
>>>>>>> texture_fixes_collision_events_&_player_object

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
float playerJumpImpulse = 1500.0f;
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
		std::cout << "Graphics engine is NULL\n";
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
		Vec3(0.2f, 0.8f, 0.2f), // Color
		TEXTURED_VS_PATH, TEXTURED_FS_PATH);

<<<<<<< HEAD
	int textureId = graphics->LoadTextureToMeshRenderer(WOOD_TEXTURE_PATH, floorRenderer);
	if (textureId == -1)
=======
	int woodTtextureId = LoadTextureToMeshRenderer(WOOD_TEXTURE_PATH, floorRenderer);
	if (woodTtextureId == -1)
>>>>>>> texture_fixes_collision_events_&_player_object
	{
		std::cout << "Texture could not be loaded for floorRenderer\n";
		std::cin.get();
		return -1;
	}

	SetTextureTilingToMeshRenderer(floorRenderer, Vec2(1.0f, 50.0f));


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
		std::cout << "Physics system is NULL\n";
		std::cin.get();
		return -1;
	}

	float collisionRestitution = 0.7f;

<<<<<<< HEAD
	ballVolume = physics->CreateRigidbody(2, ballPosition, 1.0f, 1.0f, collisionRestitution);
	physics->SetRigidbodySphereRadius(ballVolume, ballDebug.radius);

	floorVolume = physics->CreateRigidbody(3, floorPosition, 0.0f, 1.0f, collisionRestitution);
	physics->SetRigidbodyBoxHalfExtents(floorVolume, floorDebug.halfExtents);
	physics->SetRigidbodyBoxCenter(floorVolume, floorDebug.center);
	physics->SetRigidbodyBoxOrientation(floorVolume, floorDebug.orientation);
=======
	ballVolume = GetRigidbody(2, ballPosition, 1.0f, 1.0f, 0.0f);
	SetRigidbodySphereRadius(ballVolume, ballDebug.radius);

	floorVolume = GetRigidbody(3, floorPosition, 0.0f, 1.0f, 0.0f);
	SetRigidbodyBoxHalfExtents(floorVolume, floorDebug.halfExtents);
	SetRigidbodyBoxCenter(floorVolume, floorDebug.center);
	SetRigidbodyBoxOrientation(floorVolume, floorDebug.orientation);

	AddRigidbodyToPhysicsSystem((Rigidbody*)ballVolume, physics);
	AddRigidbodyToPhysicsSystem((Rigidbody*)floorVolume, physics);
>>>>>>> texture_fixes_collision_events_&_player_object

	// --------------------- END PHYSICS SETUP --------------------- \\


	player = new PlayerObject((RigidbodyVolume*)ballVolume, ballRenderer, playerSpeed, playerJumpImpulse);

	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = static_cast<float>(glfwGetTime());
		frameTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// Physics update
<<<<<<< HEAD
		physics->Update(frameTime);

		// Graphics update
		ballPosition = physics->GetRigidbodyPosition(ballVolume);
		graphics->UpdateMeshRendererPosition(ballRenderer, ballPosition);
=======
		UpdatePhysicsSystem(physics, frameTime);
>>>>>>> texture_fixes_collision_events_&_player_object
		
		floorPosition = physics->GetRigidbodyPosition(floorVolume);
		graphics->UpdateMeshRendererPosition(floorRenderer, floorPosition);

<<<<<<< HEAD
		graphics->CameraFollow(ballPosition, 10.0f, frameTime, 6.0f);
=======
		if (player != nullptr)
			CameraFollow(graphics, player->GetPosition(), 10.0f, frameTime, 6.0f);
>>>>>>> texture_fixes_collision_events_&_player_object

		glfwSwapBuffers(window);
		glfwPollEvents();
		HandleInput(window, frameTime);

<<<<<<< HEAD
		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
			graphics->CameraOrbit(ballPosition, 10.0f, mouseXOffset, mouseYOffset, frameTime, 6.0f);

		mouseXOffset = 0.0f;
		mouseYOffset = 0.0f;
	
=======
		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS && player != nullptr)
			CameraOrbit(graphics, player->GetPosition(), 10.0f, mouseXOffset, mouseYOffset, frameTime, 6.0f);

		mouseXOffset = 0.0f;
		mouseYOffset = 0.0f;
		
>>>>>>> texture_fixes_collision_events_&_player_object
		// Debug update
		ballDebug.center = ballPosition;
		floorDebug.center = floorPosition;

		debugRenderer->Clear();
		debugRenderer->AddSphere(ballDebug);
		debugRenderer->AddBox(floorDebug);
		debugRenderer->DrawDebug(Vec3(1.0f, 0.0f, 0.0f));

		player->Update(frameTime);

		graphics->Render();
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

<<<<<<< HEAD
	// Ball Movement
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		physics->AddLinearImpulseToRigidbody(ballVolume, Vec3(-speed, 0.0f, 0.0f) * frameTime);

	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		physics->AddLinearImpulseToRigidbody(ballVolume, Vec3(speed, 0.0f, 0.0f) * frameTime);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		physics->AddLinearImpulseToRigidbody(ballVolume, Vec3(0.0f, 0.0f, -speed) * frameTime);

	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		physics->AddLinearImpulseToRigidbody(ballVolume, Vec3(0.0f, 0.0f, speed) * frameTime);


	if (glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS)
		physics->AddLinearImpulseToRigidbody(ballVolume, Vec3(0.0f, verticalVel, 0.0f) * frameTime);
=======
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS && player != nullptr)
		player->Move(Vec3(playerSpeed, 0.0f, 0.0f) * frameTime);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS && player != nullptr)
		player->Move(Vec3(0.0f, 0.0f, -playerSpeed) * frameTime);

	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS && player != nullptr)
		player->Move(Vec3(0.0f, 0.0f, playerSpeed) * frameTime);

	// Player Jump
	if (glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS && player != nullptr)
		player->Jump(playerJumpImpulse * frameTime);
>>>>>>> texture_fixes_collision_events_&_player_object
}

void OrbitCamera_Callback(GLFWwindow* window, double xPosIn, double yPosIn)
{
	if(graphics == nullptr)
		return;

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