#pragma once

#include "Core/Vectors.h"
#include "Core/Matrices.h"

namespace CoreMath
{
	typedef struct Quaternion
	{
		union
		{
			struct
			{
				float w, x, y, z;
			};
			float asArray[4];
		};

		inline Quaternion() : w(1.0f), x(0.0f), y(0.0f), z(0.0f) {}
		inline Quaternion(float _w, float _x, float _y, float _z) : w(_w), x(_x), y(_y), z(_z) {}

		float& operator[](int i)
		{
			return asArray[i];
		}
	} Quaternion;

	// Hamilton product: composes rotations. (q1 * q2) applied to a vector
	// means "rotate by q2 first, then by q1" - same reading order as this
	// library's Mat3/Mat4 composition (e.g. modelMat = translation * rotation * scale).
	Quaternion operator*(const Quaternion& q1, const Quaternion& q2);
	Quaternion operator*(const Quaternion& q, float scalar);
	Quaternion operator+(const Quaternion& q1, const Quaternion& q2);
	bool operator==(const Quaternion& q1, const Quaternion& q2);
	bool operator!=(const Quaternion& q1, const Quaternion& q2);

	float Dot(const Quaternion& q1, const Quaternion& q2);
	float Magnitude(const Quaternion& q);
	float MagnitudeSq(const Quaternion& q);

	void Normalize(Quaternion& q);
	Quaternion Normalized(const Quaternion& q);

	// Conjugate is the cheap "inverse" for unit-length quaternions (which
	// orientation quaternions always should be, given they're renormalized
	// after every integration step - see RotateByVector). Inverse() is the
	// general-purpose version, safe even if q isn't unit-length.
	Quaternion Conjugate(const Quaternion& q);
	Quaternion Inverse(const Quaternion& q);

	// Builds a rotation quaternion for `angle` degrees around `axis`. Matches
	// the angle/axis convention already used by AxisAngle/AxisAngle3x3 in
	// Matrices.h (angle in degrees, converted internally).
	Quaternion AngleAxis(float angle, const Vec3& axis);

	// Convenience constructor from Euler angles, meant for one-off/authored
	// initial orientations only (e.g. placing a static prop) - NOT for
	// continuous simulation, see RotateByVector for that. Composition order
	// matches Rotation3x3/Rotation (Y * X * Z).
	Quaternion FromEuler(float pitch, float yaw, float roll);

	// Converts a (assumed unit-length) quaternion to a rotation matrix, using
	// the same convention as the rest of this library's Mat3/Mat4. Safe to
	// use directly both for a graphics model matrix
	// (modelMat = translation * ToMat4(q) * scale) and for physics
	// (box.orientation = ToMat3(q)).
	Mat3 ToMat3(const Quaternion& q);
	Mat4 ToMat4(const Quaternion& q);

	// Rotates a vector by a quaternion. Implemented via ToMat3 +
	// MultiplyMat3Vec3 rather than the textbook q*v*q^-1 formula, so it stays
	// automatically consistent with ToMat3's convention.
	Vec3 MultiplyQuatVec3(const Quaternion& q, const Vec3& v);

	// Millington-style orientation integration: advances q by the given
	// angular velocity over dt, using the quaternion derivative
	// q' = 0.5 * (angularVelocity as a pure quaternion) * q, then
	// renormalizes to counter floating-point drift. angularVelocity is
	// expected in WORLD space (matching how RigidbodyVolume already uses
	// angVel elsewhere, e.g. in the impulse-resolution Cross() calls).
	void RotateByVector(Quaternion& q, const Vec3& angularVelocity, float dt);

	// Spherical linear interpolation between two orientations. Not on the
	// critical path for rigidbody integration, but a standard utility to
	// have around (e.g. smoothing a visual rotation, camera work later on).
	Quaternion Slerp(const Quaternion& start, const Quaternion& end, float t);
}
