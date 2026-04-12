#pragma once

#include <vector>
#include "Vectors.h"
#include "Matrices.h"

using Vec3 = CoreMath::Vec3;
using Mat4 = CoreMath::Mat4;

struct Mesh;
class MeshBuffer;

class MeshRenderer
{
public:
	
	MeshRenderer(MeshBuffer* _meshBuffer, const Vec3& _position = Vec3(0.0f, 0.0f, 0.0f),
		const Vec3& _scale = Vec3(1.0f, 1.0f, 1.0f))
		: meshBuffer(_meshBuffer), position(_position), scale(_scale) { }
	~MeshRenderer() { delete meshBuffer; }

	void Draw(const Mat4& vp);

	void SetShaderProgram(uint32_t _shader);
	void SetMeshBuffer(MeshBuffer* _meshBuffer);
	void SetTexture(uint32_t _textureID);

	Vec3 GetPosition() const;
	void SetPosition(const Vec3& _position);
	void SetScale(const Vec3& _scale);

private:
	MeshBuffer* meshBuffer = nullptr;
	uint32_t shader = 0;
	Vec3 position, scale;
	float rotation;
	uint32_t textureID = -1;
};