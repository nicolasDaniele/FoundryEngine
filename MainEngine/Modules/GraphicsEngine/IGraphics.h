#pragma once

#include "Vectors.h"
#include "Matrices.h"

using Vec3 = CoreMath::Vec3;
using Mat4 = CoreMath::Mat4;

class IGraphics
{
public:
    virtual ~IGraphics() = default;

    virtual uint32_t CreateShaderProgram(const char* vertexShaderPath,
        const char* fragmentShaderPath) = 0;

    virtual void DrawDebugLines(const Vec3* vertices, int vertexCount,
        uint32_t shaderProgram, Vec3 color = Vec3(1.0f, 1.0f, 1.0f)) = 0;
};