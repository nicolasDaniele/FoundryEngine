#include <iostream>
#include "PhysicsEngine/Collisions3D.h"
#include "Core/Vectors.h"
#include "Core/MathDefinitions.h"

bool SphereShpere(const Sphere& sphere1, const Sphere& sphere2)
{
	float radiiSum = sphere1.radius + sphere2.radius;
	float sqDistance = CoreMath::MagnitudeSq(sphere1.center - sphere2.center);

	return sqDistance < (radiiSum * radiiSum);
}

bool SphereAABB(const Sphere& sphere, const AABB& aabb)
{
	Point closestpoint = ClosestPoint(aabb, sphere.center);
	float sqDistance = CoreMath::MagnitudeSq(sphere.center - closestpoint);
	float sqRadius = sphere.radius * sphere.radius;

	return sqDistance < sqRadius;
}

bool SphereOBB(const Sphere& sphere, const OBB& obb)
{
	Point closestPoint = ClosestPoint(obb, sphere.center);
	float sqDistance = CoreMath::MagnitudeSq(sphere.center - closestPoint);
	float sqRadius = sphere.radius * sphere.radius;

	return sqDistance < sqRadius;
}

bool SpherePlane(const Sphere& sphere, const Plane& plane)
{
	Point closestPoint = ClosestPoint(plane, sphere.center);
	float sqDistance = CoreMath::MagnitudeSq(sphere.center - closestPoint);
	float sqRadius = sphere.radius * sphere.radius;

	return sqDistance < sqRadius;
}

bool AABBAABB(const AABB& aabb1, const AABB& aabb2)
{
	Point aMin = GetMin(aabb1);
	Point aMax = GetMax(aabb1);

	Point bMin = GetMin(aabb2);
	Point bMax = GetMax(aabb2);

	return (aMin.x <= bMax.x && aMax.x >= bMin.x) &&
		(aMin.y <= bMax.y && aMax.y >= bMin.y) &&
		(aMin.z <= bMax.z && aMax.z >= bMin.z);
}

bool AABBOBB(const AABB& aabb, const OBB& obb)
{
	const float* orientation = obb.orientation.asArray;

	Vec3 test[15] =
	{
		Vec3(1, 0, 0),
		Vec3(0, 1, 0),
		Vec3(0, 0, 1),
		Vec3(orientation[0], orientation[1], orientation[2]),
		Vec3(orientation[3], orientation[4], orientation[5]),
		Vec3(orientation[6], orientation[7], orientation[8])
	};

	for (int i = 0; i < 3; ++i)
	{
		test[6 + i * 3 + 0] = Cross(test[i], test[0]);
		test[6 + i * 3 + 1] = Cross(test[i], test[1]);
		test[6 + i * 3 + 2] = Cross(test[i], test[2]);
	}

	for (int i = 0; i < 15; ++i)
	{
		if (!OverlapOnAxis(aabb, obb, test[i]))
		{
			return false;
		}
	}

	return true;
}

bool AABBPlane(const AABB& aabb, const Plane& plane)
{
	float pLen = aabb.halfExtents.x * fabsf(plane.normal.x) +
		aabb.halfExtents.y * fabsf(plane.normal.y) +
		aabb.halfExtents.z * fabsf(plane.normal.z);

	float dot = CoreMath::Dot(plane.normal, aabb.center);
	float distance = dot - plane.distance;

	return fabsf(distance) <= pLen;
}

bool OBBOBB(const OBB& obb1, const OBB& obb2)
{
	const float* orientation1 = obb1.orientation.asArray;
	const float* orientation2 = obb2.orientation.asArray;

	Vec3 test[15] =
	{
		Vec3(orientation1[0], orientation1[1], orientation1[2]),
		Vec3(orientation1[3], orientation1[4], orientation1[5]),
		Vec3(orientation1[6], orientation1[7], orientation1[8]),
		Vec3(orientation2[0], orientation2[1], orientation2[2]),
		Vec3(orientation2[3], orientation2[4], orientation2[5]),
		Vec3(orientation2[6], orientation2[7], orientation2[8])
	};

	for (int i = 0; i < 3; ++i)
	{
		test[6 + i * 3 + 0] = Cross(test[i], test[0]);
		test[6 + i * 3 + 1] = Cross(test[i], test[1]);
		test[6 + i * 3 + 2] = Cross(test[i], test[2]);
	}

	for (int i = 0; i < 15; ++i)
	{
		if (!OverlapOnAxis(obb1, obb2, test[i]))
		{
			return false;
		}
	}

	return true;
}

