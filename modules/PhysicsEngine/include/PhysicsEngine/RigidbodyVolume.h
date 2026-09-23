#pragma once

#include "PhysicsEngine/Rigidbody.h"
#include "PhysicsEngine/Collisions3D.h"
#include "Core/Quaternions.h"

#define GRAVITY Vec3(0.0f, -9.82f, 0.0f)

using Mat4 = CoreMath::Mat4;
using Vec3 = CoreMath::Vec3;
using Quaternion = CoreMath::Quaternion;
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
	void AddTorque(const Vec3& torque);
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
	inline float GetMass() const { return mass; }
	inline float GetDamping() const { return damping; }
	inline float GetAngularDamping() const { return angularDamping; }
	inline float GetRollingResistance() const { return rollingResistance; }
	inline int GetContactCount() const { return contactCount; }
	inline Quaternion GetOrientation() const { return orientation; }

	inline void SetAngularVelocity(const Vec3& angularVelocity) { angVel = angularVelocity; }
	// Mirrors the existing SetPosition() convention (see Rigidbody.h): this
	// does NOT immediately resync box.orientation / the world inertia tensor.
	// Those catch up on the next physics Update() via IntegrateVelocity/
	// IntegratePosition's existing SynchCollisionVolumes() calls, same as a
	// manual SetPosition() already behaves today.
	inline void SetOrientation(const Quaternion& newOrientation) { orientation = newOrientation; }
	void SetBoxHalfExtents(const Vec3& halfExtents);
	inline void SetBoxCenter(const Vec3& center) { box.center = center; }
	inline void SetBoxOrientation(const Mat3& orientation) { box.orientation = orientation; }
	void SetSphereRadius(const float radius);
	inline void SetSphereCenter(const Vec3& center) { sphere.center = center; }

	// Mass affects the local inertia tensor, so - like SetBoxHalfExtents/
	// SetSphereRadius - it isn't a trivial field assignment.
	void SetMass(float newMass);
	inline void SetFriction(float newFriction) { friction = newFriction; }
	inline void SetRestitution(float newRestitution) { restitution = newRestitution; }
	// damping/angularDamping/rollingResistance are all "strength" values in
	// [0,1]: 0 = no effect, 1 = fully removes the relevant velocity every
	// frame it's applied. See IntegrateVelocity/DampingFactor for the actual
	// formula and why values outside [0,1] are clamped there.
	inline void SetDamping(float newDamping) { damping = newDamping; }
	inline void SetAngularDamping(float newAngularDamping) { angularDamping = newAngularDamping; }
	// Only has an effect for B_SPHERE (see IntegrateVelocity) - the bodyType
	// gate lives at the public API boundary (IPhysics::SetRigidbodySphereRollingResistance),
	// same pattern as SetBoxHalfExtents/SetSphereRadius being type-specific there.
	inline void SetRollingResistance(float newRollingResistance) { rollingResistance = newRollingResistance; }

	// Locks rotation on any combination of WORLD axes (not local to the
	// body's own orientation - same space AddTorque/SetAngularVelocity/
	// AddRotationalImpulse already use). A locked axis's angular velocity is
	// zeroed every physics step (see IntegrateVelocity), so torque or
	// collision-driven spin on that axis never turns into visible rotation.
	// true = locked/frozen (matches Unity's "Freeze Rotation" checkboxes),
	// deliberately not a 0/1 float like damping - that inverted convention
	// was confusing enough once already.
	inline void SetRotationLock(bool lockX, bool lockY, bool lockZ)
	{
		rotationFreeMask = Vec3(lockX ? 0.0f : 1.0f, lockY ? 0.0f : 1.0f, lockZ ? 0.0f : 1.0f);
	}
	inline bool GetRotationLockX() const { return rotationFreeMask.x == 0.0f; }
	inline bool GetRotationLockY() const { return rotationFreeMask.y == 0.0f; }
	inline bool GetRotationLockZ() const { return rotationFreeMask.z == 0.0f; }

protected:
	float restitution;
	float friction;
	Vec3 angVel;
	Vec3 forcesSum;
	float mass;
	float damping;           // 0 = none, 1 = zeroes linear velocity every frame
	float angularDamping;    // 0 = none, 1 = zeroes angular velocity every frame
	float rollingResistance; // 0 = none, 1 = zeroes angular velocity every frame while rolling; only applied to B_SPHERE
	Vec3 rotationFreeMask = Vec3(1.0f, 1.0f, 1.0f); // 1 = free to rotate on that world axis, 0 = locked; see SetRotationLock
	int contactCount = 0;    // number of currently active collisions this body is part of
	OBB box;
	Sphere sphere;
	Quaternion orientation;
	Vec3 torquesSum;

	// invTensorLocal depends only on mass + shape dimensions, so it's cached
	// and only recomputed when those change (RecomputeLocalInertiaTensor).
	// invTensorWorld is invTensorLocal rotated into world space - it changes
	// every time orientation changes, so it's refreshed in
	// UpdateWorldInertiaTensor (called from SynchCollisionVolumes).
	Mat3 invTensorLocal;
	Mat3 invTensorWorld;

	void RecomputeLocalInertiaTensor();
	void UpdateWorldInertiaTensor();
};
