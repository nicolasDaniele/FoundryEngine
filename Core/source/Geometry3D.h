#pragma once

#include "Vectors.h"
#include "Matrices.h"
#include <vector>

namespace CoreGeometry
{
	using Vec3 = CoreMath::Vec3;
	using Mat3 = CoreMath::Mat3;

	typedef Vec3 Point;

	typedef struct Line
	{
		Point start;
		Point end;

		inline Line() { }
		inline Line(const Point& s, const Point& e) :
			start(s), end(e) { }
	} Line;

	typedef struct Ray
	{
		Point origin;
		Vec3 direction;

		inline Ray() : direction(0.0f, 0.0f, 1.0f) { }
		inline Ray(const Point& o, const Vec3& d) :
			origin(o), direction(d)
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
		Point center;
		float radius;

		inline Sphere() : radius(1.0f) { }
		inline Sphere(const Point& p, float r) :
			center(p), radius(r) { }
	} Sphere;

	typedef struct AABB
	{
		Point center;
		Vec3 halfExtents;

		inline AABB() : halfExtents(1, 1, 1) { }
		inline AABB(const Point& p, const Vec3& s) :
			center(p), halfExtents(s) { }
	} AABB;

	typedef struct OBB
	{
		Point center;
		Vec3 halfExtents;
		Mat3 orientation;

		inline OBB() : halfExtents(1, 1, 1) { }
		inline OBB(const Point& p, const Vec3& s) :
			center(p), halfExtents(s) { }
		inline OBB(const Point& p, const Vec3& s, const Mat3& o) :
			center(p), halfExtents(s), orientation(o) { }
	} OBB;

	typedef struct Plane
	{
		Vec3 normal;
		float distance;

		inline Plane() : normal(1, 0, 0) { }
		inline Plane(const Vec3& n, float d) :
			normal(n), distance(d) { }
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
		float min;
		float max;
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