bool OBBPlane(const OBB& obb, const Plane& plane)
{
	const float* orientation = obb.orientation.asArray;
	Vec3 rotation[] =
	{
		Vec3(orientation[0], orientation[1], orientation[2]),
		Vec3(orientation[3], orientation[4], orientation[5]),
		Vec3(orientation[6], orientation[7], orientation[8])
	};

	Vec3 normal = plane.normal;
	float pLen = obb.halfExtents.x * fabsf(Dot(normal, rotation[0])) +
		obb.halfExtents.y * fabsf(Dot(normal, rotation[1])) +
		obb.halfExtents.z * fabsf(Dot(normal, rotation[2]));

	float dot = CoreMath::Dot(plane.normal, obb.center);
	float distance = dot - plane.distance;

	return fabsf(distance) <= pLen;
}

bool PlanePlane(const Plane& plane1, const Plane& plane2)
{
	Vec3 d = CoreMath::Cross(plane1.normal, plane2.normal);
	return CMP(Dot(d, d), 0);
}

bool OverlapOnAxis(const AABB& aabb, const OBB& obb, const Vec3& axis)
{
	Interval a = GetInterval(aabb, axis);
	Interval b = GetInterval(obb, axis);

	return ((b.min <= a.max) && (a.min <= b.max));
}

bool OverlapOnAxis(const OBB& obb1, const OBB& obb2, const Vec3& axis)
{
	Interval a = GetInterval(obb1, axis);
	Interval b = GetInterval(obb2, axis);

	return ((b.min <= a.max) && (a.min <= b.max));
}

bool Raycast(const Sphere& sphere, const Ray& ray, RaycastResult* outResult)
{
	ResetRaycastResult(outResult);

	Vec3 e = sphere.center - ray.origin;
	float rSq = sphere.radius * sphere.radius;
	float eSq = MagnitudeSq(e);
	float a = Dot(e, ray.direction);

	float bSq = eSq - (a * a);
	float f = sqrt(rSq - bSq);
	float t = a - f;

	if (rSq - (eSq - (a * a)) < 0.0f)
	{
		return false;
	}
	else if (eSq < rSq)
	{
		t = a + f;
	}

	if (outResult != 0)
	{
		outResult->t = t;
		outResult->hit = true;
		outResult->point = ray.origin + ray.direction * t;
		outResult->normal = CoreMath::Normalized(outResult->point - sphere.center);
	}

	return true;
}

bool Raycast(const AABB& aabb, const Ray& ray, RaycastResult* outResult)
{
	ResetRaycastResult(outResult);

	Vec3 min = GetMin(aabb);
	Vec3 max = GetMax(aabb);

	float t[] = { 0, 0, 0, 0, 0, 0 };

	if (!CMP(ray.direction.x, 0)) t[0] = (min.x - ray.origin.x) / ray.direction.x;
	if (!CMP(ray.direction.x, 0)) t[1] = (max.x - ray.origin.x) / ray.direction.x;
	if (!CMP(ray.direction.y, 0)) t[2] = (min.y - ray.origin.y) / ray.direction.y;
	if (!CMP(ray.direction.y, 0)) t[3] = (max.y - ray.origin.y) / ray.direction.y;
	if (!CMP(ray.direction.z, 0)) t[4] = (min.z - ray.origin.z) / ray.direction.z;
	if (!CMP(ray.direction.z, 0)) t[5] = (max.z - ray.origin.z) / ray.direction.z;

	float tmin = fmaxf(
		fmaxf(fminf(t[0], t[1]), fminf(t[2], t[3])),
		fminf(t[4], t[5]));

	float tmax = fminf(
		fminf(fmaxf(t[0], t[1]), fmaxf(t[2], t[3])),
		fmaxf(t[4], t[5]));

	if (tmax < 0) return false;
	if (tmin > tmax) return false;

	float t_result = tmin;
	if (tmin < 0) t_result = tmax;

	if (outResult != 0)
	{
		outResult->t = t_result;
		outResult->hit = true;
		outResult->point = ray.origin + ray.direction * t_result;

		Vec3 normals[] =
		{
			Vec3(-1, 0, 0), Vec3(1, 0, 0),
			Vec3(0, -1, 0), Vec3(0, 1, 0),
			Vec3(0, 0, -1), Vec3(0, 0, 1)
		};

		for (int i = 0; i < 6; ++i)
		{
			if (CMP(t_result, t[i]))
			{
				outResult->normal = normals[i];
			}
		}
	}

	return true;
}

