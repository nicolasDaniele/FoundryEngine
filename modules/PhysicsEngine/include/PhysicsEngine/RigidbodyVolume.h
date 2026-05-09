#pragma once

#include "PhysicsEngine/Rigidbody.h"
#include "PhysicsEngine/Collisions3D.h"

#define GRAVITY Vec3(0.0f, -9.82f, 0.0f)

using Mat4 = CoreMath::Mat4;
using Vec3 = CoreMath::Vec3;
using Sphere = CoreGeometry::Sphere;
using OBB = CoreGeometry::OBB;

class RigidbodyVolume : public Rigidbody
{
public:
	RigidbodyVolume(BodyType _bodyType, const Vec3& _position,
		float _mass = 1.0f, float _friction = 0.6f,	float _restitution = 0.5f);
	~RigidbodyVolume() = default;

	RigidbodyVolume(const RigidbodyVolume&) = delete;
	RigidbodyVolume& operator=(const RigidbodyVolume&) = delete;

	RigidbodyVolume(RigidbodyVolume&&) = default;
	RigidbodyVolume& operator=(RigidbodyVolume&&) = default;

	void IntegrateVelocity(float frameTime);
	void IntegratePosition(float frameTime);
	void AddForce(const Vec3& force);
	void ApplyGravityForce();
	void ClearForces();
	void SynchCollisionVolumes();
	float GetInvMass();
	void AddLinearImpulse(const Vec3& impulse);

	void NotifyCollisionEnter(RigidbodyHandle self, RigidbodyHandle other, const CollisionData& data);
	void NotifyCollisionStay(RigidbodyHandle self, RigidbodyHandle other, const CollisionData& data);
	void NotifyCollisionExit(RigidbodyHandle self, RigidbodyHandle other);

	Mat3 GetInvTensor();
	virtual void AddRotationalImpulse(const Vec3& point, const Vec3& impulse);

	inline OBB GetBox() const { return box; }
	inline Sphere GetSphere() const { return sphere; }
	inline float GetRestitution() const { return restitution; }
	inline Vec3 GetAngularVelocity() const { return angVel; }
	inline float GetFriction() const { return friction; }

	inline void SetAngularVelocity(const Vec3& angularVelocity) { angVel = angularVelocity; }
	inline void SetBoxHalfExtents(const Vec3& halfExtents) { box.halfExtents = halfExtents; }
	inline void SetBoxCenter(const Vec3& center) { box.center = center; }
	inline void SetBoxOrientation(const Mat3& orientation) { box.orientation = orientation; }
	inline void SetSphereRadius(const float radius) { sphere.radius= radius; }
	inline void SetSphereCenter(const Vec3& center) { sphere.center = center; }

protected:
	float restitution;
	float friction;
	Vec3 angVel;
	Vec3 forcesSum;
	float mass;
	float damping;
	OBB box;
	Sphere sphere;
	Vec3 orientation;
	Vec3 torquesSum;
};