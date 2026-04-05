#include <glad/glad.h>
#include "MeshBuffer.h"
#include "Mesh.h"

MeshBuffer::MeshBuffer()
{
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ebo);
}

MeshBuffer::~MeshBuffer()
{
	if (ebo) glDeleteBuffers(1, &ebo);
	if (vbo) glDeleteBuffers(1, &vbo);
	if (vao) glDeleteVertexArrays(1, &vao);
}

void MeshBuffer::LoadMeshData(const Mesh& mesh, ShaderType shaderType)
{
    indexCount = static_cast<uint32_t>(mesh.indices.size());

    glBindVertexArray(vao);

    // Vertex buffer
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(
        GL_ARRAY_BUFFER,
        mesh.vertices.size() * sizeof(Vertex),
        mesh.vertices.data(),
        GL_STATIC_DRAW
    );

    // Index buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        mesh.indices.size() * sizeof(uint32_t),
        mesh.indices.data(),
        GL_STATIC_DRAW
    );

    // --- Vertex layout ---
    // position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
        0, 3, GL_FLOAT, GL_FALSE,
        sizeof(Vertex),
        (void*)offsetof(Vertex, position)
    );

    // normal
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(
        1, 3, GL_FLOAT, GL_FALSE,
        sizeof(Vertex),
        (void*)offsetof(Vertex, normal)
    );

    switch (shaderType)
    {
    case S_COLOR:
        // color
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(
            2, 3, GL_FLOAT, GL_FALSE,
            sizeof(Vertex),
            (void*)offsetof(Vertex, color)
        );
        break;
    case S_TEXTURE:
        // texcoord
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(
            3, 2, GL_FLOAT, GL_FALSE,
            sizeof(Vertex),
            (void*)offsetof(Vertex, texCoord)
        );
        break;
    }

    glBindVertexArray(0);
}

void MeshBuffer::Bind()
{
	glBindVertexArray(vao);
}

void MeshBuffer::Unbind()
{
    glBindVertexArray(0);
}

//void MeshBuffer::Draw() const
//{
//	glBindVertexArray(vao);
//	glDrawElements(
//		GL_TRIANGLES,
//		indexCount,
//		GL_UNSIGNED_INT,
//		nullptr
//	);
//}

uint32_t MeshBuffer::GetIndexCount() const
{
    return indexCount;
}