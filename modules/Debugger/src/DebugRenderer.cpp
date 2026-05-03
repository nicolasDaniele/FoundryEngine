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

    void DebugRenderer::AddLine(Vec3 lineStart, Vec3 lineEnd)
    {
        vertices.push_back(lineStart);
        vertices.push_back(lineEnd);
    }

    void DebugRenderer::AddBox(const OBB& box)
    {
        std::vector<CoreGeometry::Line> edges = CoreGeometry::GetEdges(box);

        for(CoreGeometry::Line edge : edges)
        {
            AddLine(edge.start, edge.end);
        }
    }

    void DebugRenderer::AddSphere(const Sphere& sphere)
    {
        for (int axis = 0; axis < 3; axis++)
        {
            for (int i = 0; i < SPHERE_SEGMENTS; i++)
            {
                float a0 = (i / (float)SPHERE_SEGMENTS) * TWO_PI;
                float a1 = ((i + 1) / (float)SPHERE_SEGMENTS) * TWO_PI;

                Vec3 p0 = CirclePoint(sphere.center, a0, sphere.radius, axis);
                Vec3 p1 = CirclePoint(sphere.center, a1, sphere.radius, axis);

                AddLine(p0, p1);
            }
        }
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
    }

    void DebugRenderer::Clear()
    {
	    vertices.clear();
    }
}