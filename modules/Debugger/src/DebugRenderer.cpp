#include <glad/glad.h>
#include "Debugger/DebugRenderer.h"
#include "EngineInterfaces/IGraphics.h"
#include "Core/MathDefinitions.h"

namespace Debugger
{
    // @TODO: Recieve shaders as parameters
    DebugRenderer::DebugRenderer(IGraphics* _graphics)
    {
        graphics = _graphics;
        shaderProgram = graphics->CreateShaderProgram("Assets/Shaders/DebugColor.vs",
             "Assets/Shaders/DebugColor.fs");
    }

    void DebugRenderer::AddLineToBatch(std::vector<Vec3>& batch, Vec3 lineStart, Vec3 lineEnd)
    {
        batch.push_back(lineStart);
        batch.push_back(lineEnd);
    }

    void DebugRenderer::AddBoxToBatch(std::vector<Vec3>& batch, const OBB& box)
    {
        std::vector<CoreGeometry::Line> edges = CoreGeometry::GetEdges(box);

        for (CoreGeometry::Line edge : edges)
            AddLineToBatch(batch, edge.start, edge.end);
    }

    void DebugRenderer::AddSphereToBatch(std::vector<Vec3>& batch, const Sphere& sphere)
    {
        for (int axis = 0; axis < 3; axis++)
        {
            for (int i = 0; i < SPHERE_SEGMENTS; i++)
            {
                float a0 = (i / (float)SPHERE_SEGMENTS) * TWO_PI;
                float a1 = ((i + 1) / (float)SPHERE_SEGMENTS) * TWO_PI;

                Vec3 p0 = CirclePoint(sphere.center, a0, sphere.radius, axis);
                Vec3 p1 = CirclePoint(sphere.center, a1, sphere.radius, axis);

                AddLineToBatch(batch, p0, p1);
            }
        }
    }

    std::vector<Vec3>& DebugRenderer::GetOrCreateColorBatch(const Vec3& color)
    {
        for (ColoredLineBatch& batch : coloredBatches)
        {
            if (batch.color.x == color.x && batch.color.y == color.y && batch.color.z == color.z)
                return batch.vertices;
        }

        coloredBatches.push_back({ color, {} });
        return coloredBatches.back().vertices;
    }

    void DebugRenderer::AddLine(Vec3 lineStart, Vec3 lineEnd)
    {
        AddLineToBatch(vertices, lineStart, lineEnd);
    }

    void DebugRenderer::AddBox(const OBB& box)
    {
        AddBoxToBatch(vertices, box);
    }

    void DebugRenderer::AddSphere(const Sphere& sphere)
    {
        AddSphereToBatch(vertices, sphere);
    }

    void DebugRenderer::AddColoredLine(Vec3 lineStart, Vec3 lineEnd, const Vec3& color)
    {
        AddLineToBatch(GetOrCreateColorBatch(color), lineStart, lineEnd);
    }

    void DebugRenderer::AddColoredBox(const OBB& box, const Vec3& color)
    {
        AddBoxToBatch(GetOrCreateColorBatch(color), box);
    }

    void DebugRenderer::AddColoredSphere(const Sphere& sphere, const Vec3& color)
    {
        AddSphereToBatch(GetOrCreateColorBatch(color), sphere);
    }

    Vec3 DebugRenderer::CirclePoint(const Vec3& center, float angleRad, float radius, int axis)
    {
        switch (axis)
        {
            case 0: // XZ
                return Vec3(
                    center.x + cos(angleRad) * radius,
                    center.y,
                    center.z + sin(angleRad) * radius
                );

            case 1: // XY
                return Vec3(
                    center.x + cos(angleRad) * radius,
                    center.y + sin(angleRad) * radius,
                    center.z
                );

            case 2: // YZ
                return Vec3(
                    center.x,
                    center.y + cos(angleRad) * radius,
                    center.z + sin(angleRad) * radius
                );
        }

        return center;
    }

    void DebugRenderer::DrawDebug(const Vec3& color)
    {
        graphics->DrawDebugLines(
            vertices.data(),
            vertices.size(),
            shaderProgram,
            color
        );

        // One extra draw call per distinct color - fine for debug-only geometry.
        for (const ColoredLineBatch& batch : coloredBatches)
        {
            graphics->DrawDebugLines(
                batch.vertices.data(),
                batch.vertices.size(),
                shaderProgram,
                batch.color
            );
        }
    }

    void DebugRenderer::Clear()
    {
	    vertices.clear();
	    coloredBatches.clear();
    }
}
