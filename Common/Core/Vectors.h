#pragma once

namespace CoreMath
{
	typedef struct Vec2
	{
		union 
		{
			struct
			{
				float x;
				float y;
			};
			float asArray[2];
		};

		inline Vec2(float x, float y)
			: x(x), y(y) {}
		inline Vec2() : x(0.0f), y(0.0f) {}

		float& operator[](int i)
		{
			return asArray[i];
		}
	} Vec2;

	typedef struct Vec3
	{
		union 
		{
			struct
			{
				float x;
				float y;
				float z;
			};
			float asArray[3];
		};

		inline Vec3(float x, float y, float z) 
		: x(x), y(y), z(z) {}
		inline Vec3() : x(0.0f), y(0.0f), z(0.0f) {}

		float& operator[](int i)
		{
			return asArray[i];
		}
	} Vec3;

	typedef struct Vec4
	{
		union
		{
			struct
			{
				float x;
				float y;
				float z;
				float w;
			};
			float asArray[4];
		};

		inline Vec4(float x, float y, float z, float w)
			: x(x), y(y), z(z), w(w) {}
		inline Vec4() : x(0.0f), y(0.0f), z(0.0f), w(0.0f) {}

		float& operator[](int i)
		{
			return asArray[i];
		}
	} Vec4;

	Vec2 operator+(const Vec2& v1, const Vec2& v2);
	Vec3 operator+(const Vec3& v1, const Vec3& v2);
	Vec2 operator-(const Vec2& v1, const Vec2& v2);
	Vec3 operator-(const Vec3& v1, const Vec3& v2);
	Vec2 operator*(const Vec2& v1, const Vec2& v2);
	Vec3 operator*(const Vec3& v1, const Vec3& v2);
	Vec2 operator*(const Vec2& v, float f);
	Vec3 operator*(const Vec3& v, float f);
	bool operator==(const Vec2& v1, const Vec2& v2);
	bool operator==(const Vec3& v1, const Vec3& v);
	bool operator!=(const Vec2& v1, const Vec2& v2);
	bool operator!=(const Vec3& v1, const Vec3& v2);

	float Dot(const Vec2& v1, const Vec2& v2);
	float Dot(const Vec3& v1, const Vec3& v2);

	float Magnitude(const Vec2& v);
	float Magnitude(const Vec3& v);
	float MagnitudeSq(const Vec2& v);
	float MagnitudeSq(const Vec3& v);

	void Normalize(Vec2& v);
	void Normalize(Vec3& v);
	Vec2 Normalized(const Vec2& v);
	Vec3 Normalized(const Vec3& v);

	Vec3 Cross(const Vec3 v1, const Vec3 v2);

	float Angle(const Vec2& v1, const Vec2& v2);
	float Angle(const Vec3& v1, const Vec3& v2);

	Vec2 Project(const Vec2& length, const Vec2& direction);
	Vec3 Project(const Vec3& length, const Vec3& direction);
	Vec2 Perpendicular(const Vec2& length, const Vec2& direction);
	Vec3 Perpendicular(const Vec3& length, const Vec3& direction);

	Vec2 Reflection(const Vec2& vec, const Vec2& normal);
	Vec3 Reflection(const Vec3& vec, const Vec3& normal);
};