bool Raycast(const OBB& obb, const Ray& ray, RaycastResult* outResult)
{
	ResetRaycastResult(outResult);

	const float* orientation = obb.orientation.asArray;
	const float* size = obb.halfExtents.asArray;
	Vec3 p = obb.center - ray.origin;

	Vec3 xAxis(orientation[0], orientation[1], orientation[2]);
	Vec3 yAxis(orientation[3], orientation[4], orientation[5]);
	Vec3 zAxis(orientation[6], orientation[7], orientation[8]);

	Vec3 f(
		Dot(xAxis, ray.direction),
		Dot(yAxis, ray.direction),
		Dot(zAxis, ray.direction)
	);

	Vec3 e(
		Dot(xAxis, p),
		Dot(yAxis, p),
		Dot(zAxis, p)
	);

	float t[6] = { 0, 0, 0, 0, 0, 0 };
	for (int i = 0; i < 3; ++i)
	{
		if (CMP(f[i], 0))
		{
			if (-e[i] - size[i] > 0 || -e[i] + size[i] < 0)
			{
				return false;
			}

			f[i] = 0.00001f;
		}

		t[i * 2 + 0] = (e[i] + size[i]) / f[i];
		t[i * 2 + 1] = (e[i] - size[i]) / f[i];
	}

	float tmin = fmaxf(
		fmaxf(fminf(t[0], t[1]), fminf(t[2], t[3])),
		fminf(t[4], t[5]));

	float tmax = fminf(
		fminf(fmaxf(t[0], t[1]), fmaxf(t[2], t[3])),
		fmaxf(t[4], t[5]));

	if (tmax < 0) return false;
	if (tmin > tmax) return false;

	float t_result = tmin;
	if (tmin < 0.0f) return tmax;

	if (outResult != 0)
	{
		outResult->hit = true;
		outResult->t = t_result;
		outResult->point = ray.origin + ray.direction * t_result;

		Vec3 normals[] =
		{
			xAxis, xAxis * -1.0f,
			yAxis, yAxis * -1.0f,
			zAxis, zAxis * -1.0f
		};

		for (int i = 0; i < 6; ++i)
		{
			if (CMP(t_result, t[i]))
			{
				outResult->normal = Normalized(normals[i]);
			}
		}
	}

	return true;
}

bool Raycast(const Plane& plane, const Ray& ray, RaycastResult* outResult)
{
	ResetRaycastResult(outResult);

	float nd = CoreMath::Dot(ray.direction, plane.normal);
	float pn = CoreMath::Dot(ray.origin, plane.normal);

	if (nd >= 0.0f)
	{
		return false;
	}

	float t = (plane.distance - pn) / nd;
	if (t >= 0.0f)
	{
		if (outResult != 0)
		{
			outResult->t = t;
			outResult->hit = true;
			outResult->point = ray.origin + ray.direction * t;
			outResult->normal = CoreMath::Normalized(plane.normal);
		}

		return true;
	}

	return false;
}

bool Raycast(const Triangle& triangle, const Ray& ray, RaycastResult* outResult)
{
	ResetRaycastResult(outResult);

	Plane plane = FromTriangle(triangle);
	RaycastResult planeResult;

	if (!Raycast(plane, ray, &planeResult))
	{
		return false;
	}

	float t = planeResult.t;
	Point resultPoint = ray.origin + ray.direction * t;
	Vec3 barycentric = Barycentric(resultPoint, triangle);

	if (barycentric.x >= 0.0f && barycentric.x <= 1.0f &&
		barycentric.y >= 0.0f && barycentric.y <= 1.0f &&
		barycentric.z >= 0.0f && barycentric.z <= 1.0f)
	{
		if (outResult != 0)
		{
			outResult->t = t;
			outResult->hit = true;
			outResult->point = ray.origin + ray.direction * t;
			outResult->normal = plane.normal;
		}

		return true;
	}

	return false;
}

