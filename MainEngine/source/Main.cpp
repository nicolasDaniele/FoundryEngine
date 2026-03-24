#include <glad/glad.h>
#include <glfw3.h>
#include <iostream>
#include "GraphicsEngine/GraphicsApi.h"
#include "Debugger/DebugRenderer.h"
#include "PhysicsEngine/PhysicsApi.h"
#include "Geometry3D.h"
#include "Vectors.h"

using Vec3 = CoreMath::Vec3;

const int WIDTH = 960;
const int HEIGHT = 600;

const char* FLAT_VS_PATH = "Assets/Shaders/FlatModel.vs";
const char* FLAT_FS_PATH = "Assets/Shaders/FlatModel.fs";

float lastMouseXPos = WIDTH / 2.0f;
float lastMouseYPos = HEIGHT / 2.0f;
bool firstMouse = true;
float frameTime = 0.0f;
float lastFrame = 0.0f;

RigidbodyVolume* ballVolume = nullptr;
RigidbodyVolume* cubeVolume = nullptr;
RigidbodyVolume* floorVolume = nullptr;
RigidbodyVolume* leftWallVolume = nullptr;
RigidbodyVolume* rightWallVolume = nullptr;
RigidbodyVolume* backWallVolume = nullptr;
CoreGeometry::Sphere ballDebug;
CoreGeometry::OBB cubeDebug;
CoreGeometry::OBB floorDebug;
CoreGeometry::OBB leftWallDebug;
CoreGeometry::OBB rightWallDebug;
CoreGeometry::OBB backWallDebug;

