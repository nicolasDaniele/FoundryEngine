#pragma once

#include "Vectors.h"
#include <vector>

using Vec3 = CoreMath::Vec3;
using Vec2 = CoreMath::Vec2;

struct Vertex
{
	Vec3 position;
	Vec3 normal;
	Vec3 color;
	Vec2 texCoord;
};

struct Mesh
{
	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;
};