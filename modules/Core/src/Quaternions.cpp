#include "Core/Quaternions.h"
#include "Core/MathDefinitions.h"

namespace CoreMath
{
	Quaternion operator*(const Quaternion& q1, const Quaternion& q2)
	{
		return Quaternion(
			q1.w * q2.w - q1.x * q2.x - q1.y * q2.y - q1.z * q2.z,
			q1.w * q2.x + q1.x * q2.w + q1.y * q2.z - q1.z * q2.y,
			q1.w * q2.y - q1.x * q2.z + q1.y * q2.w + q1.z * q2.x,
			q1.w * q2.z + q1.x * q2.y - q1.y * q2.x + q1.z * q2.w
		);
	}

	Quaternion operator*(const Quaternion& q, float scalar)
	{
		return Quaternion(q.w * scalar, q.x * scalar, q.y * scalar, q.z * scalar);
	}

	Quaternion operator+(const Quaternion& q1, const Quaternion& q2)
	{
		return Quaternion(q1.w + q2.w, q1.x + q2.x, q1.y + q2.y, q1.z + q2.z);
	}

	bool operator==(const Quaternion& q1, const Quaternion& q2)
	{
		return CMP(q1.w, q2.w) && CMP(q1.x, q2.x) && CMP(q1.y, q2.y) && CMP(q1.z, q2.z);
	}

	bool operator!=(const Quaternion& q1, const Quaternion& q2)
	{
		return !(q1 == q2);
	}

	float Dot(const Quaternion& q1, const Quaternion& q2)
	{
		return q1.w * q2.w + q1.x * q2.x + q1.y * q2.y + q1.z * q2.z;
	}

	float Magnitude(const Quaternion& q)
	{
		return SQRT(Dot(q, q));
	}

	float MagnitudeSq(const Quaternion& q)
	{
		return Dot(q, q);
	}

	void Normalize(Quaternion& q)
	{
		float mag = Magnitude(q);
		if (mag < FLT_EPSILON)
		{
			q = Quaternion(); // degenerate case - fall back to identity instead of dividing by ~0
			return;
		}

		q = q * (1.0f / mag);
	}

	Quaternion Normalized(const Quaternion& q)
	{
		Quaternion result = q;
		Normalize(result);
		return result;
	}

	Quaternion Conjugate(const Quaternion& q)
	{
		return Quaternion(q.w, -q.x, -q.y, -q.z);
	}

	Quaternion Inverse(const Quaternion& q)
	{
		float magSq = MagnitudeSq(q);
		if (magSq < FLT_EPSILON)
			return Quaternion();

		return Conjugate(q) * (1.0f / magSq);
	}

	Quaternion AngleAxis(float angle, const Vec3& axis)
	{
		float halfRad = DEG2RAD(angle) * 0.5f;
		float s = SIN(halfRad);

		Vec3 a = Normalized(axis);

		return Quaternion(COS(halfRad), a.x * s, a.y * s, a.z * s);
	}

	Quaternion FromEuler(float pitch, float yaw, float roll)
	{
		Quaternion qYaw   = AngleAxis(yaw,   Vec3(0.0f, 1.0f, 0.0f));
		Quaternion qPitch = AngleAxis(pitch, Vec3(1.0f, 0.0f, 0.0f));
		Quaternion qRoll  = AngleAxis(roll,  Vec3(0.0f, 0.0f, 1.0f));

		return qYaw * qPitch * qRoll;
	}

	Mat3 ToMat3(const Quaternion& q)
	{
		float w = q.w, x = q.x, y = q.y, z = q.z;

		return Mat3(
			1.0f - 2.0f * (y * y + z * z), 2.0f * (x * y + w * z),        2.0f * (x * z - w * y),
			2.0f * (x * y - w * z),        1.0f - 2.0f * (x * x + z * z), 2.0f * (y * z + w * x),
			2.0f * (x * z + w * y),        2.0f * (y * z - w * x),        1.0f - 2.0f * (x * x + y * y)
		);
	}

	Mat4 ToMat4(const Quaternion& q)
	{
		Mat3 r = ToMat3(q);

		return Mat4(
			r._11, r._12, r._13, 0.0f,
			r._21, r._22, r._23, 0.0f,
			r._31, r._32, r._33, 0.0f,
			0.0f,  0.0f,  0.0f,  1.0f
		);
	}

	Vec3 MultiplyQuatVec3(const Quaternion& q, const Vec3& v)
	{
		return MultiplyMat3Vec3(ToMat3(q), v);
	}

	void RotateByVector(Quaternion& q, const Vec3& angularVelocity, float dt)
	{
		Quaternion angularQuat(0.0f, angularVelocity.x, angularVelocity.y, angularVelocity.z);
		Quaternion derivative = (angularQuat * q) * 0.5f;

		q = q + derivative * dt;
		Normalize(q);
	}

	Quaternion Slerp(const Quaternion& start, const Quaternion& end, float t)
	{
		Quaternion from = start;
		Quaternion to = end;

		float dot = Dot(from, to);

		// q and -q represent the same rotation; pick the sign that gives the
		// shorter path so Slerp doesn't take the "long way around".
		if (dot < 0.0f)
		{
			to = to * -1.0f;
			dot = -dot;
		}

		if (dot > 0.9995f)
		{
			// Nearly identical rotations - plain lerp + normalize avoids
			// dividing by a near-zero sin(theta0) in the general formula below.
			Quaternion result = from + (to + from * -1.0f) * t;
			Normalize(result);
			return result;
		}

		float theta0 = ACOS(dot);
		float theta = theta0 * t;

		Quaternion orthogonal = Normalized(to + from * -dot);

		return from * COS(theta) + orthogonal * SIN(theta);
	}
}