void HandleInput(GLFWwindow* window, float frameTime);
void MouseMoveCallback(GLFWwindow* window, double xposIn, double yposIn);

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

	glfwSetCursorPosCallback(window, MouseMoveCallback);
	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "[MainEngine] Failed to initialize GLAD";
		return -1;
	}

	// ------------ Objects Variables ------------- \\

	Vec3 ballPosition = Vec3(-2.f, 4.0f, 0.0f);
	Vec3 ballSize = Vec3(0.5f, 0.5f, 0.5f);

	Vec3 cubePosition = Vec3(-2.f, 1.0f, 0.0f);
	Vec3 cubeSize = Vec3(3.0f, 0.2f, 3.0f);

	Vec3 floorPosition = Vec3(0.0f, -2.0f, 0.0f);
	Vec3 floorSize = Vec3(12.0f, 0.2f, 12.0f);

	Vec3 leftWallPosition = Vec3(-6.0f, 0.9f, 0.0f);
	Vec3 leftWallSize = Vec3(0.2f, 6.0f, 12.0f);

	Vec3 rightWallPosition = Vec3(6.0f, 0.9f, 0.0f);
	Vec3 rightWallSize = Vec3(0.2f, 6.0f, 12.0f);

	Vec3 backWallPosition = Vec3(0.0f, 0.9f, -6.0f);
	Vec3 backWallSize = Vec3(12.0f, 6.0f, 0.2f);

	// -------------------- GRAHICS SETUP -------------------- \\

	CameraParams cameraParams;
	cameraParams.fieldOfView = 45.0f;
	cameraParams.width = WIDTH;
	cameraParams.height = HEIGHT;
	cameraParams.nearPlane = 0.1f;
	cameraParams.farPlane = 100.0f;
	cameraParams.position = Vec3(0.0f, 1.5f, 16.0f);

	Graphics* graphics = GetGraphicsEngine(cameraParams);

	MeshRenderer* ballRenderer = CreateMeshRenderer(graphics, MeshType::M_SPHERE,
		ballPosition,
		ballSize,
		Vec3(0.9f, 0.2f, 0.0f), // Color
		FLAT_VS_PATH, FLAT_FS_PATH);
	 
	MeshRenderer* cubeRenderer = CreateMeshRenderer(graphics, MeshType::M_CUBE,
		cubePosition,
		cubeSize,
		Vec3(0.9f, 0.1f, 0.1f), // Color
		FLAT_VS_PATH, FLAT_FS_PATH);

	MeshRenderer* floorRenderer = CreateMeshRenderer(graphics, MeshType::M_CUBE,
		floorPosition,
		floorSize,
		Vec3(0.2f, 0.8f, 0.2f), // Color
		FLAT_VS_PATH, FLAT_FS_PATH);

	MeshRenderer* leftWallRenderer = CreateMeshRenderer(graphics, MeshType::M_CUBE,
		leftWallPosition,
		leftWallSize,
		Vec3(0.2f, 0.7f, 0.2f), // Color
		FLAT_VS_PATH, FLAT_FS_PATH);

	MeshRenderer* rightWallRenderer = CreateMeshRenderer(graphics, MeshType::M_CUBE,
		rightWallPosition,
		rightWallSize,
		Vec3(0.2f, 0.7f, 0.2f), // Color
		FLAT_VS_PATH, FLAT_FS_PATH);

	MeshRenderer* backWallRenderer = CreateMeshRenderer(graphics, MeshType::M_CUBE,
		backWallPosition,
		backWallSize,
		Vec3(0.2f, 0.6f, 0.2f), // Color
		FLAT_VS_PATH, FLAT_FS_PATH);

	// ------------------- END GRAHICS SETUP ------------------- \\


	// ----------------------- DEBUG SETUP ----------------------- \\

	Debugger::DebugRenderer* debugRenderer = new Debugger::DebugRenderer(graphics);

	ballDebug.center = ballPosition;
	ballDebug.radius = ballSize.y;// *0.5f;

	cubeDebug.center = cubePosition;
	cubeDebug.halfExtents = cubeSize * 0.5f;
	cubeDebug.orientation = Mat3();
	
	floorDebug.center = floorPosition;
	floorDebug.halfExtents = floorSize * 0.5f;
	floorDebug.orientation = Mat3();

	leftWallDebug.center = leftWallPosition;
	leftWallDebug.halfExtents = leftWallSize * 0.5f;
	leftWallDebug.orientation = Mat3();

	rightWallDebug.center = rightWallPosition;
	rightWallDebug.halfExtents = rightWallSize * 0.5f;
	rightWallDebug.orientation = Mat3();

	backWallDebug.center = backWallPosition;
	backWallDebug.halfExtents = backWallSize * 0.5f;
	backWallDebug.orientation = Mat3();

	// ---------------------- END DEBUG SETUP ---------------------- \\


	// ---------------------- PHYSICS SETUP ---------------------- \\
	
	PhysicsSystem* physics = GetPhysicsSystem();

	float collisionRestitution = 0.7f;

	ballVolume = GetRigidbody(2, ballPosition, 1.0f, 1.0f, collisionRestitution);
	SetRigidbodySphereRadius(ballVolume, ballDebug.radius);

	cubeVolume = GetRigidbody(3, cubePosition, 0.6f, 1.0f, 0.8f);
	SetRigidbodyBoxHalfExtents(cubeVolume, cubeDebug.halfExtents);
	SetRigidbodyBoxCenter(cubeVolume, cubeDebug.center);
	SetRigidbodyBoxOrientation(cubeVolume, cubeDebug.orientation);

	floorVolume = GetRigidbody(3, floorPosition, 0.0f, 1.0f, collisionRestitution);
	SetRigidbodyBoxHalfExtents(floorVolume, floorDebug.halfExtents);
	SetRigidbodyBoxCenter(floorVolume, floorDebug.center);
	SetRigidbodyBoxOrientation(floorVolume, floorDebug.orientation);

	leftWallVolume = GetRigidbody(3, leftWallPosition, 0.0f, 0.6f, collisionRestitution);
	SetRigidbodyBoxHalfExtents(leftWallVolume, leftWallDebug.halfExtents);
	SetRigidbodyBoxCenter(leftWallVolume, leftWallDebug.center);
	SetRigidbodyBoxOrientation(leftWallVolume, leftWallDebug.orientation);

	rightWallVolume = GetRigidbody(3, rightWallPosition, 0.0f, 0.6f, collisionRestitution);
	SetRigidbodyBoxHalfExtents(rightWallVolume, rightWallDebug.halfExtents);
	SetRigidbodyBoxCenter(rightWallVolume, rightWallDebug.center);
	SetRigidbodyBoxOrientation(rightWallVolume, rightWallDebug.orientation);

	backWallVolume = GetRigidbody(3, backWallPosition, 0.0f, 0.6f, collisionRestitution);
	SetRigidbodyBoxHalfExtents(backWallVolume, backWallDebug.halfExtents);
	SetRigidbodyBoxCenter(backWallVolume, backWallDebug.center);
	SetRigidbodyBoxOrientation(backWallVolume, backWallDebug.orientation);

	AddRigidbodyToPhysicsSystem((Rigidbody*)ballVolume, physics);
	AddRigidbodyToPhysicsSystem((Rigidbody*)cubeVolume, physics);
	AddRigidbodyToPhysicsSystem((Rigidbody*)floorVolume, physics);
	AddRigidbodyToPhysicsSystem((Rigidbody*)leftWallVolume, physics);
	AddRigidbodyToPhysicsSystem((Rigidbody*)rightWallVolume, physics);
	AddRigidbodyToPhysicsSystem((Rigidbody*)backWallVolume, physics);


	// --------------------- END PHYSICS SETUP --------------------- \\

	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = static_cast<float>(glfwGetTime());
		frameTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// Physics update
		UpdatePhysicsSystem(physics, frameTime);

		// Graphics update
		ballPosition = GetRigidbodyPosition(ballVolume);
		UpdateMeshRendererPosition(ballRenderer, ballPosition);

		cubePosition = GetRigidbodyPosition(cubeVolume);
		UpdateMeshRendererPosition(cubeRenderer, cubePosition);
		
		floorPosition = GetRigidbodyPosition(floorVolume);
		UpdateMeshRendererPosition(floorRenderer, floorPosition);

		leftWallPosition = GetRigidbodyPosition(leftWallVolume);
		UpdateMeshRendererPosition(leftWallRenderer, leftWallPosition);

		rightWallPosition = GetRigidbodyPosition(rightWallVolume);
		UpdateMeshRendererPosition(rightWallRenderer, rightWallPosition);

		backWallPosition = GetRigidbodyPosition(backWallVolume);
		UpdateMeshRendererPosition(backWallRenderer, backWallPosition);

		graphics->Render();

		// Debug update
		ballDebug.center = ballPosition;
		cubeDebug.center = cubePosition;
		floorDebug.center = floorPosition;
		leftWallDebug.center = leftWallPosition;
		rightWallDebug.center = rightWallPosition;
		backWallDebug.center = backWallPosition;

		debugRenderer->Clear();
		debugRenderer->AddSphere(ballDebug);
		debugRenderer->AddBox(cubeDebug);
		debugRenderer->AddBox(floorDebug);
		debugRenderer->AddBox(leftWallDebug);
		debugRenderer->AddBox(rightWallDebug);
		debugRenderer->AddBox(backWallDebug);
		debugRenderer->DrawDebug(Vec3(1.0f, 0.0f, 0.0f));

		glfwSwapBuffers(window);
		glfwPollEvents();
		HandleInput(window, frameTime);
	}

	DestroyGraphicsEngine(graphics);
	DestroyPhysicsSystem(physics);
	glfwTerminate();

	return 0;
}

