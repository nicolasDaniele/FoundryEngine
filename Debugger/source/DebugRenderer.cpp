#include <glad/glad.h>
#include "DebugRenderer.h"
#include "GraphicsEngine/IGraphics.h"
#include "MathDefinitions.h"

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
        // @TODO: This is drawing CIRCLE. Make it draw SPHERE
        for (int axis = 0; axis < 3; axis++)
        {
            for (int i = 0; i < SPHERE_SEGMENTS; i++)
            {
                float a0 = (i / (float)SPHERE_SEGMENTS) * TWO_PI;
                float a1 = ((i + 1) / (float)SPHERE_SEGMENTS) * TWO_PI;

                Vec3 p0 = CirclePoint(sphere.center, a0, sphere.radius);
                Vec3 p1 = CirclePoint(sphere.center, a1, sphere.radius);

                AddLine(p0, p1);
            }
        }
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

    Vec3 DebugRenderer::CirclePoint(const Vec3& center, float angleRad, float radius)
    {
        return Vec3(
            center.x + COS(angleRad) * radius,
            center.y,
            center.z + SIN(angleRad) * radius
        );
    }

    // WHERE to call this?
    void DebugRenderer::Clear()
    {
	    vertices.clear();
    }
}