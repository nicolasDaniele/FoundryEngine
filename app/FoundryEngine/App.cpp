#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <iostream>
#include <random>
#include <ctime>
#include "Core/Geometry3D.h"
#include "Core/Vectors.h"
#include "Core/Quaternions.h"
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
const char* LIT_VS_PATH = "Assets/Shaders/Lit.vs";
const char* LIT_FS_PATH = "Assets/Shaders/Lit.fs";
const char* TEXTURED_LIT_VS_PATH = "Assets/Shaders/TexturedLit.vs";
const char* TEXTURED_LIT_FS_PATH = "Assets/Shaders/TexturedLit.fs";

const char* WOOD_TEXTURE_PATH = "Assets/Textures/wood.jpg";
const char* METAL_TEXTURE_PATH = "Assets/Textures/metal.png";

float lastMouseXPos = WIDTH / 2.0f;
float lastMouseYPos = HEIGHT / 2.0f;
bool firstMouse = true;
float mouseXOffset = 0.0f;
float mouseYOffset = 0.0f;

float frameTime = 0.0f;
float lastFrame = 0.0f;

std::vector<MeshRendererHandle> boxRenderers;
std::vector<RigidbodyHandle> boxVolumes;

// Point lights created in SetupSceneLighting are remembered here so their
// positions/colors can be visualized as debug gizmos (see the debug
// rendering block in main()). Directional lights have no meaningful
// position, so they're not tracked here.
std::vector<LightParams> pointLightGizmos;
const float LIGHT_GIZMO_RADIUS = 0.3f;

IGraphics* graphics = nullptr;
IPhysics* physics = nullptr;

float playerSpeed = 10.0f;
float playerJumpImpulse = 2500.0f;
PlayerObject* player = nullptr;

bool drawDebug = false;
bool tWasPressed = false;

// TO TEST
bool floorIsRotating = false;
float floorRotRate = 0.0f;
// Nuevas variables globales
Vec3 floor0AngVel = Vec3(0.0f);       // lo que realmente le estamos pasando a la física
Vec3 floor0TargetAngVel = Vec3(0.0f); // lo que el input está pidiendo
const float floorAngularAccel = 3.0f; // qué tan rápido floor0AngVel alcanza el target (rad/s por segundo) - ajustable a gusto


