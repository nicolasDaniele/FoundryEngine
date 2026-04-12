#pragma once

#include "Vectors.h"
#include "GraphicsTypes.h"

using Vec3 = CoreMath::Vec3;

class Mesh;

const float PI = 3.141592f;
const float SPHERE_RADIUS = 1.0f;
const int SECTOR_COUNT = 36;
const int STACK_COUNT = 18;

class MeshFactory
{
public:
    static Mesh CreateMesh(MeshType meshType, const Vec3& color = Vec3(1.0f, 1.0f, 1.0f));

private:
    static Mesh CreateTriangle(const Vec3& color = Vec3(1.0f, 1.0f, 1.0f));
    static Mesh CreateQuad(const Vec3& color = Vec3(1.0f, 1.0f, 1.0f));
    static Mesh CreateCube(const Vec3& color = Vec3(1.0f, 1.0f, 1.0f));
    static Mesh CreateSphere(const Vec3& color = Vec3(1.0f, 1.0f, 1.0f));
};