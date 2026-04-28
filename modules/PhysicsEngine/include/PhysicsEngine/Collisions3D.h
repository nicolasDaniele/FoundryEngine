#pragma once

#include <vector>
#include "Core/Geometry3D.h"
#include "EngineInterfaces/IPhysics.h"

#define AABBShpere(aabb, sphere)    SphereAABB(sphere, aabb)
#define OBBShpere(obb, sphere)      SphereOBB(sphere, obb)
#define PlaneSphere(plane, sphere) SpherePlane(sphere, plane)
#define OBBAABB(obb, aabb) AABBOBB(aabb, obb)
#define PlaneAABB(plane, aabb) AABBPlane(aabb, plane)
#define PlaneOBB(plane, obb) OBBPlane(obb, plane)
#define SphereTriangle(sphere, triangle) TriangleSphere(triangle, sphere)
#define AABBTriangle(aabb, triangle) TriangleAABB(triangle, aabb)
#define OBBTriangle(obb, triangle) TriangleOBB(triangle, obb)
#define PlaneTriangle(plane, triangle) TrianglePlane(triangle, plane)

using Vec3		= CoreMath::Vec3;
using Mat3		= CoreMath::Mat3;
using Point		= CoreGeometry::Point;
using Line		= CoreGeometry::Line;
using Ray		= CoreGeometry::Ray;
using Sphere	= CoreGeometry::Sphere;
using AABB		= CoreGeometry::AABB;
using OBB		= CoreGeometry::OBB;
using Plane		= CoreGeometry::Plane;
using Triangle	= CoreGeometry::Triangle;
using Interval	= CoreGeometry::Interval;

typedef struct RaycastResult
{
	Vec3 point;
	Vec3 normal;
	float t;
	bool hit;
} RaycastResult;

typedef struct CollisionData
{
	bool colliding;
	Vec3 normal;
	float depth;
	std::vector<Vec3> contacts;
} CollisionData;

struct CollisionKey
{
    RigidbodyHandle A;
    RigidbodyHandle B;

    CollisionKey(RigidbodyHandle a, RigidbodyHandle b)
    {
        if (a.index < b.index)
        {
            A = a;
            B = b;
        }
        else
        {
            A = b;
            B = a;
        }
    }

    bool operator==(const CollisionKey& other) const
    {
        return A.index == other.A.index &&
               A.generation == other.A.generation &&
               B.index == other.B.index &&
               B.generation == other.B.generation;
    }
};

struct CollisionKeyHash
{
    size_t operator()(const CollisionKey& key) const
    {
        size_t h1 = std::hash<uint32_t>()(key.A.index);
        size_t h2 = std::hash<uint32_t>()(key.A.generation);
        size_t h3 = std::hash<uint32_t>()(key.B.index);
        size_t h4 = std::hash<uint32_t>()(key.B.generation);

        size_t h = h1;
        h ^= h2 + 0x9e3779b9 + (h << 6) + (h >> 2);
        h ^= h3 + 0x9e3779b9 + (h << 6) + (h >> 2);
        h ^= h4 + 0x9e3779b9 + (h << 6) + (h >> 2);

        return h;
    }
};


bool SphereShpere(const Sphere& sphere1, const Sphere& sphere2);
bool SphereAABB(const Sphere& sphere, const AABB& aabb);
bool SphereOBB(const Sphere& sphere, const OBB& obb);
bool SpherePlane(const Sphere& sphere, const Plane& plane);
bool AABBAABB(const AABB& aabb1, const AABB& aabb2);
bool AABBOBB(const AABB& aabb, const OBB& obb);
bool AABBPlane(const AABB& aabb, const Plane& plane);
bool OBBOBB(const OBB& obb1, const OBB& obb2);
bool OBBPlane(const OBB& obb, const Plane& plane);
bool PlanePlane(const Plane& plane1, const Plane& plane2);

bool OverlapOnAxis(const AABB& aabb, const OBB& obb, const Vec3& axis);
bool OverlapOnAxis(const OBB& obb1, const OBB& obb2, const Vec3& axis);

bool Raycast(const Sphere& sphere, const Ray& ray, RaycastResult* outResult);
bool Raycast(const AABB& aabb, const Ray& ray, RaycastResult* outResult);
bool Raycast(const OBB& obb, const Ray& ray, RaycastResult* outResult);
bool Raycast(const Plane& plane, const Ray& ray, RaycastResult* outResult);
bool Raycast(const Triangle& triangle, const Ray& ray, RaycastResult* outResult);
void ResetRaycastResult(RaycastResult* outResult);

bool Linecast(const Sphere& sphere, const Line& line);
bool Linecast(const AABB& aabb, const Line& line);
bool Linecast(const OBB& obb, const Line& line);
bool Linecast(const Plane& plane, const Line& line);
bool Linecast(const Triangle& triangle, const Line& line);

bool OverlapOnAxis(const AABB& aabb, const Triangle& triangle, const Vec3& axis);
bool OverlapOnAxis(const OBB& obb, const Triangle& triangle, const Vec3& axis);
bool OverlapOnAxis(const Triangle& triangle1, const Triangle& triangle2, const Vec3& axis);
Vec3 SATCrossEdge(const Vec3& a, const Vec3& b, const Vec3& c, const Vec3& d);
Vec3 Barycentric(const Point& point, const Triangle& triangle);
bool TriangleSphere(const Triangle& triangle, const Sphere& sphere);
bool TriangleAABB(const Triangle& triangle, const AABB& aabb);
bool TriangleOBB(const Triangle& triangle, const OBB& obb);
bool TrianglePlane(const Triangle& triangle, const Plane& plane);
bool TriangleTriangle(const Triangle& triangle1, const Triangle& triangle2);
bool TriangleTriangleRobust(const Triangle& triangle1, const Triangle& triangle2);

CollisionData FindCollisionFeatures(const Sphere& sphere1, const Sphere& sphere2);
CollisionData FindCollisionFeatures(const OBB& obb1, const OBB& obb2);
CollisionData FindCollisionFeatures(const OBB& obb, const Sphere& sphere);
void ResetCollisionData(CollisionData* result);

bool ClipToPlane(const Plane& plane, const Line& line, Point* outPoint);
std::vector<Point> ClipEdgesToOBB(const std::vector<Line>& edges, const OBB& obb);
float OBBToOBBPenetrationDepth(const OBB& obb1, const OBB& obb2,
	const Vec3& axis, bool* outShouldFlip);