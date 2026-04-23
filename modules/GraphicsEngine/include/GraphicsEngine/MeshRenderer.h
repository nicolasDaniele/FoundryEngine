#pragma once

#include <vector>
#include "Core/Vectors.h"
#include "Core/Matrices.h"

using Vec2 = CoreMath::Vec2;
using Vec3 = CoreMath::Vec3;
using Mat4 = CoreMath::Mat4;

struct Mesh;
class MeshBuffer;

class MeshRenderer
{
public:
	
	MeshRenderer(MeshBuffer* _meshBuffer, const Vec3& _position = Vec3(0.0f, 0.0f, 0.0f),
		const Vec3& _scale = Vec3(1.0f, 1.0f, 1.0f));
	~MeshRenderer() { delete meshBuffer; }

	void InitUniforms();
	void Draw(const Mat4& vp);

	void SetShaderProgram(uint32_t _shader);
	void SetMeshBuffer(MeshBuffer* _meshBuffer);
	void SetTexture(uint32_t _textureID);
	void SetTextureTiling(const Vec2& _tiling) { textureTiling = _tiling; }

	Vec3 GetPosition() const;
	void SetPosition(const Vec3& _position);
	void SetScale(const Vec3& _scale);

private:
	MeshBuffer* meshBuffer = nullptr;
	uint32_t shader = 0;
	Mat4 modelMat = Mat4();
	Vec3 position, scale = Vec3(0.0f, 0.0f, 0.0f);
	GLint vpLoc, modelLoc, texLoc, tilingLoc;
	uint32_t textureID = -1;
	Vec2 textureTiling = Vec2(1.0f, 1.0f);

	bool hasChanged = true;
};