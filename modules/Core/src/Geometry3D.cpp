#include "Core/Geometry3D.h"
#include "Core/MathDefinitions.h"

#include <iostream>

namespace CoreGeometry
{

	float Lenght(const Line& line)
	{
		return Magnitude(line.start - line.end);
	}

	float LenghtSq(const Line& line)
	{
		return MagnitudeSq(line.start - line.end);
	}

	Ray FromPoints(const Point& from, const Point& to)
	{
		return Ray(from, Normalized(to - from));
	}

	Vec3 GetMin(const AABB& aabb)
	{
		Vec3 p1 = aabb.center + aabb.halfExtents;
		Vec3 p2 = aabb.center - aabb.halfExtents;

		return Vec3(fminf(p1.x, p2.x),
			fminf(p1.y, p2.y), 
			fminf(p1.z, p2.z));
	}

	Vec3 GetMax(const AABB& aabb)
	{
		Vec3 p1 = aabb.center + aabb.halfExtents;
		Vec3 p2 = aabb.center - aabb.halfExtents;

		return Vec3(fmaxf(p1.x, p2.x),
			fmaxf(p1.y, p2.y),
			fmaxf(p1.z, p2.z));
	}

	AABB FromMinMax(const Vec3& min, const Vec3& max)
	{
		return AABB((min + max) * 0.5f, (min - max) * 0.5f);
	}

	float PlaneEquation(const Point& point, const Plane& plane)
	{
		return Dot(point, plane.normal) - plane.distance;
	}

	bool PointInSphere(const Point& point, const Sphere& sphere)
	{
		float magSq = MagnitudeSq(point - sphere.center);
		float radSq = sphere.radius * sphere.radius;

		return magSq < radSq;
	}

	Point ClosestPoint(const Sphere& sphere, const Point& point)
	{
		Vec3 sphereToPoint = point - sphere.center;
		Normalize(sphereToPoint);
		sphereToPoint = sphereToPoint * sphere.radius;

		return sphereToPoint + sphere.center;
	}

	bool PointInAABB(const Point& point, const AABB& aabb)
	{
		Point min = GetMin(aabb);
		Point max = GetMax(aabb);

		if (point.x < min.x || point.y < min.y || point.z < min.z)
		{
			return false;
		}
	
		if (point.x > max.x || point.y > max.y || point.z > max.z)
		{
			return false;
		}

		return true;
	}

	Point ClosestPoint(const AABB& aabb, const Point& point)
	{
		Point result = point;
		Point min = GetMin(aabb);
		Point max = GetMax(aabb);

		result.x = (result.x < min.x) ? min.x : result.x;
		result.y = (result.y < min.y) ? min.y : result.y;
		result.z = (result.z < min.z) ? min.z : result.z;

		result.x = (result.x > max.x) ? max.x : result.x;
		result.y = (result.y > max.y) ? max.y : result.y;
		result.z = (result.z > max.z) ? max.z : result.z;

		return result;
	}

	bool PointInOBB(const Point& point, const OBB& obb)
	{
		Vec3 dir = point - obb.center;

		for (int i = 0; i < 3; ++i)
		{
			const float* orientation = &obb.orientation.asArray[i * 3];
			Vec3 axis(
				orientation[0],
				orientation[3],
				orientation[6]);

			float distance = Dot(dir, axis);
			if (distance > obb.halfExtents.asArray[i])
			{
				return false;
			}
			if (distance < -obb.halfExtents.asArray[i])
			{
				return false;
			}
		}

		return true;
	}

	Point ClosestPoint(const OBB& obb, const Point& point)
	{
		Point result = obb.center;
		Vec3 dir = point - obb.center;

		for (int i = 0; i < 3; ++i)
		{
			/*const float* orientation = &obb.orientation.asArray[i * 3];
			Vec3 axis(
				orientation[0],
				orientation[1], 
				orientation[2]);*/
			Vec3 axis(obb.orientation.asArray[i * 3],
				obb.orientation.asArray[i * 3 + 1],
				obb.orientation.asArray[i * 3 + 2]);

			float distance = Dot(dir, axis);

			if (distance > obb.halfExtents.asArray[i])
				distance = obb.halfExtents.asArray[i];
			if (distance < -obb.halfExtents.asArray[i])
				distance = -obb.halfExtents.asArray[i];

			result = result + (axis * distance);
		}

		return result;
	}

	bool PointOnPlane(const Point& point, const Plane& plane)
	{
		float dot = Dot(point, plane.normal);
		return CMP(dot - plane.distance, 0.0f);
	}