void ResetRaycastResult(RaycastResult* outResult)
{
	if (outResult != 0)
	{
		outResult->t = -1;
		outResult->hit = false;
		outResult->normal = Vec3(0, 0, 1);
		outResult->point = Vec3(0, 0, 0);
	}
}

bool Linecast(const Sphere& sphere, const Line& line)
{
	Point closest = ClosestPoint(line, sphere.center);
	float distSq = CoreMath::MagnitudeSq(sphere.center - closest);

	return distSq <= (sphere.radius * sphere.radius);
}

bool Linecast(const AABB& aabb, const Line& line)
{
	Ray ray;
	ray.origin = line.start;
	ray.direction = CoreMath::Normalized(line.end - line.start);

	RaycastResult result;
	if (!Raycast(aabb, ray, &result))
	{
		return false;
	}

	float t = result.t;

	return t >= 0 && t * t <= LenghtSq(line);
}

bool Linecast(const OBB& obb, const Line& line)
{
	Ray ray;
	ray.origin = line.start;
	ray.direction = CoreMath::Normalized(line.end - line.start);

	RaycastResult result;
	if (!Raycast(obb, ray, &result))
	{
		return false;
	}

	float t = result.t;

	return t >= 0 && t * t <= LenghtSq(line);
}

bool Linecast(const Plane& plane, const Line& line)
{
	Vec3 ab = line.end - line.start;

	float nA = CoreMath::Dot(plane.normal, line.start);
	float nAB = Dot(plane.normal, ab);

	if (nAB == 0.0f)
	{
		nAB = 0.0001f;
	}
	float t = (plane.distance - nA) / nAB;

	return t >= 0.0f && t <= 1.0f;
}

bool Linecast(const Triangle& triangle, const Line& line)
{
	Ray ray;
	ray.origin = line.start;
	ray.origin = CoreMath::Normalized(line.end - line.start);
	RaycastResult result;

	if (!Raycast(triangle, ray, &result))
	{
		return false;
	}

	float t = result.t;

	return t >= 0.0f && t * t <= LenghtSq(line);
}

bool OverlapOnAxis(const AABB& aabb, const Triangle& triangle, const Vec3& axis)
{
	Interval a = GetInterval(aabb, axis);
	Interval b = GetInterval(triangle, axis);

	return ((b.min <= a.max) && (a.min <= b.max));
}

bool OverlapOnAxis(const Triangle& triangle1, const Triangle& triangle2, const Vec3& axis)
{
	Interval a = GetInterval(triangle1, axis);
	Interval b = GetInterval(triangle2, axis);

	return ((b.min <= a.max) && (a.min <= b.max));
}

Vec3 SATCrossEdge(const Vec3& a, const Vec3& b, const Vec3& c, const Vec3& d)
{
	Vec3 ab = a - b;
	Vec3 cd = c - d;
	Vec3 result = Cross(ab, cd);

	if (!CMP(MagnitudeSq(result), 0))
	{
		return result;
	}
	else
	{
		Vec3 axis = Cross(ab, c - a);
		result = Cross(ab, axis);

		if (!CMP(MagnitudeSq(result), 0))
		{
			return result;
		}
	}

	return Vec3();
}

Vec3 Barycentric(const Point& point, const Triangle& triangle)
{
	Vec3 ap = point - triangle.a;
	Vec3 bp = point - triangle.b;
	Vec3 cp = point - triangle.c;

	Vec3 ab = triangle.b - triangle.a;
	Vec3 ac = triangle.c - triangle.a;

	Vec3 bc = triangle.c - triangle.b;
	Vec3 cb = triangle.b - triangle.c;
	Vec3 ca = triangle.a - triangle.c;

	Vec3 v = ab - Project(ab, cb);
	float a = 1.0f - (Dot(v, ap) / Dot(v, ab));

	v = bc - Project(bc, ac);
	float b = 1.0f - (Dot(v, bp) / Dot(v, bc));

	v = ca - Project(ca, ab);
	float c = 1.0f - (Dot(v, cp) / Dot(v, ca));

	return Vec3(a, b, c);
}

