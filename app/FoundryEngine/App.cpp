#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <iostream>
#include <random>
#include <ctime>
#include "Core/Geometry3D.h"
#include "Core/Vectors.h"
#include "Debugger/DebugRenderer.h"
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

std::vector<MeshRendererHandle> boxRenderers;
std::vector<RigidbodyHandle> boxVolumes;

IGraphics* graphics = nullptr;
IPhysics* physics = nullptr;

float playerSpeed = 10.0f;
float playerJumpImpulse = 2500.0f;
PlayerObject* player = nullptr;

bool drawDebug = false;
bool tWasPressed = false;

void HandleInput(GLFWwindow* window, float frameTime);
void OrbitCamera_Callback(GLFWwindow* window, double xposIn, double yposIn);
void SetupFloorLayout();
float GetRandomColor();

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

	Debugger::DebugRenderer* debugRenderer = new Debugger::DebugRenderer(graphics);

	// ------------------ End Engines Initialization ------------------ \\


	// ------------------------ Player Setup ------------------------ \\

	Vec3 ballStartPosition = Vec3(0.0f, 20.0f, 30.0f);
	Vec3 ballSize = Vec3(0.5f, 0.5f, 0.5f);

	MeshRendererHandle ballRenderer = graphics->CreateMeshRenderer(MeshType::M_SPHERE, ShaderType::S_COLOR,
		ballStartPosition, ballSize,
		Vec3(0.4f, 0.4f, 0.4f), // Color
		FLAT_VS_PATH, FLAT_FS_PATH);
	
	RigidbodyHandle ballBody = physics->CreateRigidbody(BodyType::B_SPHERE, ballStartPosition);
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

		if(player->GetPosition().y < -10.0f)
			player->Reset(ballStartPosition);

		// Camera Movement
		graphics->CameraFollow(player->GetPosition(), 10.0f, frameTime, 6.0f);

		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS && player != nullptr)
			graphics->CameraOrbit(player->GetPosition(), 10.0f, mouseXOffset, mouseYOffset, frameTime, 6.0f);
		
		mouseXOffset = 0.0f;
		mouseYOffset = 0.0f;
		

		// Graphics Rendering
		graphics->Render();
		

		// Debug Rendering
		if(drawDebug)
		{
			debugRenderer->Clear();	
	
			debugRenderer->AddSphere({ player->GetPosition(), ballSize.x });
			for(int i = 0; i < boxRenderers.size(); i++)
			{
				CoreGeometry::OBB obb;
				debugRenderer->AddBox({ 
					graphics->GetMeshRendererPosition(boxRenderers[i]), 
					graphics->GetMeshRendererScale(boxRenderers[i]) * 0.5f
				});
			}
	
			debugRenderer->DrawDebug(Vec3(1.0f, 0.1f, 0.1f));
		}
		// End Debug Rendering
		
		glfwSwapBuffers(window);
	}

	delete player;
	delete debugRenderer;

	DestroyGraphicsEngine(graphics);
	DestroyPhysicsEngine(physics);
	glfwTerminate();

	return 0;
}

