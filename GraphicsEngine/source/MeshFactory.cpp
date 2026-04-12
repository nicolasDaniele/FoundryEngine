#include "MeshFactory.h"
#include "Mesh.h"
#include "MathDefinitions.h"

Mesh MeshFactory::CreateMesh(MeshType meshType, const Vec3& color)
{
	switch (meshType)
	{
	case M_TRIANGLE:
		return CreateTriangle(color);
		break;
	case M_QUAD:
		return CreateQuad(color);
		break;
	case M_CUBE:
		return CreateCube(color);
		break;
	case M_SPHERE:
		return CreateSphere(color);
		break;
	default:
		return Mesh();
		break;
	}
}

Mesh MeshFactory::CreateTriangle(const Vec3& color)
{
	Mesh mesh;

	mesh.vertices.clear();
	mesh.indices.clear();

	mesh.vertices = {
		// Vertex 0 (bottom left)
		{ 
			{ -1.0f, -1.0f, 0.0f },	// Position
			{ 0.0f, 0.0f, 1.0f },	// Normal
			color,					// Color
			{ 0.0f, 0.0f }			// Texture Coordinate
		},
		// Vertex 1 (bottom right)
		{ 
			{ 1.0f, -1.0f, 0.0f },	// Position
			{ 0.0f, 0.0f, 1.0f },	// Normal
			color,					// Color
			{ 1.0f, 0.0f }			// Texture Coordinate
		},
		// Vertex 2 (top)
		{ 
			{ 0.0f, 1.0f, 0.0f },	// Position
			{ 0.0f, 0.0f, 1.0f },	// Normal
			color,					// Color
			{ 0.5f, 1.0f }			// Texture Coordinate
		}
	};

	mesh.indices = {
		0, 1, 2
	};

	return mesh;
}

Mesh MeshFactory::CreateQuad(const Vec3& color)
{
	Mesh mesh;

	mesh.vertices.clear();
	mesh.indices.clear();

	mesh.vertices = {
		// Vertex 0 (top right)
		{
			{ 1.0f, 1.0f, 0.0f },	// Position
			{ 0.0f, 0.0f, 1.0f },	// Normal
			color,					// Color
			{ 1.0f, 1.0f }			// Texture Coordinate
		},
		// Vertex 1 (bottom right)
		{
			{ 1.0f, -1.0f, 0.0f },	// Position
			{ 0.0f, 0.0f, 1.0f },	// Normal
			color,					// Color
			{ 1.0f, 0.0f }			// Texture Coordinate
		},
		// Vertex 2 (bottom left)
		{
			{ -1.0f, -1.0f, 0.0f },	// Position
			{ 0.0f, 0.0f, 1.0f },	// Normal
			color,					// Color
			{ 0.0f, 0.0f }			// Texture Coordinate
		},
		// Vertex 3 (top left)
		{
			{ -1.0f, 1.0f, 0.0f },	// Position
			{ 0.0f, 0.0f, 1.0f },	// Normal
			color,					// Color
			{ 0.0f, 1.0f }			// Texture Coordinate
		}
	};

	mesh.indices = {
		0, 1, 3,
		1, 2, 3
	};

	return mesh;
}