void HandleInput(GLFWwindow* window, float frameTime);
void OrbitCamera_Callback(GLFWwindow* window, double xposIn, double yposIn);
void SetupSceneLighting();
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
	cameraParams.position = Vec3(-40.0f, 10.0f, 40.0f);
	
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


	// ---------------------------- Lighting Setup ---------------------------- \\

	SetupSceneLighting();

	// -------------------------- End Lighting Setup -------------------------- \\


	// ------------------------ Player Setup ------------------------ \\

	Vec3 playerStartPosition = Vec3(0.0f, 20.0f, 30.0f);
	Vec3 playerSize = Vec3(0.5f);

	MeshRendererHandle playerRenderer = graphics->CreateMeshRenderer(MeshType::M_SPHERE, ShaderType::S_TEXTURE_LIT,
		playerStartPosition, playerSize,
		Vec3(1.f), // Color
		TEXTURED_LIT_VS_PATH, TEXTURED_LIT_FS_PATH);

	int playerTexId = graphics->LoadTextureToMeshRenderer(METAL_TEXTURE_PATH, playerRenderer);
	if (playerTexId == -1)
	{
		std::cout << "[App] Texture could not be loaded for playerTexId.\n";
		std::cin.get();
		return -1;
	}

	Material playerMaterial;
	playerMaterial.ambientStrength = 0.15f;
	playerMaterial.specularStrength = 0.8f;
	playerMaterial.shininess = 80.0f;
	graphics->SetMeshRendererMaterial(playerRenderer, playerMaterial);
	
	RigidbodyHandle playerBody = physics->CreateRigidbody(BodyType::B_SPHERE, playerStartPosition, 
		1.f, 1.f, 0.1f); // mass, friction, restitution
	physics->SetRigidbodySphereRadius(playerBody, playerSize.y);
	physics->SetRigidbodySphereRollingResistance(playerBody, 0.6f);
	physics->SetRigidbodyFriction(playerBody, 0.7f);
	physics->SetRigidbodyDamping(playerBody, 0.1f);
	physics->SetRigidbodyAngularDamping(playerBody, 0.2f);
	
	player = new PlayerObject(playerBody, playerRenderer, physics, graphics);
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


		if (boxVolumes.size() > 0)
		{
			Vec3 angVelDelta = floor0TargetAngVel - floor0AngVel;
			float maxStep = floorAngularAccel * frameTime;

			if (CoreMath::Magnitude(angVelDelta) <= maxStep)
				floor0AngVel = floor0TargetAngVel;
			else
				floor0AngVel = floor0AngVel + CoreMath::Normalized(angVelDelta) * maxStep;

			physics->SetRigidbodyAngularVelocity(boxVolumes[0], floor0AngVel);
			graphics->SetMeshRendererRotation(boxRenderers[0], physics->GetRigidbodyOrientation(boxVolumes[0]));
		}


		physics->Update(frameTime);		
		player->Update(frameTime);

		if(player->GetPosition().y < -10.0f)
			player->Reset(playerStartPosition);

		// Camera Movement
		graphics->CameraFollow(player->GetPosition(), 40.0f, frameTime, 6.0f);


		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS && player != nullptr)
			graphics->CameraOrbit(player->GetPosition(), 40.0f, mouseXOffset, mouseYOffset, frameTime, 6.0f);
		
		mouseXOffset = 0.0f;
		mouseYOffset = 0.0f;
		

		graphics->SetMeshRendererRotation(boxRenderers[0], physics->GetRigidbodyOrientation(boxVolumes[0]));
		graphics->SetMeshRendererRotation(boxRenderers[1], physics->GetRigidbodyOrientation(boxVolumes[1]));


		// Graphics Rendering
		graphics->Render();
		

		// Debug Rendering
		if(drawDebug)
		{
			debugRenderer->Clear();	
	
			debugRenderer->AddSphere({ player->GetPosition(), playerSize.x });

			for(int i = 0; i < boxRenderers.size(); i++)
			{
				debugRenderer->AddBox({ 
					graphics->GetMeshRendererPosition(boxRenderers[i]), 
					graphics->GetMeshRendererScale(boxRenderers[i]) * 0.5f,
					CoreMath::ToMat3(graphics->GetMeshRendererRotation(boxRenderers[i]))
				});
			}

			// Point light gizmos: a small wireframe sphere per light, tinted
			// with the light's own emission color.
			for (const LightParams& light : pointLightGizmos)
				debugRenderer->AddColoredSphere({ light.position, LIGHT_GIZMO_RADIUS }, light.color);
	
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



	// TO TEST FLOOR ROTATION
	if (boxVolumes.size() > 0)
	{
		if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS)
			floor0TargetAngVel = Vec3(-0.5f, 0, 0);
		else if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS)
			floor0TargetAngVel = Vec3(0.5f, 0, 0);
		else
			floor0TargetAngVel = Vec3(0.0f);
	}

	if (boxVolumes.size() > 1)
	{
		if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS)
			physics->SetRigidbodyAngularVelocity(boxVolumes[1], Vec3(0, -0.5f, 0));
		else if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
			physics->SetRigidbodyAngularVelocity(boxVolumes[1], Vec3(0, 0.5f, 0));
		else
			physics->SetRigidbodyAngularVelocity(boxVolumes[1], Vec3(0.0f));
	}
		
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

