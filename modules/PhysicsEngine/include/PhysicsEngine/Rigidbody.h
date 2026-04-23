#pragma once

#include "Core/Vectors.h"
#include "Core/Geometry3D.h"

#define RIGIDBODY_TYPE_BASE		0
#define RIGIDBODY_TYPE_PARTICLE		1
#define RIGIDBODY_TYPE_SPHERE		2
#define RIGIDBODY_TYPE_BOX		3

using Vec3 = CoreMath::Vec3;
using OBB = CoreGeometry::OBB;

class Rigidbody
{
public:
	int type;

	inline Rigidbody() { type = RIGIDBODY_TYPE_BASE; }
	virtual ~Rigidbody() = default;
	
	Rigidbody(const Rigidbody&) = delete;
	Rigidbody& operator=(const Rigidbody&) = delete;

	Rigidbody(Rigidbody&&) = default;
	Rigidbody& operator=(Rigidbody&&) = default;

	inline bool HasVolume() 
	{
		return type == RIGIDBODY_TYPE_SPHERE
			|| type == RIGIDBODY_TYPE_BOX;
	}
	virtual void Update(float frameTime) { }
	virtual void IntegrateVelocity(float frameTime) { }
	virtual void IntegratePosition(float frameTime) { }
	virtual void ApplyGravityForce() { }

	void SetPosition(const Vec3& newPosition) { position = newPosition; }
	void SetVelocity(const Vec3& newVelocity) { velocity = newVelocity; }

	inline Vec3 GetPosition() const { return position; }
	inline Vec3 GetVelocity() const { return velocity; }
	inline Vec3 GetAcceleration() const { return acceleration; }

protected:
	Vec3 position;
	Vec3 velocity;
	Vec3 acceleration;
};