#pragma once

#include "Core/Vectors.h"
#include "Core/Matrices.h"
#include "GraphicsEngine/MeshBuffer.h"

using Vec3 = CoreMath::Vec3;
using Mat4 = CoreMath::Mat4;

struct Mesh;

class MeshRenderer
{
public:
	MeshRenderer() {}
	MeshRenderer(std::unique_ptr<MeshBuffer> _meshBuffer, const Vec3& _position = Vec3(0.0f, 0.0f, 0.0f),
		const Vec3& _scale = Vec3(1.0f, 1.0f, 1.0f))
		: meshBuffer(std::move(_meshBuffer)), position(_position), scale(_scale) { }
	~MeshRenderer();

	MeshRenderer(const MeshRenderer&) = delete;
	MeshRenderer& operator=(const MeshRenderer&) = delete;

	MeshRenderer(MeshRenderer&&) = default;
	MeshRenderer& operator=(MeshRenderer&&) = default;

	void Draw(const Mat4& vp);

	void SetShaderProgram(uint32_t _shader);
	//void SetMeshBuffer(std::unique_ptr<MeshBuffer> _meshBuffer);
	void SetTexture(uint32_t _textureID);

	Vec3 GetPosition() const;
	void SetPosition(const Vec3& _position);
	void SetScale(const Vec3& _scale);

private:
	std::unique_ptr<MeshBuffer> meshBuffer = nullptr;
	uint32_t shader = 0;
	Vec3 position, scale;
	float rotation;
	uint32_t textureID = -1;
};