bool TriangleSphere(const Triangle& triangle, const Sphere& sphere)
{
	Point closest = ClosestPoint(triangle, sphere.center);
	float magSq = CoreMath::MagnitudeSq(closest - sphere.center);

	return magSq <= (sphere.radius * sphere.radius);
}

bool TriangleAABB(const Triangle& triangle, const AABB& aabb)
{
	Vec3 f0 = triangle.b - triangle.a;
	Vec3 f1 = triangle.c - triangle.b;
	Vec3 f2 = triangle.a - triangle.c;

	Vec3 u0(1.0f, 0.0f, 0.0f);
	Vec3 u1(0.0f, 1.0f, 0.0f);
	Vec3 u2(0.0f, 0.0f, 1.0f);

	Vec3 test[13] =
	{
		u0, u1, u2,
		Cross(f0, f1),
		Cross(u0, f0), Cross(u0, f1), Cross(u0, f2),
		Cross(u1, f0), Cross(u1, f1), Cross(u1, f2),
		Cross(u2, f0), Cross(u2, f1), Cross(u2, f2)
	};

	for (int i = 0; i < 13; ++i)
	{
		if (!OverlapOnAxis(aabb, triangle, test[i]))
		{
			return false;
		}
	}

	return true;
}

bool TriangleOBB(const Triangle& triangle, const OBB& obb)
{
	Vec3 f0 = triangle.b - triangle.a;
	Vec3 f1 = triangle.c - triangle.b;
	Vec3 f2 = triangle.a - triangle.c;

	const float* orientation = obb.orientation.asArray;
	Vec3 u0(orientation[0], orientation[1], orientation[2]);
	Vec3 u1(orientation[3], orientation[4], orientation[5]);
	Vec3 u2(orientation[6], orientation[7], orientation[8]);

	Vec3 test[13] =
	{
		u0, u1, u2,
		Cross(f0, f1),
		Cross(u0, f0), Cross(u0, f1), Cross(u0, f2),
		Cross(u1, f0), Cross(u1, f1), Cross(u1, f2),
		Cross(u2, f0), Cross(u2, f1), Cross(u2, f2)
	};

	for (int i = 0; i < 13; ++i)
	{
		if (!OverlapOnAxis(obb, triangle, test[i]))
		{
			return false;
		}
	}

	return true;
}

bool OverlapOnAxis(const OBB& obb, const Triangle& triangle, const Vec3& axis)
{
	Vec3 f0 = triangle.b - triangle.a;
	Vec3 f1 = triangle.c - triangle.b;
	Vec3 f2 = triangle.a - triangle.c;

	const float* orientation = obb.orientation.asArray;
	Vec3 u0(orientation[0], orientation[1], orientation[2]);
	Vec3 u1(orientation[3], orientation[4], orientation[5]);
	Vec3 u2(orientation[6], orientation[7], orientation[8]);

	Vec3 test[13] =
	{
		u0, u1, u2,
		Cross(f0, f1),
		Cross(u0, f0), Cross(u0, f1), Cross(u0, f2),
		Cross(u1, f0), Cross(u1, f1), Cross(u1, f2),
		Cross(u2, f0), Cross(u2, f1), Cross(u2, f2)
	};

	for (int i = 0; i < 13; ++i)
	{
		if (!OverlapOnAxis(obb, triangle, test[i]))
		{
			return false;
		}
	}

	return true;
}

bool TrianglePlane(const Triangle& triangle, const Plane& plane)
{
	float side1 = PlaneEquation(triangle.a, plane);
	float side2 = PlaneEquation(triangle.c, plane);
	float side3 = PlaneEquation(triangle.a, plane);

	if (CMP(side1, 0) && CMP(side2, 0) && CMP(side3, 0))
	{
		return true;
	}

	if (side1 > 0 && side2 > 0 && side3 > 0)
	{
		return false;
	}

	if (side1 < 0 && side2 < 0 && side3 < 0)
	{
		return false;
	}

	return true;
}