void HandleInput(GLFWwindow* window, float frameTime)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	float speed = 10.0f;
	float verticalVel = 40.0f;

	// Ball Movement
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS && ballVolume != nullptr)
		AddLinearImpulseToRigidbody(ballVolume, Vec3(-speed, 0.0f, 0.0f) * frameTime);

	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS && ballVolume != nullptr)
		AddLinearImpulseToRigidbody(ballVolume, Vec3(speed, 0.0f, 0.0f) * frameTime);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS && ballVolume != nullptr)
		AddLinearImpulseToRigidbody(ballVolume, Vec3(0.0f, 0.0f, -speed) * frameTime);

	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS && ballVolume != nullptr)
		AddLinearImpulseToRigidbody(ballVolume, Vec3(0.0f, 0.0f, speed) * frameTime);

	if (glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS && ballVolume != nullptr)
		AddLinearImpulseToRigidbody(ballVolume, Vec3(0.0f, verticalVel, 0.0f) * frameTime);
	
	//Cube Movement
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS && cubeVolume != nullptr)
		AddLinearImpulseToRigidbody(cubeVolume, Vec3(-speed, 0.0f, 0.0f) * frameTime);

	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS && cubeVolume != nullptr)
		AddLinearImpulseToRigidbody(cubeVolume, Vec3(speed, 0.0f, 0.0f) * frameTime);

	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS && cubeVolume != nullptr)
		AddLinearImpulseToRigidbody(cubeVolume, Vec3(0.0f, 0.0f, -speed) * frameTime);

	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS && cubeVolume != nullptr)
		AddLinearImpulseToRigidbody(cubeVolume, Vec3(0.0f, 0.0f, speed) * frameTime);

	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && cubeVolume != nullptr)
		AddLinearImpulseToRigidbody(cubeVolume, Vec3(0.0f, verticalVel, 0.0f) * frameTime);
}

void MouseMoveCallback(GLFWwindow* window, double xposIn, double yposIn)
{
	float xpos = static_cast<float>(xposIn);
	float ypos = static_cast<float>(yposIn);

	if (firstMouse)
	{
		lastMouseXPos = xpos;
		lastMouseYPos = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastMouseXPos;
	float yoffset = lastMouseYPos - ypos;

	lastMouseXPos = xpos;
	lastMouseYPos = ypos;
}