	Point ClosestPoint(const Plane& plane, const Point& point)
	{
		float dot = Dot(plane.normal, point);
		float distance = dot - plane.distance;
		return point - plane.normal * distance;
	}

	bool PointOnLine(const Point& point, const Line& line)
	{
		Point closest = ClosestPoint(line, point);
		float distanceSq = MagnitudeSq(closest - point);

		return CMP(distanceSq, 0.0f);
	}

	Point ClosestPoint(const Line& line, const Point& point)
	{
		Vec3 lVec = line.end - line.start;
		float t = Dot(point - line.start, lVec) / Dot(lVec, lVec);
		t = fmaxf(t, 0.0f);
		t = fminf(t, 1.0f);

		return line.start + lVec * t;
	}

	bool PointOnRay(const Point& point, const Ray& ray)
	{
		if (point == ray.origin)
		{
			return true;
		}

		Vec3 norm = point - ray.origin;
		Normalize(norm);
		float diff = Dot(norm, ray.direction);

		return CMP(diff, 1.0f);
	}

	Point ClosestPoint(const Ray& ray, const Point& point)
	{
		float t = Dot(point - ray.origin, ray.direction);
		t /= Dot(ray.direction, ray.direction);
		t = fmaxf(t, 0.0f);

		return Point(ray.origin + ray.direction * t);
	}

	Interval GetInterval(const AABB& aabb, const Vec3& axis)
	{
		Vec3 min = GetMin(aabb);
		Vec3 max = GetMax(aabb);

		Vec3 vertex[8] =
		{
			Vec3(min.x, max.y, max.z),
			Vec3(min.x, max.y, min.z),
			Vec3(min.x, min.y, max.z),
			Vec3(min.x, min.y, min.z),
			Vec3(max.x, max.y, max.z),
			Vec3(max.x, max.y, min.z),
			Vec3(max.x, min.y, max.z),
			Vec3(max.x, min.y, min.z)
		};

		Interval result;
		result.min = result.max = Dot(axis, vertex[0]);

		for (int i = 0; i < 8; ++i)
		{
			float projection = Dot(axis, vertex[i]);
			result.min = (projection < result.min) ? projection : result.min;
			result.max = (projection > result.max) ? projection : result.max;
		}

		return result;
	}

	Interval GetInterval(const OBB& obb, const Vec3& axis)
	{
		Vec3 vertex[8];
		Vec3 C = obb.center;
		Vec3 E = obb.halfExtents;

		const float* O = obb.orientation.asArray;

		Vec3 A[] =
		{
			Vec3(O[0], O[1], O[2]),
			Vec3(O[3], O[4], O[5]),
			Vec3(O[6], O[7], O[8])
		};

		vertex[0] = C + A[0] * E[0] + A[1] * E[1] + A[2] * E[2];
		vertex[1] = C - A[0] * E[0] + A[1] * E[1] + A[2] * E[2];
		vertex[2] = C + A[0] * E[0] - A[1] * E[1] + A[2] * E[2];
		vertex[3] = C + A[0] * E[0] + A[1] * E[1] - A[2] * E[2];
		vertex[4] = C - A[0] * E[0] - A[1] * E[1] - A[2] * E[2];
		vertex[5] = C + A[0] * E[0] - A[1] * E[1] - A[2] * E[2];
		vertex[6] = C - A[0] * E[0] + A[1] * E[1] - A[2] * E[2];
		vertex[7] = C - A[0] * E[0] - A[1] * E[1] + A[2] * E[2];

		Interval result;
		result.min = result.max = Dot(axis, vertex[0]);

		for (int i = 0; i < 8; ++i)
		{
			float projection = Dot(axis, vertex[i]);
			result.min = (projection < result.min) ? projection : result.min;
			result.max = (projection > result.max) ? projection : result.max;
		}

		return result;
	}

	bool PointInTriangle(const Point& point, const Triangle& triangle)
	{
		Vec3 a = triangle.a - point;
		Vec3 b = triangle.b - point;
		Vec3 c = triangle.c - point;

		Vec3 normPBC = Cross(b, c);
		Vec3 normPCA = Cross(c, a);
		Vec3 normPAB = Cross(a, b);

		if (Dot(normPCA, normPAB) < 0.0f)
		{
			return false;
		}
		else if (Dot(normPBC, normPAB) < 0.0f)
		{
			return false;
		}

		return true;
	}

	Plane FromTriangle(const Triangle& triangle)
	{
		Plane result;
		result.normal = Normalized(Cross(
			triangle.b - triangle.a, triangle.c- triangle. a));
		result.distance = Dot(result.normal, triangle.a);

		return result;
	}