void SetupSceneLighting()
{
	LightParams dirLight;
	dirLight.type = L_DIRECTIONAL;
	dirLight.color = Vec3(1.f, 1.f, 1.f);
	dirLight.intensity = 1.;
	graphics->CreateLight(dirLight);
	return;

	// Creates the light in the engine and also remembers its LightParams in
	// pointLightGizmos, so the debug renderer can later draw a wireframe
	// sphere at its position, tinted with its own color.
	auto addLight = [](LightParams params)
	{
		graphics->CreateLight(params);
		pointLightGizmos.push_back(params);
	};

	// Spot Lights
	LightParams spotLight_S1;
	spotLight_S1.type = L_SPOT;
	spotLight_S1.position = Vec3(0.0f, 6.0f, 30.0f);
	spotLight_S1.color = Vec3(1.f, 1.f, 1.f);
	spotLight_S1.intensity = 1.;
	addLight(spotLight_S1);

	LightParams spotLight_S2;
	spotLight_S2.type = L_SPOT;
	spotLight_S2.position = Vec3(0.0f, 6.0f, 15.0f);
	spotLight_S2.color = Vec3(1.f, 1.f, 1.f);
	spotLight_S2.intensity = 1.;
	addLight(spotLight_S2);

	LightParams spotLight_S3;
	spotLight_S3.type = L_SPOT;
	spotLight_S3.position = Vec3(0.0f, 6.0f, 0.0f);
	spotLight_S3.color = Vec3(1.f, 1.f, 1.f);
	spotLight_S3.intensity = 1.;
	addLight(spotLight_S3);

	LightParams spotLight_S4;
	spotLight_S4.type = L_SPOT;
	spotLight_S4.position = Vec3(0.0f, 6.0f, -15.0f);
	spotLight_S4.color = Vec3(1.f, 1.f, 1.f);
	spotLight_S4.intensity = 1.;
	addLight(spotLight_S4);


	// Point Lights
	LightParams pointLight_L1;
	pointLight_L1.type = L_POINT;
	pointLight_L1.position = Vec3(-4.0f, 5.0f, 30.0f);
	pointLight_L1.color = Vec3(0.5f, 0.7f, 0.2f);
	pointLight_L1.intensity = 0.8f;
	addLight(pointLight_L1);

	LightParams pointLight_R1;
	pointLight_R1.type = L_POINT;
	pointLight_R1.position = Vec3(4.0f, 5.0f, 10.0f);
	pointLight_R1.color = Vec3(0.7f, 0.5f, 0.2f);
	pointLight_R1.intensity = 0.8f;
	addLight(pointLight_R1);

	LightParams pointLight_L2;
	pointLight_L2.type = L_POINT;
	pointLight_L2.position = Vec3(-4.0f, 5.0f, -10.0f);
	pointLight_L2.color = Vec3(0.5f, 0.7f, 0.2f);
	pointLight_L2.intensity = 0.8f;
	addLight(pointLight_L2);

	LightParams pointLight_R2;
	pointLight_R2.type = L_POINT;
	pointLight_R2.position = Vec3(4.0f, 5.0f, -30.0f);
	pointLight_R2.color = Vec3(0.7f, 0.5f, 0.2f);
	pointLight_R2.intensity = 0.8f;
	addLight(pointLight_R2);

	LightParams pointLight_L3;
	pointLight_L3.type = L_POINT;
	pointLight_L3.position = Vec3(-4.0f, 5.0f, -50.0f);
	pointLight_L3.color = Vec3(0.5f, 0.7f, 0.2f);
	pointLight_L3.intensity = 0.8f;
	addLight(pointLight_L3);

	LightParams pointLight_R3;
	pointLight_R3.type = L_POINT;
	pointLight_R3.position = Vec3(4.0f, 5.0f, -70.0f);
	pointLight_R3.color = Vec3(0.7f, 0.5f, 0.2f);
	pointLight_R3.intensity = 0.8f;
	addLight(pointLight_R3);

	LightParams pointLight_L4;
	pointLight_L4.type = L_POINT;
	pointLight_L4.position = Vec3(-4.0f, 5.0f, -90.0f);
	pointLight_L4.color = Vec3(0.5f, 0.7f, 0.2f);
	pointLight_L4.intensity = 0.8f;
	addLight(pointLight_L4);

	LightParams pointLight_R4;
	pointLight_R4.type = L_POINT;
	pointLight_R4.position = Vec3(4.0f, 5.0f, -110.0f);
	pointLight_R4.color = Vec3(0.7f, 0.5f, 0.2f);
	pointLight_R4.intensity = 0.8f;
	addLight(pointLight_R4);
}

