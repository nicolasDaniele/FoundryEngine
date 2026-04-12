#pragma once

#include <vector>
#include "Vectors.h"
#include "Matrices.h"
#include "Geometry3D.h"

class IGraphics;

namespace Debugger
{
	using Vec3 = CoreMath::Vec3;
	using Mat4 = CoreMath::Mat4;
	using OBB = CoreGeometry::OBB;
	using Sphere = CoreGeometry::Sphere;
	
	const int16_t SPHERE_SEGMENTS = 16;
	const float TWO_PI = 3.141592f * 2;

	class DebugRenderer
	{
	public:
		DebugRenderer(IGraphics* _graphics);

		void AddLine(Vec3 lineStart, Vec3 lineEnd);
		void AddBox(const OBB& box);
		void AddSphere(const Sphere& sphere);
		void DrawDebug(const Vec3& color);
		void Clear();

	private:
		IGraphics* graphics = nullptr;
		uint32_t shaderProgram;
		std::vector<Vec3> vertices;

		Vec3 CirclePoint(const Vec3& center, float angleRad, float radius);
	};
}