bool TriangleTriangle(const Triangle& triangle1, const Triangle& triangle2)
{
	Vec3 t1_f0 = triangle1.b - triangle1.a;
	Vec3 t1_f1 = triangle1.c - triangle1.b;
	Vec3 t1_f2 = triangle1.a - triangle1.c;

	Vec3 t2_f0 = triangle2.b - triangle2.a;
	Vec3 t2_f1 = triangle2.c - triangle2.b;
	Vec3 t2_f2 = triangle2.a - triangle2.c;

	Vec3 axisToTest[] =
	{
		Cross(t1_f0,t1_f1),
		Cross(t2_f0,t2_f1),
		Cross(t2_f0,t1_f0), Cross(t2_f0,t1_f1), Cross(t2_f0,t1_f2),
		Cross(t2_f1,t1_f0), Cross(t2_f1,t1_f1), Cross(t2_f1,t1_f2),
		Cross(t2_f2,t1_f0), Cross(t2_f2,t1_f1), Cross(t2_f2,t1_f2)
	};

	for (int i = 0; i < 11; ++i)
	{
		if (!OverlapOnAxis(triangle1, triangle2, axisToTest[i]))
		{
			return false;
		}
	}

	return true;
}

bool TriangleTriangleRobust(const Triangle& triangle1, const Triangle& triangle2)
{
	Vec3 axisToTest[] =
	{
		SATCrossEdge(triangle1.a, triangle1.b, triangle1.b, triangle1.c),
		SATCrossEdge(triangle2.a, triangle2.b, triangle2.b, triangle2.c),

		SATCrossEdge(triangle2.a, triangle2.b, triangle1.a, triangle1.b),
		SATCrossEdge(triangle2.a, triangle2.b, triangle1.b, triangle1.c),
		SATCrossEdge(triangle2.a, triangle2.b, triangle1.c, triangle1.a),

		SATCrossEdge(triangle2.b, triangle2.c, triangle1.a, triangle1.b),
		SATCrossEdge(triangle2.b, triangle2.c, triangle1.b, triangle1.c),
		SATCrossEdge(triangle2.b, triangle2.c, triangle1.c, triangle1.a),

		SATCrossEdge(triangle2.c, triangle2.a, triangle1.a, triangle1.b),
		SATCrossEdge(triangle2.c, triangle2.a, triangle1.b, triangle1.c),
		SATCrossEdge(triangle2.c, triangle2.a, triangle1.c, triangle1.a)
	};

	for (int i = 0; i < 11; ++i)
	{
		if (!OverlapOnAxis(triangle1, triangle2, axisToTest[i]))
		{
			if (!CMP(MagnitudeSq(axisToTest[i]), 0))
			{
				return false;
			}
		}
	}

	return true;
}

Point Intersection(Plane plane1, Plane plane2, Plane plane3)
{
	Mat3 D(
		plane1.normal.x, plane2.normal.x, plane3.normal.x,
		plane1.normal.y, plane2.normal.y, plane3.normal.y,
		plane1.normal.z, plane2.normal.z, plane3.normal.z
	);

	Vec3 A(-plane1.distance, -plane2.distance, -plane3.distance);

	Mat3 Dx = D;
	Mat3 Dy = D;
	Mat3 Dz = D;
	Dx._11 = A.x; Dx._12 = A.y; Dx._13 = A.z;
	Dy._21 = A.x; Dx._22 = A.y; Dx._23 = A.z;
	Dz._31 = A.x; Dx._32 = A.y; Dx._33 = A.z;

	float detD = CoreMath::Determinant(D);
	if (CMP(detD, 0))
	{
		return Point();
	}

	float detDx = CoreMath::Determinant(Dx);
	float detDy = CoreMath::Determinant(Dy);
	float detDz = CoreMath::Determinant(Dz);

	return Point(detDx / detD, detDy / detD, detDz / detD);
}

float Classify(const AABB& aabb, const Plane& plane)
{
	float r = fabsf(aabb.halfExtents.x * plane.normal.x) +
		fabsf(aabb.halfExtents.y * plane.normal.y) +
		fabsf(aabb.halfExtents.z * plane.normal.z);
	float d = CoreMath::Dot(plane.normal, aabb.center) + plane.distance;

	if (fabsf(d) < r)
	{
		return 0.0f;
	}
	else if (d < 0.0f)
	{
		return d + r;
	}

	return d - r;
}