void SetupFloorLayout()
{
	// boxRenderer 0
	boxRenderers.push_back(graphics->CreateMeshRenderer(MeshType::M_CUBE, ShaderType::S_TEXTURE_LIT,
			Vec3(0.0f, 0.0f, 0.0f),		// Position
			Vec3(6.0f, 0.2f, 80.0f),	// Size
			Vec3(GetRandomColor()),		// Color
			TEXTURED_LIT_VS_PATH, TEXTURED_LIT_FS_PATH));

	// boxRenderer 1
	boxRenderers.push_back(graphics->CreateMeshRenderer(MeshType::M_CUBE, ShaderType::S_TEXTURE_LIT,
			Vec3(0.0f, -2.0f, -65.0f),	// Position
			Vec3(25.0f, 0.2f, 25.0f),	// Size
			Vec3(GetRandomColor()),		// Color
			TEXTURED_LIT_VS_PATH, TEXTURED_LIT_FS_PATH));
	
	// boxRenderer 2
	boxRenderers.push_back(graphics->CreateMeshRenderer(MeshType::M_CUBE, ShaderType::S_TEXTURE_LIT,
			Vec3(0.0f, 0.0f, -105.0f),	// Position
			Vec3(2.0f, 0.2f, 20.0f),	// Size
			Vec3(GetRandomColor()),		// Color
			TEXTURED_LIT_VS_PATH, TEXTURED_LIT_FS_PATH));

	// boxRenderer 3
	boxRenderers.push_back(graphics->CreateMeshRenderer(MeshType::M_CUBE, ShaderType::S_TEXTURE_LIT,
			Vec3(0.0f, 2.0f, -130.0f),	// Position
			Vec3(2.0f, 0.2f, 20.0f),	// Size
			Vec3(GetRandomColor()),		// Color
			TEXTURED_LIT_VS_PATH, TEXTURED_LIT_FS_PATH));

	// boxRenderer 4
	boxRenderers.push_back(graphics->CreateMeshRenderer(MeshType::M_CUBE, ShaderType::S_TEXTURE_LIT,
			Vec3(0.0f, 4.0f, -155.0f),	// Position
			Vec3(2.0f, 0.2f, 20.0f),	// Size
			Vec3(GetRandomColor()),		// Color
			TEXTURED_LIT_VS_PATH, TEXTURED_LIT_FS_PATH));

	// boxRenderer 5
	boxRenderers.push_back(graphics->CreateMeshRenderer(MeshType::M_CUBE, ShaderType::S_TEXTURE_LIT,
			Vec3(5.0f, 5.0f, -175.0f),	// Position
			Vec3(4.0f, 0.2f, 10.0f),	// Size
			Vec3(GetRandomColor()),		// Color
			TEXTURED_LIT_VS_PATH, TEXTURED_LIT_FS_PATH));

	// boxRenderer 6
	boxRenderers.push_back(graphics->CreateMeshRenderer(MeshType::M_CUBE, ShaderType::S_TEXTURE_LIT,
			Vec3(0.0f, 5.0f, -192.0f),	// Position
			Vec3(4.0f, 0.2f, 10.0f),	// Size
			Vec3(GetRandomColor()),		// Color
			TEXTURED_LIT_VS_PATH, TEXTURED_LIT_FS_PATH));

	// boxRenderer 7
	boxRenderers.push_back(graphics->CreateMeshRenderer(MeshType::M_CUBE, ShaderType::S_TEXTURE_LIT,
			Vec3(-5.0f, 5.0f, -214.0f),	// Position
			Vec3(4.0f, 0.2f, 10.0f),	// Size
			Vec3(GetRandomColor()),		// Color
			TEXTURED_LIT_VS_PATH, TEXTURED_LIT_FS_PATH));

	// boxRenderer 8
	boxRenderers.push_back(graphics->CreateMeshRenderer(MeshType::M_CUBE, ShaderType::S_TEXTURE_LIT,
			Vec3(0.0f, 5.0f, -255.0f),	// Position
			Vec3(6.0f, 0.2f, 50.0f),	// Size
			Vec3(GetRandomColor()),		// Color
			TEXTURED_LIT_VS_PATH, TEXTURED_LIT_FS_PATH));

	// GOAL
	// boxRenderer 9
	boxRenderers.push_back(graphics->CreateMeshRenderer(MeshType::M_CUBE, ShaderType::S_TEXTURE_LIT,
			Vec3(0.0f, 5.0f, -290.0f),	// Position
			Vec3(30.0f, 0.2f, 20.0f),	// Size
			Vec3(1.0f),					// Color
			TEXTURED_LIT_VS_PATH, TEXTURED_LIT_FS_PATH));
	
	// boxRenderer 10 (back wall)
	boxRenderers.push_back(graphics->CreateMeshRenderer(MeshType::M_CUBE, ShaderType::S_TEXTURE_LIT,
			Vec3(0.0f, 8.0f, -300.0f),	// Position
			Vec3(30.0f, 6.0f, 0.2f),	// Size
			Vec3(1.0f),					// Color
			TEXTURED_LIT_VS_PATH, TEXTURED_LIT_FS_PATH));

	// boxRenderer 11 (left wall)
	boxRenderers.push_back(graphics->CreateMeshRenderer(MeshType::M_CUBE, ShaderType::S_TEXTURE_LIT,
			Vec3(-15.0f, 8.0f, -290.0f),// Position
			Vec3(0.2f, 6.0f, 20.0f),	// Size
			Vec3(1.0f),					// Color
			TEXTURED_LIT_VS_PATH, TEXTURED_LIT_FS_PATH));

	// boxRenderer 12 (right wall)
	boxRenderers.push_back(graphics->CreateMeshRenderer(MeshType::M_CUBE, ShaderType::S_TEXTURE_LIT,
			Vec3(15.0f, 8.0f, -290.0f),	// Position
			Vec3(0.2f, 6.0f, 20.0f),	// Size
			Vec3(1.0f),					// Color
			TEXTURED_LIT_VS_PATH, TEXTURED_LIT_FS_PATH));

	Material floorMaterial;
	floorMaterial.ambientStrength = 0.2f;
	floorMaterial.specularStrength = 0.2f;
	floorMaterial.shininess = 16.0f;
	
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

		graphics->SetMeshRendererMaterial(boxRenderers[i], floorMaterial);

		Vec3 boxPosition = graphics->GetMeshRendererPosition(boxRenderers[i]);
		CoreGeometry::OBB boxGeomery;
		boxGeomery.center = boxPosition;
		boxGeomery.halfExtents = graphics->GetMeshRendererScale(boxRenderers[i]) * 0.5f;

		boxVolumes.push_back(physics->CreateRigidbody(BodyType::B_BOX, boxPosition, 0.0f, 0.9f, 0.2f));
		physics->SetRigidbodyBoxHalfExtents(boxVolumes[i], boxGeomery.halfExtents);
		physics->SetRigidbodyBoxCenter(boxVolumes[i], boxGeomery.center);
	}

	// TO TEST
	Quaternion boxRot = CoreMath::FromEuler(0, 0, 0);;
	physics->SetRigidbodyOrientation(boxVolumes[0], boxRot);
}

float GetRandomColor()
{
    static std::mt19937 gen(static_cast<unsigned int>(std::time(0)));  
    
	std::uniform_real_distribution<float> dis(0.5f, 1.0f);
    
	return dis(gen);
}
