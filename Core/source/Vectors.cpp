#include "Vectors.h"
#include "MathDefinitions.h"

namespace CoreMath

{
	Vec2 operator+(const Vec2& v1, const Vec2& v2)
	{
		return { v1.x + v2.x, v1.y + v2.y };
	}

	Vec3 operator+(const Vec3& v1, const Vec3& v2)
	{
		return { v1.x + v2.x, v1.y + v2.y, v1.z+ v2.z };
	}

	Vec2 operator-(const Vec2& v1, const Vec2& v2)
	{
		return { v1.x - v2.x, v1.y - v2.y };
	}

	Vec3 operator-(const Vec3& v1, const Vec3& v2)
	{
		return { v1.x - v2.x, v1.y - v2.y, v1.z - v2.z };
	}

	Vec2 operator*(const Vec2& v1, const Vec2& v2)
	{
		return { v1.x * v2.x, v1.y * v2.y };
	}

	Vec3 operator*(const Vec3& v1, const Vec3& v2)
	{
		return { v1.x * v2.x, v1.y * v2.y, v1.z * v2.z };
	}

	Vec2 operator*(const Vec2& v, float f)
	{
		return { v.x * f, v.y * f };
	}

	Vec3 operator*(const Vec3& v, float f)
	{
		return { v.x * f, v.y * f, v.z * f };
	}

	bool operator==(const Vec2& v1, const Vec2& v2)
	{
		return CMP(v1.x, v2.x) && CMP(v1.y, v2.y);
	}

	bool operator==(const Vec3& v1, const Vec3& v2)
	{
		return CMP(v1.x, v2.x) && CMP(v1.y, v2.y) && CMP(v1.z, v2.z);
	}

	bool operator!=(const Vec2& v1, const Vec2& v2)
	{
		return !(v1 == v2);
	}

	bool operator!=(const Vec3& v1, const Vec3& v2)
	{
		return !(v1 == v2);
	}

	float Dot(const Vec2& v1, const Vec2& v2)
	{
		return v1.x * v2.x + v1.y * v2.y;
	}

	float Dot(const Vec3& v1, const Vec3& v2)
	{
		return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
	}

	float Magnitude(const Vec2& v) 
	{
		return SQRT(Dot(v, v));
	}

	float Magnitude(const Vec3& v)
	{
		return SQRT(Dot(v, v));
	}

	float MagnitudeSq(const Vec2& v)
	{
		return Dot(v, v);
	}

	float MagnitudeSq(const Vec3& v)
	{
		return Dot(v, v);
	}

	void Normalize(Vec2& v)
	{
		v = v * (1.0f / Magnitude(v));
	}

	void Normalize(Vec3& v)
	{
		v = v * (1.0f / Magnitude(v));
	}

	Vec2 Normalized(const Vec2& v)
	{
		return v * (1.0f / Magnitude(v));
	}

	Vec3 Normalized(const Vec3& v)
	{
		return v * (1.0f / Magnitude(v));
	}

	Vec3 Cross(const Vec3 v1, const Vec3 v2)
	{
		Vec3 result;

		result.x = v1.y * v2.z - v1.z * v2.y;
		result.y = v1.z * v2.x - v1.x * v2.z;
		result.z = v1.x * v2.y - v1.y * v2.x;

		return result;
	}

	float Angle(const Vec2& v1, const Vec2& v2)
	{
		float m = SQRT(MagnitudeSq(v1) * MagnitudeSq(v2));
		return ACOS(Dot(v1, v2) / m);
	}

	float Angle(const Vec3& v1, const Vec3& v2)
	{
		float m = SQRT(MagnitudeSq(v1) * MagnitudeSq(v2));
		return ACOS(Dot(v1, v2) / m);
	}

	Vec2 Project(const Vec2& length, const Vec2& direction)
	{
		float dot = Dot(length, direction);
		float magSq = MagnitudeSq(direction);
		return direction * (dot / magSq);
	}

	Vec3 Project(const Vec3& length, const Vec3& direction)
	{
		float dot = Dot(length, direction);
		float magSq = MagnitudeSq(direction);
		return direction * (dot / magSq);
	}

	Vec2 Perpendicular(const Vec2& length, const Vec2& direction)
	{
		return length - Project(length, direction);
	}

	Vec3 Perpendicular(const Vec3& length, const Vec3& direction)
	{
		return length - Project(length, direction);
	}

	Vec2 Reflection(const Vec2& vec, const Vec2& normal)
	{
		float dot = Dot(vec, normal);
		return vec - normal * (dot * 2.0f); 
	}

	Vec3 Reflection(const Vec3& vec, const Vec3& normal)
	{
		float dot = Dot(vec, normal);
		return vec - normal * (dot * 2.0f);
	}
}