void HandleInput(GLFWwindow* window, float frameTime)
{
	// Close Window
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	// Toggle Draw Debug
	bool tPressed = glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS;
	if (tPressed && !tWasPressed)
		drawDebug = !drawDebug;

	tWasPressed = tPressed;

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
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && player != nullptr)
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
	// boxRenderer 0
	boxRenderers.push_back(graphics->CreateMeshRenderer(MeshType::M_CUBE, ShaderType::S_TEXTURE,
			Vec3(0.0f, 0.0f, 0.0f),		// Position
			Vec3(6.0f, 0.2f, 80.0f),	// Size
			Vec3(GetRandomColor()),		// Color
			TEXTURED_VS_PATH, TEXTURED_FS_PATH));

	// boxRenderer 1
	boxRenderers.push_back(graphics->CreateMeshRenderer(MeshType::M_CUBE, ShaderType::S_TEXTURE,
			Vec3(0.0f, -2.0f, -65.0f),	// Position
			Vec3(6.0f, 0.2f, 50.0f),	// Size
			Vec3(GetRandomColor()),		// Color
			TEXTURED_VS_PATH, TEXTURED_FS_PATH));
	
	// boxRenderer 2
	boxRenderers.push_back(graphics->CreateMeshRenderer(MeshType::M_CUBE, ShaderType::S_TEXTURE,
			Vec3(0.0f, 0.0f, -105.0f),	// Position
			Vec3(2.0f, 0.2f, 20.0f),	// Size
			Vec3(GetRandomColor()),		// Color
			TEXTURED_VS_PATH, TEXTURED_FS_PATH));

	// boxRenderer 3
	boxRenderers.push_back(graphics->CreateMeshRenderer(MeshType::M_CUBE, ShaderType::S_TEXTURE,
			Vec3(0.0f, 2.0f, -130.0f),	// Position
			Vec3(2.0f, 0.2f, 20.0f),	// Size
			Vec3(GetRandomColor()),		// Color
			TEXTURED_VS_PATH, TEXTURED_FS_PATH));

	// boxRenderer 4
	boxRenderers.push_back(graphics->CreateMeshRenderer(MeshType::M_CUBE, ShaderType::S_TEXTURE,
			Vec3(0.0f, 4.0f, -155.0f),	// Position
			Vec3(2.0f, 0.2f, 20.0f),	// Size
			Vec3(GetRandomColor()),		// Color
			TEXTURED_VS_PATH, TEXTURED_FS_PATH));

	// boxRenderer 5
	boxRenderers.push_back(graphics->CreateMeshRenderer(MeshType::M_CUBE, ShaderType::S_TEXTURE,
			Vec3(5.0f, 5.0f, -175.0f),	// Position
			Vec3(4.0f, 0.2f, 10.0f),	// Size
			Vec3(GetRandomColor()),		// Color
			TEXTURED_VS_PATH, TEXTURED_FS_PATH));

	// boxRenderer 6
	boxRenderers.push_back(graphics->CreateMeshRenderer(MeshType::M_CUBE, ShaderType::S_TEXTURE,
			Vec3(0.0f, 5.0f, -192.0f),	// Position
			Vec3(4.0f, 0.2f, 10.0f),	// Size
			Vec3(GetRandomColor()),		// Color
			TEXTURED_VS_PATH, TEXTURED_FS_PATH));

	// boxRenderer 7
	boxRenderers.push_back(graphics->CreateMeshRenderer(MeshType::M_CUBE, ShaderType::S_TEXTURE,
			Vec3(-5.0f, 5.0f, -214.0f),	// Position
			Vec3(4.0f, 0.2f, 10.0f),	// Size
			Vec3(GetRandomColor()),		// Color
			TEXTURED_VS_PATH, TEXTURED_FS_PATH));

	// boxRenderer 8
	boxRenderers.push_back(graphics->CreateMeshRenderer(MeshType::M_CUBE, ShaderType::S_TEXTURE,
			Vec3(0.0f, 5.0f, -255.0f),	// Position
			Vec3(6.0f, 0.2f, 50.0f),	// Size
			Vec3(GetRandomColor()),		// Color
			TEXTURED_VS_PATH, TEXTURED_FS_PATH));

	// GOAL
	// boxRenderer 9
	boxRenderers.push_back(graphics->CreateMeshRenderer(MeshType::M_CUBE, ShaderType::S_TEXTURE,
			Vec3(0.0f, 5.0f, -290.0f),	// Position
			Vec3(30.0f, 0.2f, 20.0f),	// Size
			Vec3(1.0f),					// Color
			TEXTURED_VS_PATH, TEXTURED_FS_PATH));
	
	// boxRenderer 10 (back wall)
	boxRenderers.push_back(graphics->CreateMeshRenderer(MeshType::M_CUBE, ShaderType::S_TEXTURE,
			Vec3(0.0f, 8.0f, -300.0f),	// Position
			Vec3(30.0f, 6.0f, 0.2f),	// Size
			Vec3(1.0f),					// Color
			TEXTURED_VS_PATH, TEXTURED_FS_PATH));

	// boxRenderer 11 (left wall)
	boxRenderers.push_back(graphics->CreateMeshRenderer(MeshType::M_CUBE, ShaderType::S_TEXTURE,
			Vec3(-15.0f, 8.0f, -290.0f),// Position
			Vec3(0.2f, 6.0f, 20.0f),	// Size
			Vec3(1.0f),					// Color
			TEXTURED_VS_PATH, TEXTURED_FS_PATH));

	// boxRenderer 12 (right wall)
	boxRenderers.push_back(graphics->CreateMeshRenderer(MeshType::M_CUBE, ShaderType::S_TEXTURE,
			Vec3(15.0f, 8.0f, -290.0f),	// Position
			Vec3(0.2f, 6.0f, 20.0f),	// Size
			Vec3(1.0f),					// Color
			TEXTURED_VS_PATH, TEXTURED_FS_PATH));
	
	for(int i = 0; i < boxRenderers.size(); i++)
	{
		int textureId = graphics->LoadTextureToMeshRenderer(WOOD_TEXTURE_PATH, boxRenderers[i]);
		if (textureId == -1)
		{
			std::cout << "[App] Texture could not be loaded for boxRenderer in index." << i << std::endl;
			std::cin.get();
			return;
		}

		float yTiling = graphics->GetMeshRendererScale(boxRenderers[i]).z / 2.0f;
		graphics->SetTextureTilingToMeshRenderer(boxRenderers[i], Vec2(1.0f, yTiling));

		Vec3 boxPosition = graphics->GetMeshRendererPosition(boxRenderers[i]);
		CoreGeometry::OBB boxGeomery;
		boxGeomery.center = boxPosition;
		boxGeomery.halfExtents = graphics->GetMeshRendererScale(boxRenderers[i]) * 0.5f;

		boxVolumes.push_back(physics->CreateRigidbody(BodyType::B_BOX, boxPosition, 0.0f));
		physics->SetRigidbodyBoxHalfExtents(boxVolumes[i], boxGeomery.halfExtents);
		physics->SetRigidbodyBoxCenter(boxVolumes[i], boxGeomery.center);
	}
}

float GetRandomColor()
{
    static std::mt19937 gen(static_cast<unsigned int>(std::time(0)));  
    
	std::uniform_real_distribution<float> dis(0.5f, 1.0f);
    
	return dis(gen);
}