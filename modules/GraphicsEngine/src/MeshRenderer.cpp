#include <glad/glad.h>
#include <iostream>
#include "GraphicsEngine/MeshRenderer.h"
#include "GraphicsEngine/Mesh.h"

void MeshRenderer::InitUniforms()
{
	vpLoc = glGetUniformLocation(shader, "vp");
	modelLoc = glGetUniformLocation(shader, "model");

	if(meshBuffer->GetShaderType() == ShaderType::S_TEXTURE)
	{
		texLoc = glGetUniformLocation(shader, "Texture");
		tilingLoc = glGetUniformLocation(shader, "uTiling");
	}
}

void MeshRenderer::Draw(const Mat4& vp)
{
	if (hasChanged)
	{
		Mat4 translationMat = CoreMath::Translation(position);
		Mat4 scaleMat = CoreMath::Scale(scale);
		modelMat = translationMat * scaleMat;

		hasChanged = false;
	}

	glUseProgram(shader);
	glUniformMatrix4fv(vpLoc, 1, GL_FALSE, vp.asArray);
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, modelMat.asArray);

	if(meshBuffer->GetShaderType() == ShaderType::S_TEXTURE)
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
	textureTiling = _tiling;
}

Vec3 MeshRenderer::GetPosition() const
{
	return position;
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