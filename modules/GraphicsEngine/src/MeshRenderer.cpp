#include <glad/glad.h>
#include <iostream>
#include <cmath>
#include "GraphicsEngine/MeshRenderer.h"
#include "GraphicsEngine/Mesh.h"

namespace
{
	// Computes the normal matrix (transpose of the inverse of the model
	// matrix's upper-left 3x3) as a column-major 3x3 array, ready for
	// glUniformMatrix3fv with transpose = GL_FALSE.
	//
	// This is a general-purpose 3x3 inverse-transpose, not a shortcut tied to
	// today's translation+scale-only transforms. That means MeshRenderer does
	// not need any changes here once rotation support is added later - it
	// will keep producing correct normals for any combination of rotation
	// and non-uniform scale.
	void ComputeNormalMatrix3x3(const Mat4& model, float outNormalMatrix[9])
	{
		// Upper-left 3x3 of a column-major Mat4 (matches how model is already
		// uploaded via glUniformMatrix4fv(..., GL_FALSE, model.asArray)).
		float m0 = model.asArray[0], m1 = model.asArray[1], m2 = model.asArray[2];
		float m3 = model.asArray[4], m4 = model.asArray[5], m5 = model.asArray[6];
		float m6 = model.asArray[8], m7 = model.asArray[9], m8 = model.asArray[10];

		float i00 = m4 * m8 - m7 * m5;
		float i01 = m6 * m5 - m3 * m8;
		float i02 = m3 * m7 - m6 * m4;
		float i10 = m7 * m2 - m1 * m8;
		float i11 = m0 * m8 - m6 * m2;
		float i12 = m6 * m1 - m0 * m7;
		float i20 = m1 * m5 - m4 * m2;
		float i21 = m3 * m2 - m0 * m5;
		float i22 = m0 * m4 - m3 * m1;

		float det = m0 * i00 + m3 * i10 + m6 * i20;

		if (std::fabs(det) < 1e-8f)
		{
			// Degenerate transform (e.g. a zero scale axis) - fall back to
			// identity instead of dividing by ~0.
			outNormalMatrix[0] = 1.0f; outNormalMatrix[1] = 0.0f; outNormalMatrix[2] = 0.0f;
			outNormalMatrix[3] = 0.0f; outNormalMatrix[4] = 1.0f; outNormalMatrix[5] = 0.0f;
			outNormalMatrix[6] = 0.0f; outNormalMatrix[7] = 0.0f; outNormalMatrix[8] = 1.0f;
			return;
		}

		float invDet = 1.0f / det;

		// Writing the (row-major) inverse's entries out in row order here
		// produces the (column-major) transpose of the inverse - exactly the
		// layout glUniformMatrix3fv expects - with no separate transpose step.
		outNormalMatrix[0] = i00 * invDet; outNormalMatrix[1] = i01 * invDet; outNormalMatrix[2] = i02 * invDet;
		outNormalMatrix[3] = i10 * invDet; outNormalMatrix[4] = i11 * invDet; outNormalMatrix[5] = i12 * invDet;
		outNormalMatrix[6] = i20 * invDet; outNormalMatrix[7] = i21 * invDet; outNormalMatrix[8] = i22 * invDet;
	}
}

bool MeshRenderer::IsLit() const
{
	ShaderType type = meshBuffer->GetShaderType();
	return type == S_COLOR_LIT || type == S_TEXTURE_LIT;
}

bool MeshRenderer::IsTextured() const
{
	ShaderType type = meshBuffer->GetShaderType();
	return type == S_TEXTURE || type == S_TEXTURE_LIT;
}

void MeshRenderer::InitUniforms()
{
	modelLoc = glGetUniformLocation(shader, "model");

	// Lit shaders read the view-projection matrix from the FrameData UBO
	// instead of a per-mesh uniform.
	if (!IsLit())
		vpLoc = glGetUniformLocation(shader, "vp");

	if (IsLit())
	{
		normalMatrixLoc     = glGetUniformLocation(shader, "normalMatrix");
		ambientStrengthLoc  = glGetUniformLocation(shader, "ambientStrength");
		diffuseStrengthLoc  = glGetUniformLocation(shader, "diffuseStrength");
		specularStrengthLoc = glGetUniformLocation(shader, "specularStrength");
		shininessLoc        = glGetUniformLocation(shader, "shininess");
	}

	if (IsTextured())
	{
		texLoc = glGetUniformLocation(shader, "uTexture");
		tilingLoc = glGetUniformLocation(shader, "uTiling");
	}
}

void MeshRenderer::UpdateNormalMatrix()
{
	ComputeNormalMatrix3x3(modelMat, normalMatrix3);
}

void MeshRenderer::Draw(const Mat4& vp)
{
	if (hasChanged)
	{
		Mat4 translationMat = CoreMath::Translation(position);
		Mat4 scaleMat = CoreMath::Scale(scale);
		modelMat = translationMat * scaleMat;

		if (IsLit())
			UpdateNormalMatrix();

		hasChanged = false;
	}

	glUseProgram(shader);
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, modelMat.asArray);

	if (!IsLit())
	{
		glUniformMatrix4fv(vpLoc, 1, GL_FALSE, vp.asArray);
	}
	else
	{
		glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, normalMatrix3);
		glUniform1f(ambientStrengthLoc, material.ambientStrength);
		glUniform1f(diffuseStrengthLoc, material.diffuseStrength);
		glUniform1f(specularStrengthLoc, material.specularStrength);
		glUniform1f(shininessLoc, material.shininess);
	}

	if (IsTextured())
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureID);

		glUniform1i(texLoc, 0);
		glUniform2f(tilingLoc, textureTiling.x, textureTiling.y);
	}

	meshBuffer->Bind();
	glDrawElements(GL_TRIANGLES, meshBuffer->GetIndexCount(),
		GL_UNSIGNED_INT, nullptr);

	meshBuffer->Unbind();
}

void MeshRenderer::SetShaderProgram(uint32_t _shader)
{
	shader = _shader;
}

void MeshRenderer::SetTexture(uint32_t _textureID)
{
	textureID = _textureID;
}

void MeshRenderer::SetTextureTiling(const Vec2& _tiling)
{
	if (!IsTextured()) return;

	textureTiling = _tiling;
}

void MeshRenderer::SetMaterial(const Material& _material)
{
	material = _material;
}

Vec3 MeshRenderer::GetPosition() const
{
	return position;
}

Vec3 MeshRenderer::GetScale() const
{
	return scale;
}

void MeshRenderer::SetPosition(const Vec3& _position)
{
	position = _position;
	hasChanged = true;
}

void MeshRenderer::SetScale(const Vec3& _scale)
{
	scale = _scale;
	hasChanged = true;
}