Mesh MeshFactory::CreateCube(const Vec3& color)
{
	Mesh mesh;
	mesh.vertices.clear();
	mesh.indices.clear();

	mesh.vertices = {
		// Position					// Normal				// Color			// TexCoord
		{ {-0.5f, -0.5f, -0.5f},  {0.0f, 0.0f, 0.0f},		  color,			{0.0f, 0.0f} },
		{ { 0.5f, -0.5f, -0.5f},  {0.0f, 0.0f, 0.0f},		  color,			{1.0f, 0.0f} },
		{ { 0.5f,  0.5f, -0.5f},  {0.0f, 0.0f, 0.0f},		  color,			{1.0f, 1.0f} },
		{ { 0.5f,  0.5f, -0.5f},  {0.0f, 0.0f, 0.0f},		  color,			{1.0f, 1.0f} },
		{ {-0.5f,  0.5f, -0.5f},  {0.0f, 0.0f, 0.0f},		  color,			{0.0f, 1.0f} },
		{ {-0.5f, -0.5f, -0.5f},  {0.0f, 0.0f, 0.0f},		  color,			{0.0f, 0.0f} },
		 												  	  						
		{ {-0.5f, -0.5f,  0.5f},  {0.0f, 0.0f, 0.0f},		  color,			{0.0f, 0.0f} },
		{ { 0.5f, -0.5f,  0.5f},  {0.0f, 0.0f, 0.0f},		  color,			{1.0f, 0.0f} },
		{ { 0.5f,  0.5f,  0.5f},  {0.0f, 0.0f, 0.0f},		  color,			{1.0f, 1.0f} },
		{ { 0.5f,  0.5f,  0.5f},  {0.0f, 0.0f, 0.0f},		  color,			{1.0f, 1.0f} },
		{ {-0.5f,  0.5f,  0.5f},  {0.0f, 0.0f, 0.0f},		  color,			{0.0f, 1.0f} },
		{ {-0.5f, -0.5f,  0.5f},  {0.0f, 0.0f, 0.0f},		  color,			{0.0f, 0.0f} },
		 					 																 
		{ {-0.5f,  0.5f,  0.5f}, {0.0f, 0.0f, 0.0f},		  color,			{1.0f, 0.0f} },
		{ {-0.5f,  0.5f, -0.5f}, {0.0f, 0.0f, 0.0f},		  color,			{1.0f, 1.0f} },
		{ {-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, 0.0f},		  color,			{0.0f, 1.0f} },
		{ {-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, 0.0f},		  color,			{0.0f, 1.0f} },
		{ {-0.5f, -0.5f,  0.5f}, {0.0f, 0.0f, 0.0f},		  color,			{0.0f, 0.0f} },
		{ {-0.5f,  0.5f,  0.5f}, {0.0f, 0.0f, 0.0f},		  color,			{1.0f, 0.0f} },
		 																	 
		{ { 0.5f,  0.5f,  0.5f},  {0.0f, 0.0f, 0.0f},		  color,			{1.0f, 0.0f} },
		{ { 0.5f,  0.5f, -0.5f},  {0.0f, 0.0f, 0.0f},		  color,			{1.0f, 1.0f} },
		{ { 0.5f, -0.5f, -0.5f},  {0.0f, 0.0f, 0.0f},		  color,			{0.0f, 1.0f} },
		{ { 0.5f, -0.5f, -0.5f},  {0.0f, 0.0f, 0.0f},		  color,			{0.0f, 1.0f} },
		{ { 0.5f, -0.5f,  0.5f},  {0.0f, 0.0f, 0.0f},		  color,			{0.0f, 0.0f} },
		{ { 0.5f,  0.5f,  0.5f},  {0.0f, 0.0f, 0.0f},		  color,			{1.0f, 0.0f} },
		 																					 
		{ {-0.5f, -0.5f, -0.5f},  {0.0f, 0.0f, 0.0f},		  color,			{0.0f, 1.0f} },
		{ { 0.5f, -0.5f, -0.5f},  {0.0f, 0.0f, 0.0f},		  color,			{1.0f, 1.0f} },
		{ { 0.5f, -0.5f,  0.5f},  {0.0f, 0.0f, 0.0f},		  color,			{1.0f, 0.0f} },
		{ { 0.5f, -0.5f,  0.5f},  {0.0f, 0.0f, 0.0f},		  color,			{1.0f, 0.0f} },
		{ {-0.5f, -0.5f,  0.5f},  {0.0f, 0.0f, 0.0f},		  color,			{0.0f, 0.0f} },
		{ {-0.5f, -0.5f, -0.5f},  {0.0f, 0.0f, 0.0f},		  color,			{0.0f, 1.0f} },
		 														 
		{ {-0.5f,  0.5f, -0.5f},  {0.0f, 0.0f, 0.0f},		  color,			{0.0f, 1.0f} },
		{ { 0.5f,  0.5f, -0.5f},  {0.0f, 0.0f, 0.0f},		  color,			{1.0f, 1.0f} },
		{ { 0.5f,  0.5f,  0.5f},  {0.0f, 0.0f, 0.0f},		  color,			{1.0f, 0.0f} },
		{ { 0.5f,  0.5f,  0.5f},  {0.0f, 0.0f, 0.0f},		  color,			{1.0f, 0.0f} },
		{ {-0.5f,  0.5f,  0.5f},  {0.0f, 0.0f, 0.0f},		  color,			{0.0f, 0.0f} },
		{ {-0.5f,  0.5f, -0.5f},  {0.0f, 0.0f, 0.0f},		  color,			{0.0f, 1.0f} }
	};

	for (uint32_t i = 0; i < mesh.vertices.size(); ++i)
		mesh.indices.push_back(i);

	return mesh;
}

Mesh MeshFactory::CreateSphere(const Vec3& color)
{
	Mesh mesh;

	mesh.vertices.clear(); 
	mesh.indices.clear();

	float x, y, z, xy;	// Vertex Position
	float nx, ny, nz;	// Vertex Normal
	float s, t;			// Vertex TexCoord
	float lengthInv = 1.0f / SPHERE_RADIUS;
	float sectorStep = 2 * PI / SECTOR_COUNT;
	float stackStep = PI / STACK_COUNT;
	float sectorAngle, stackAngle;

	for (int i = 0; i <= STACK_COUNT; ++i)
	{
		stackAngle = PI / 2 - i * stackStep;
		xy = SPHERE_RADIUS * COS(stackAngle);
		z = SPHERE_RADIUS * SIN(stackAngle);

		for (int j = 0; j <= SECTOR_COUNT; ++j)
		{
			sectorAngle = j * sectorStep;

			x = xy * COS(sectorAngle);
			y = xy * SIN(sectorAngle);

			nx = x * lengthInv;
			ny = y * lengthInv;
			nz = z * lengthInv;

			s = (float)j / SECTOR_COUNT;
			t = (float)i / STACK_COUNT;

			mesh.vertices.push_back({ {x, y, z}, {nx, ny, nz}, color, {s, t}});
		}
	}

	std::vector<int> lineIndices;
	int k1, k2;
	for (int i = 0; i < STACK_COUNT; ++i)
	{
		k1 = i * (SECTOR_COUNT + 1);
		k2 = k1 + SECTOR_COUNT + 1;

		for (int j = 0; j < SECTOR_COUNT; ++j, ++k1, ++k2)
		{
			if (i != 0)
			{
				mesh.indices.push_back(k1);
				mesh.indices.push_back(k2);
				mesh.indices.push_back(k1 + 1);
			}

			if (i != (STACK_COUNT - 1))
			{
				mesh.indices.push_back(k1 + 1);
				mesh.indices.push_back(k2);
				mesh.indices.push_back(k2 + 1);
			}

			lineIndices.push_back(k1);
			lineIndices.push_back(k2);

			if (i != 0)
			{
				lineIndices.push_back(k1);
				lineIndices.push_back(k1 + 1);
			}
		}
	}

	return mesh;
}