	Point ClosestPoint(const Triangle& triangle, const Point& point)
	{
		Plane plane = FromTriangle(triangle);
		Point closest = ClosestPoint(plane, point);

		if (PointInTriangle(closest, triangle))
		{
			return closest;
		}

		Point c1 = ClosestPoint(Line(triangle.a, triangle.b), point);
		Point c2 = ClosestPoint(Line(triangle.b, triangle.c), point);
		Point c3 = ClosestPoint(Line(triangle.c, triangle.a), point);

		float magSq1 = MagnitudeSq(point - c1);
		float magSq2 = MagnitudeSq(point - c2);
		float magSq3 = MagnitudeSq(point - c3);

		if (magSq1 < magSq2 && magSq1 < magSq3)
		{
			return c1;
		}
		else if (magSq2 < magSq1 && magSq2 < magSq3)
		{
			return c2;
		}

		return c3;
	}

	Interval GetInterval(const Triangle& triangle, const Vec3& axis)
	{
		Interval result;
		result.min = Dot(axis, triangle.points[0]);
		result.max = result.min;

		for (int i = 0; i < 3; ++i)
		{
			float value = Dot(axis, triangle.points[i]);
			result.min = fminf(result.min, value);
			result.max = fmaxf(result.max, value);
		}

		return result;
	}

	std::vector<Point> GetVertices(const OBB& obb)
	{
		std::vector<Vec3> vertices;
		vertices.resize(8);

		Vec3 center = obb.center;
		Vec3 extents = obb.halfExtents;
		const float* orientation = obb.orientation.asArray;
		Vec3 axes[] =
		{
			Vec3(orientation[0], orientation[1], orientation[2]),
			Vec3(orientation[3], orientation[4], orientation[5]),
			Vec3(orientation[6], orientation[7], orientation[8]),
		};

		vertices[0] = center + axes[0] * extents[0] + axes[1] * extents[1] + axes[2] * extents[2];
		vertices[1] = center - axes[0] * extents[0] + axes[1] * extents[1] + axes[2] * extents[2];
		vertices[2] = center + axes[0] * extents[0] - axes[1] * extents[1] + axes[2] * extents[2];
		vertices[3] = center + axes[0] * extents[0] + axes[1] * extents[1] - axes[2] * extents[2];
		vertices[4] = center - axes[0] * extents[0] - axes[1] * extents[1] - axes[2] * extents[2];
		vertices[5] = center + axes[0] * extents[0] - axes[1] * extents[1] - axes[2] * extents[2];
		vertices[6] = center - axes[0] * extents[0] + axes[1] * extents[1] - axes[2] * extents[2];
		vertices[7] = center - axes[0] * extents[0] - axes[1] * extents[1] + axes[2] * extents[2];

		return vertices;
	}

	std::vector<Line> GetEdges(const OBB& obb)
	{
		std::vector<Line> result;
		result.reserve(12);
		std::vector<Point> vertices = GetVertices(obb);

		int index[][2] = 
		{
			{6,1},{6,3},{6,4},{2,7},{2,5},{2,0},
			{0,1},{0,3},{7,1},{7,4},{4,5},{5,3}
		};

		for (int j = 0; j < 12; ++j)
		{
			result.push_back(Line(vertices[index[j][0]], vertices[index[j][1]]));
		}

		return result;
	}

	std::vector<Plane> GetPlanes(const OBB& obb)
	{
		Vec3 center = obb.center;
		Vec3 extents = obb.halfExtents;
		const float* orientation = obb.orientation.asArray;

		Vec3 axes[] =
		{
			Vec3(orientation[0], orientation[1], orientation[2]),
			Vec3(orientation[3], orientation[4], orientation[5]),
			Vec3(orientation[6], orientation[7], orientation[8]),
		};
	
		std::vector<Plane> result;
		result.resize(6);

		result[0] = Plane(axes[0], Dot(axes[0], (center + axes[0] * extents.x)));
		result[1] = Plane(axes[0] * -1.0f, -Dot(axes[0], (center - axes[0] * extents.x)));
		result[2] = Plane(axes[1], Dot(axes[1], (center + axes[1] * extents.y)));
		result[3] = Plane(axes[1] * -1.0f, -Dot(axes[1], (center - axes[1] * extents.y)));
		result[4] = Plane(axes[2], Dot(axes[2], (center + axes[2] * extents.z)));
		result[5] = Plane(axes[2] * -1.0f, -Dot(axes[2], (center - axes[2] * extents.z)));

		return result;
	}
}