float Classify(const OBB& obb, const Plane& plane)
{
	Vec3 normal = CoreMath::MultiplyMat3Vec3(obb.orientation, plane.normal);
	float r = fabs(obb.halfExtents.x * plane.normal.x) +
		fabs(obb.halfExtents.y * plane.normal.y) +
		fabs(obb.halfExtents.z * plane.normal.z);
	float d = CoreMath::Dot(plane.normal, obb.center) + plane.distance;

	if (fabsf(d) < r)
	{
		return 0.0f;
	}
	else if (d < 0.0f)
	{
		return d + r;
	}

	return d - r;
}

CollisionData FindCollisionFeatures(const Sphere& A, const Sphere& B)
{
	CollisionData result;
	ResetCollisionData(&result);

	float radii = A.radius + B.radius;
	Vec3 distance = B.center - A.center;

	if (MagnitudeSq(distance) - radii * radii > 0 || MagnitudeSq(distance) == 0.0f)
	{
		return result;
	}

	result.colliding = true;
	result.normal = CoreMath::Normalized(distance);
	result.depth = fabsf(Magnitude(distance) - radii) * 0.5f;

	float interceptionDistance = A.radius - result.depth * 0.5f;
	Point contact = A.center + result.normal * interceptionDistance;
	result.contacts.push_back(contact);

	return result;
}

CollisionData FindCollisionFeatures(const OBB& obb1, const OBB& obb2)
{
	CollisionData result;
	ResetCollisionData(&result);

	const float* o1 = obb1.orientation.asArray;
	const float* o2 = obb2.orientation.asArray;

	Vec3 test[15] =
	{
		Vec3(o1[0], o1[1], o1[2]),
		Vec3(o1[3], o1[4], o1[5]),
		Vec3(o1[6], o1[7], o1[8]),
		Vec3(o2[0], o2[1], o2[2]),
		Vec3(o2[3], o2[4], o2[5]),
		Vec3(o2[6], o2[7], o2[8])
	};
	for (int i = 0; i < 3; ++i)
	{
		test[6 + i * 3 + 0] = Cross(test[i], test[0]);
		test[6 + i * 3 + 1] = Cross(test[i], test[1]);
		test[6 + i * 3 + 2] = Cross(test[i], test[2]);
	}

	Vec3* hitNormal = 0;
	bool shouldFlip;

	for (int i = 0; i < 15; ++i)
	{
		if (MagnitudeSq(test[i]) < 0.001f)
		{
			continue;
		}

		float depth = OBBToOBBPenetrationDepth(obb1, obb2, test[i], &shouldFlip);
		if (depth <= 0.0f)
		{
			return result;
		}
		else if (depth < result.depth)
		{
			if (shouldFlip)
			{
				test[i] = test[i] * -1.0f;
			}

			result.depth = depth;
			hitNormal = &test[i];
		}
	}

	if (hitNormal == 0)
	{
		return result;
	}

	Vec3 axis = Normalized(*hitNormal);

	std::vector<Point> c1 = ClipEdgesToOBB(CoreGeometry::GetEdges(obb2), obb1);
	std::vector<Point> c2 = ClipEdgesToOBB(CoreGeometry::GetEdges(obb1), obb2);
	result.contacts.reserve(c1.size() + c2.size());
	result.contacts.insert(result.contacts.end(), c1.begin(), c1.end());
	result.contacts.insert(result.contacts.end(), c2.begin(), c2.end());

	if (result.contacts.empty())
	{
		Interval int1 = GetInterval(obb1, axis);
		Interval int2 = GetInterval(obb2, axis);

		float penetration = std::min(int1.max - int2.min, int2.max - int1.min);
		float contactDist = penetration * 0.5f;

		Point contact = obb1.center + axis * ((int1.max + int1.min) * 0.5f - contactDist);

		result.contacts.push_back(contact);
	}

	Interval i = CoreGeometry::GetInterval(obb1, axis);
	float distance = (i.max - i.min) * 0.5f - result.depth * 0.5f;
	Vec3 pointOnPlane = obb1.center + axis * distance;

	for (int i = result.contacts.size() - 1; i >= 0; --i)
	{
		Vec3 contact = result.contacts[i];
		result.contacts[i] = contact + (axis * Dot(axis, pointOnPlane - contact));

		for (int j = result.contacts.size() - 1; j > i; --j)
		{
			if (CoreMath::MagnitudeSq(result.contacts[j] - result.contacts[i]) < 0.0001f)
			{
				result.contacts.erase(result.contacts.begin() + j);
				break;
			}
		}

		result.colliding = true;
		result.normal = axis;
	}

	return result;
}

