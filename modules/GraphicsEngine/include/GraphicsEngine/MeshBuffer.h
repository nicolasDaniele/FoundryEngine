#pragma once

#include <cstdint>
#include "EngineInterfaces/GraphicsTypes.h"

struct Mesh;

class MeshBuffer
{
public:
    MeshBuffer();
    ~MeshBuffer();

    void LoadMeshData(const Mesh& mesh, ShaderType shaderType);
    void Bind();
    void Unbind();

    uint32_t GetIndexCount() const;

private:
    uint32_t vao = 0, vbo = 0, ebo = 0;
    uint32_t indexCount = 0;
};