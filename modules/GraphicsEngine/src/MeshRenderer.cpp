#include <glad/glad.h>
#include <iostream>
#include "GraphicsEngine/MeshRenderer.h"
#include "GraphicsEngine/Mesh.h"
#include "GraphicsEngine/MeshBuffer.h"

void MeshRenderer::Draw(const Mat4& vp)
{
	Mat4 translationMat = CoreMath::Translation(position);
	Mat4 scaleMat = CoreMath::Scale(scale);

	Mat4 modelMat = translationMat * scaleMat;

	glUseProgram(shader);
	GLint vpLoc = glGetUniformLocation(shader, "vp");
	glUniformMatrix4fv(vpLoc, 1, GL_FALSE, vp.asArray);
	
	GLint modelLoc = glGetUniformLocation(shader, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, modelMat.asArray);

	if (textureID == -1)
	{
		std::cout << "[MeshRenderer] No Texture Assigned" << std::endl;
	}
	else
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureID);
		glUniform1i(glGetUniformLocation(shader, "Texture"), 0);
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

void MeshRenderer::SetMeshBuffer(MeshBuffer* _meshBuffer)
{
	meshBuffer = _meshBuffer;
}

void MeshRenderer::SetTexture(uint32_t _textureID)
{
	textureID = _textureID;
}

Vec3 MeshRenderer::GetPosition() const
{
	return position;
}

void MeshRenderer::SetPosition(const Vec3& _position)
{
	position = _position;
}

void MeshRenderer::SetScale(const Vec3& _scale)
{
	scale = _scale;
}