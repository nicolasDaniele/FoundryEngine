#pragma once

#include "Core/Vectors.h"
#include "Core/Matrices.h"
#include <vector>

namespace CoreGeometry
{
	using Vec3 = CoreMath::Vec3;
	using Mat3 = CoreMath::Mat3;

	typedef Vec3 Point;

	typedef struct Line
	{
		Point start = Point();
		Point end = Point();

		inline Line() { }
		inline Line(const Point& _start, const Point& _end) :
			start(_start), end(_end) { }
	} Line;

	typedef struct Ray
	{
		Point origin = Point();
		Vec3 direction = Vec3();

		inline Ray() : direction(0.0f, 0.0f, 1.0f) { }
		inline Ray(const Point& _origin, const Vec3& _direction) :
			origin(_origin), direction(_direction)
		{
			NormalizeDirection();
		}
		inline void NormalizeDirection()
		{
			Normalize(direction);
		}
	} Ray;

	typedef struct Sphere
	{
		Point center = Vec3();
		float radius = 0.0f;

		inline Sphere() : radius(1.0f) { }
		inline Sphere(const Point& _center, float _radius) :
			center(_center), radius(_radius) { }
	} Sphere;

	typedef struct AABB
	{
		Point center = Point();
		Vec3 halfExtents = Vec3();

		inline AABB() : halfExtents(1.0f) { }
		inline AABB(const Point& _center, const Vec3& _halfExtents) :
			center(_center), halfExtents(_halfExtents) { }
	} AABB;

	typedef struct OBB
	{
		Point center = Point();
		Vec3 halfExtents = Vec3();
		Mat3 orientation = Mat3();

		inline OBB() : halfExtents(1, 1, 1) { }
		inline OBB(const Point& _center, const Vec3& _halfExtents) :
			center(_center), halfExtents(_halfExtents) { }
		inline OBB(const Point& _center, const Vec3& _halfExtents, const Mat3& _orientation) :
			center(_center), halfExtents(_halfExtents), orientation(_orientation) { }
	} OBB;

	typedef struct Plane
	{
		Vec3 normal = Vec3();
		float distance = 0.0f;

		inline Plane() : normal(1.0f) { }
		inline Plane(const Vec3& _normal, float _distance) :
			normal(_normal), distance(_distance) { }
	} Plane;

	typedef struct Triangle
	{
		union
		{
			struct
			{
				Point a;
				Point b;
				Point c;
			};

			Point points[3];
			float values[9];
		};

		inline Triangle() { }
		inline Triangle(const Point& p1, const Point& p2, const Point& p3) :
			a(p1), b(p2), c(p3) { }
	} Triangle;

	typedef struct Interval
	{
		float min = 0.0f;
		float max = 0.0f;
	} Interval;

	float Lenght(const Line& line);
	float LenghtSq(const Line& line);

	Ray FromPoints(const Point& from, const Point& to);

	Vec3 GetMin(const AABB& aabb);
	Vec3 GetMax(const AABB& aabb);
	AABB FromMinMax(const Vec3& min, const Vec3& max);

	float PlaneEquation(const Point& point, const Plane& plane);

	bool PointInSphere(const Point& point, const Sphere& sphere);
	Point ClosestPoint(const Sphere& sphere, const Point& point);

	bool PointInAABB(const Point& point, const AABB& aabb);
	Point ClosestPoint(const AABB& aabb, const Point& point);

	bool PointInOBB(const Point& point, const OBB& obb);
	Point ClosestPoint(const OBB& obb, const Point& point);

	bool PointOnPlane(const Point& point, const Plane& plane);
	Point ClosestPoint(const Plane& plane, const Point& point);

	bool PointOnLine(const Point& point, const Line& line);
	Point ClosestPoint(const Line& line, const Point& point);

	bool PointOnRay(const Point& point, const Ray& ray);
	Point ClosestPoint(const Ray& ray, const Point& point);

	Interval GetInterval(const AABB& aabb, const Vec3& axis);
	Interval GetInterval(const OBB& obb, const Vec3& axis);

	bool PointInTriangle(const Point& point, const Triangle& triangle);
	Plane FromTriangle(const Triangle& triangle);
	Point ClosestPoint(const Triangle& triangle, const Point& point);
	Interval GetInterval(const Triangle& triangle, const Vec3& axis);

	std::vector<Point> GetVertices(const OBB& obb);
	std::vector<Line> GetEdges(const OBB& obb);
	std::vector<Plane> GetPlanes(const OBB& obb);
}