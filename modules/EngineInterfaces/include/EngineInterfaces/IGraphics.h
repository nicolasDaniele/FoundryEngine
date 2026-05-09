#pragma once

#ifdef GRAPHICSENGINE_EXPORTS
	#define GRAPHICS_API __declspec(dllexport)
#else
	#define GRAPHICS_API __declspec(dllimport)
#endif

#include "Core/Vectors.h"
#include "Core/Matrices.h"
#include "Core/Utils.h"
#include "GraphicsTypes.h"
#include "GraphicsPublicData.h"

using Vec2 = CoreMath::Vec2;
using Vec3 = CoreMath::Vec3;
using Mat4 = CoreMath::Mat4;

/// Lightweight handle used to reference a MeshRenderer.
///
/// A handle is composed of:
/// - index: position in the internal slot array
/// - generation: used to validate that the slot has not been reused
///
/// This prevents accessing destroyed objects.
struct MeshRendererHandle
{
    uint32_t index;
    uint32_t generation;
};

class IGraphics
{
public:
    virtual ~IGraphics() = default;

	/// Creates a MeshRenderer and returns a handle to it.
	/// A handle is used to reference the MeshRenderer without exposing internal pointers.
	/// The handle becomes invalid if:
	/// - The MeshRenderer is destroyed.
	/// - The slot is reused (generation mismatch).
	///
	/// @param meshType Type of Mesh to render (triangle, quad, cube, sphere).
	/// @param shaderType Type of shader to render (S_COLOR = flat color; S_TEXTURE = textured material).
	/// @param position Initial world position.
	/// @param scale Initial scale.
	/// @param color Color of the MeshRenderer (in RGB channels, with values from 0 to 1).
	/// @param vertexShaderPath The path to find the vertex shader file for this MeshRenderer.
	/// @param fragmentShaderPath The path to find the fragment shader file for this MeshRenderer.
	/// @return MeshRendererHandle used to reference the object.
	virtual MeshRendererHandle CreateMeshRenderer(MeshType meshType, ShaderType shaderType,
		Vec3 position = Vec3(0.0f),
		Vec3 scale = Vec3(1.0f),
		Vec3 color = Vec3(1.0f),
		const char* vertexShaderPath = "", 
		const char* fragmentShaderPath = "") = 0;

	/// Updates the position of a MeshRenderer.
	/// If the handle is invalid, the call is ignored.
	virtual void UpdateMeshRendererPosition(MeshRendererHandle meshHandle, Vec3 newPosition) = 0;
	/// Loads a texture from the given path and sets it to a MeshRenderer.
	/// If the handle is invalid or the texture cannot be found, 
	/// the call is ignored and the function returns -1.
	virtual int LoadTextureToMeshRenderer(const char* textureFilePath, MeshRendererHandle meshHandle) = 0;
	/// Sets a tiling for a MeshRenderer's texture.
	/// If the handle is invalid or the MeshRenderer was not created with ShaderType::S_TEXTURE, the call is ignored.
	virtual void SetTextureTilingToMeshRenderer(MeshRendererHandle meshHandle, Vec2 tiling) = 0;
	/// Returns a MeshRenderer's current position.
	/// If the handle is invalid, returns Vec3(0.0f).
	virtual Vec3 GetMeshRendererPosition(MeshRendererHandle meshHandle) = 0;
	/// Returns a MeshRenderer's scale.
	/// If the handle is invalid, returns Vec3(0.0f).
	virtual Vec3 GetMeshRendererScale(MeshRendererHandle meshHandle) = 0;
	
	/// Checks whether a handle is still valid.
	///
	/// A handle is valid if:
	/// - index is within bounds
	/// - slot is alive
	/// - generation matches
	virtual bool IsValidMeshRenderer(MeshRendererHandle meshHandle) = 0;
	
	/// Rotates the camera in place (first-person style).
	/// If Graphics' camera is null, the call is ignored.
	/// @param xOffset Amount of yaw variation.
	/// @param yOffset Amount of pitch variation.
	virtual void RotateCamera(float xOffset, float yOffset) = 0;
	
	/// Orbits the camera around a target point (third-person style).
	/// If Graphics' camera is null, the call is ignored.
	/// @param target The poisition to orbit the camera about.
	/// @param distance The distance to maintain from the target.
	/// @param xOffset Amount of yaw variation.
	/// @param yOffset Amount of pitch variation.
	/// @param frameTime Time elapsed since last frame (in seconds).
	/// @param smoothSpeed Used to smoothly lerp the camra's current rotation to the target rotation.
	virtual void CameraOrbit(Vec3 target, float distance, float xOffset, float yOffset, float frameTime, float smoothSpeed) = 0;
	
	/// Moves the camera in a specified direction.
	/// If Graphics' camera is null, the call is ignored.
	/// @param direction Direction to move the camera to (see Utils::Direction).
	/// @param frameTime Time elapsed since last frame (in seconds).
	virtual void MoveCamera(Utils::Direction direction, float frameTime) = 0;

	/// Moves the camera towards a target vector.
	/// @param target The position to move the camera to.
	/// @param distance The distance to maintain from the target.
	/// @param frameTime Time elapsed since last frame (in seconds).
	/// @param smoothSpeed Used to smoothly lerp the camra's current position to the target position.
	virtual void CameraFollow(Vec3 target, float distance, float frameTime, float smoothSpeed) = 0;
		
	
	/// Creates a shader program from the given vertex and fragment shader files.
	/// If one or both shader files are invalid or cannot be found,
	/// the ShaderLoader class prints an error and returns 0.
	/// @param vertexShaderPath The path to find the given vertex shader file.
	/// @param fragmentShaderPath The path to find the given fragment shader file.
	/// @return The handle for the requested shader program (0 = invalid program).
	virtual uint32_t CreateShaderProgram(const char* vertexShaderPath,
		const char* fragmentShaderPath) = 0;
	
	/// Draws a set of lines (mainly aimed to debug)
	/// @param vertices Array of vertices to draw lines from.
	/// @param vertexCount Number of vertices in the array.
	/// @param shaderProgram Shader Program to draw the lines.
	/// @param color Color of the lines to draw.
	virtual void DrawDebugLines(const Vec3* vertices, int vertexCount,
		uint32_t shaderProgram, Vec3 color = Vec3(1.0f, 1.0f, 1.0f)) = 0;

	/// Renders every existing MeshRenderers.
	///
	/// NOTE:
	/// Must be called once per frame after updating scene state.
	virtual void Render() = 0;
};

extern "C"
{
	/// Creates an instance of the IGraphics interface for external use.
	///
	/// NOTE:
	/// The caller owns the returned pointer and must destroy it using DestroyGraphicsEngine.
	/// NOTE:
	/// Requires a valid OpenGL context.
	/// GLAD must be initialized before using this interface.
	///
	/// @param cameraParams Initial configuration of the camera (see GraphicsPublicData.h).
	/// @param loadProc Function used to load OpenGL functions (e.g. glfwGetProcAddress).
	/// @return The created instance of IGraphics.
	GRAPHICS_API IGraphics* GetGraphicsEngine(CameraParams cameraParams, GLADloadproc loadProc);

	/// Destroys a given instance of the IGraphics interface.
	/// @param physics The instance of IGraphics to destroy.
	GRAPHICS_API void DestroyGraphicsEngine(IGraphics* graphicsEngine);
}