CollisionData FindCollisionFeatures(const OBB& obb, const Sphere& sphere)
{
	CollisionData result;
	ResetCollisionData(&result);

	Point closestPoint = CoreGeometry::ClosestPoint(obb, sphere.center);
	float distanceSq = CoreMath::MagnitudeSq(closestPoint - sphere.center);
	if (distanceSq > sphere.radius * sphere.radius)
	{
		return result;
	}

	Vec3 normal;
	float penetration = 0.0f;
	Point contactPoint = closestPoint;

	if (CMP(distanceSq, 0.0f))
	{
		float minPenetration = FLT_MAX;
		Vec3 dir = sphere.center - obb.center;

		for (int i = 0; i < 3; ++i) 
		{
			Vec3 axis(obb.orientation.asArray[i * 3],
				obb.orientation.asArray[i * 3 + 1],
				obb.orientation.asArray[i * 3 + 2]);

			float hExtent = obb.halfExtents.asArray[i];
			float projection = Dot(dir, axis);

			float d = hExtent - fabsf(projection);

			if (d < minPenetration)
			{
				minPenetration = d;
				normal = axis * (projection >= 0 ? 1.0f : -1.0f);
			}
				
		}

		penetration = sphere.radius + minPenetration;
		contactPoint = sphere.center - normal * sphere.radius;
	}
	else
	{
		float distance = sqrtf(distanceSq);
		normal = CoreMath::Normalized(sphere.center - closestPoint);
		penetration = sphere.radius - distance;
	}

	result.colliding = true;
	result.normal = normal;
	result.depth = penetration;
	result.contacts.push_back(contactPoint);

	return result;
}

void ResetCollisionData(CollisionData* result)
{
	if (result != 0) {
		result->colliding = false;
		result->normal = Vec3(0, 0, 1);
		result->depth = FLT_MAX;
		result->contacts.clear();
	}
}

bool ClipToPlane(const Plane& plane, const Line& line, Point* outPoint)
{
	Vec3 ab = line.end - line.start;
	float nAB = Dot(plane.normal, ab);
	if (CMP(nAB, 0))
	{
		return false;
	}

	float nA = CoreMath::Dot(plane.normal, line.start);
	float t = (plane.distance - nA) / nAB;

	if (t >= 0.0f && t <= 1.0f)
	{
		if (outPoint != 0)
		{
			*outPoint = line.start + ab * t;
		}

		return true;
	}

	return false;
}

std::vector<Point> ClipEdgesToOBB(const std::vector<Line>& edges, const OBB& obb)
{
	std::vector<Point> result;
	result.reserve(edges.size());
	Point intersection;

	const std::vector<Plane>& planes = GetPlanes(obb);

	for (int i = 0; i < planes.size(); ++i)
	{
		for (int j = 0; j < edges.size(); ++j)
		{
			if (ClipToPlane(planes[i], edges[j], &intersection))
			{
				if (PointInOBB(intersection, obb))
				{
					result.push_back(intersection);
				}
			}
		}
	}

	return result;
}

float OBBToOBBPenetrationDepth(const OBB& obb1, const OBB& obb2,
	const Vec3& axis, bool* outShouldFlip)
{
	Interval i1 = GetInterval(obb1, Normalized(axis));
	Interval i2 = GetInterval(obb2, Normalized(axis));

	if (!((i2.min <= i1.max) && (i1.min <= i2.max)))
	{
		return 0.0f;
	}

	float len1 = i1.max - i1.min;
	float len2 = i2.max - i2.min;

	float min = fminf(i1.min, i2.min);
	float max = fmaxf(i1.max, i2.max);

	float length = max - min;

	if (outShouldFlip != 0)
	{
		*outShouldFlip = (i2.min < i1.min);
	}

	return (len1 + len2) - length;
}