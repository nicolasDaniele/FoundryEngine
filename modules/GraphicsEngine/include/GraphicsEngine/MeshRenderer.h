#pragma once

#include "Core/Vectors.h"
#include "Core/Matrices.h"
#include "GraphicsEngine/MeshBuffer.h"
#include "EngineInterfaces/GraphicsPublicData.h"

using Vec2 = CoreMath::Vec2;
using Vec3 = CoreMath::Vec3;
using Mat4 = CoreMath::Mat4;

struct Mesh;

class MeshRenderer
{
public:
	MeshRenderer() { }
	MeshRenderer(std::unique_ptr<MeshBuffer> _meshBuffer, const Vec3& _position = Vec3(0.0f, 0.0f, 0.0f),
		const Vec3& _scale = Vec3(1.0f, 1.0f, 1.0f))
		: meshBuffer(std::move(_meshBuffer)), position(_position), scale(_scale) { }
	~MeshRenderer() = default;

	MeshRenderer(const MeshRenderer&) = delete;
	MeshRenderer& operator=(const MeshRenderer&) = delete;

	MeshRenderer(MeshRenderer&&) = default;
	MeshRenderer& operator=(MeshRenderer&&) = default;

	void InitUniforms();
	void Draw(const Mat4& vp);

	void SetShaderProgram(uint32_t _shader);
	void SetTexture(uint32_t _textureID);
	void SetTextureTiling(const Vec2& _tiling);
	void SetMaterial(const Material& _material);

	Vec3 GetPosition() const;
	Vec3 GetScale() const;
	void SetPosition(const Vec3& _position);
	void SetScale(const Vec3& _scale);

private:
	std::unique_ptr<MeshBuffer> meshBuffer = nullptr;
	uint32_t shader = 0;
	Mat4 modelMat = Mat4();
	Vec3 position = Vec3(0.0f, 0.0f, 0.0f);
	Vec3 scale = Vec3(1.0f, 1.0f, 1.0f);
	Material material;

	GLint vpLoc = -1, modelLoc = -1, texLoc = -1, tilingLoc = -1;
	GLint normalMatrixLoc = -1, ambientStrengthLoc = -1, diffuseStrengthLoc = -1,
	      specularStrengthLoc = -1, shininessLoc = -1;

	// Column-major 3x3 normal matrix, ready for glUniformMatrix3fv. Recomputed
	// only when the transform changes (see hasChanged), using a general
	// inverse-transpose so it stays correct once rotation support is added to
	// MeshRenderer - no changes will be needed here at that point.
	float normalMatrix3[9] = { 1,0,0, 0,1,0, 0,0,1 };

	uint32_t textureID = -1;
	Vec2 textureTiling = Vec2(1.0f, 1.0f);

	bool hasChanged = true;

	bool IsLit() const;
	bool IsTextured() const;
	void UpdateNormalMatrix();
};
