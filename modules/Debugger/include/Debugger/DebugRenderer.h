#pragma once

#include <vector>
#include "Core/Vectors.h"
#include "Core/Matrices.h"
#include "Core/Geometry3D.h"

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
		~DebugRenderer() = default;

		void AddLine(Vec3 lineStart, Vec3 lineEnd);
		void AddBox(const OBB& box);
		void AddSphere(const Sphere& sphere);

		// Same shapes as above, but each one keeps its own color instead of
		// sharing the single color passed to DrawDebug(). Useful for gizmos
		// that carry meaning through color, like a point light's tint.
		void AddColoredLine(Vec3 lineStart, Vec3 lineEnd, const Vec3& color);
		void AddColoredBox(const OBB& box, const Vec3& color);
		void AddColoredSphere(const Sphere& sphere, const Vec3& color);

		void DrawDebug(const Vec3& color);
		void Clear();

	private:
		// One batch of line vertices sharing a single color.
		struct ColoredLineBatch
		{
			Vec3 color;
			std::vector<Vec3> vertices;
		};

		IGraphics* graphics = nullptr;
		uint32_t shaderProgram;

		std::vector<Vec3> vertices; // uniform-color batch, drawn with DrawDebug()'s own color param
		std::vector<ColoredLineBatch> coloredBatches; // per-shape color batches (e.g. light gizmos)

		Vec3 CirclePoint(const Vec3& center, float angleRad, float radius, int axis);

		// Shared implementations, parameterized by target batch, so the
		// uniform-color and colored variants don't duplicate the geometry logic.
		void AddLineToBatch(std::vector<Vec3>& batch, Vec3 lineStart, Vec3 lineEnd);
		void AddBoxToBatch(std::vector<Vec3>& batch, const OBB& box);
		void AddSphereToBatch(std::vector<Vec3>& batch, const Sphere& sphere);

		std::vector<Vec3>& GetOrCreateColorBatch(const Vec3& color);
	};
}
