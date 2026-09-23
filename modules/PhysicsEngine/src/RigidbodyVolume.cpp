#include <iostream>
#include "Core/Geometry3D.h"
#include "Core/Matrices.h"
#include "Core/Vectors.h"
#include "Core/Utils.h"
#include "Core/MathDefinitions.h"
#include "EngineInterfaces/PhysicsInterfaces.h"
#include "PhysicsEngine/RigidbodyVolume.h"

namespace
{
	// Converts a user-facing "strength" value (0 = no effect, 1 = fully
	// removes the relevant velocity every frame it's applied) into the
	// multiplicative factor actually used in IntegrateVelocity. Clamped to
	// [0,1]: a strength outside that range would make (1 - strength)
	// negative, and POW() with a negative base and a non-integer exponent
	// (frameTime is essentially never an integer) produces NaN - silently
	// corrupting velocity instead of failing loudly.
	float DampingFactor(float strength, float frameTime)
	{
		Utils::Clamp(strength, 0.0f, 1.0f);
		return POW(1.0f - strength, frameTime);
	}
}

RigidbodyVolume::RigidbodyVolume(BodyType _bodyType, const Vec3& _position, float _mass, float _friction, float _restitution)
{
	bodyType = _bodyType;
	position = _position;
	mass = _mass;
	friction = _friction;
	restitution = _restitution;
	
	// @TODO: Pass in damping as a parameter
	// 0.2 strength here reproduces the old hardcoded "0.8 factor" default
	// from before damping/angularDamping were flipped to the 0=none/1=max
	// convention (0.8 factor == 1 - 0.2 strength).
	damping = 0.2f;
	angularDamping = 0.2f;
	rollingResistance = 0.0f; // no resistance by default; only meaningful for spheres - see SetRigidbodySphereRollingResistance

	// Gives invTensorLocal/invTensorWorld a valid value immediately, using
	// whatever the box/sphere's default dimensions are at this point. If
	// SetBoxHalfExtents/SetSphereRadius are called afterwards (as App.cpp
	// already does), they'll recompute this again with the real dimensions.
	RecomputeLocalInertiaTensor();
}

void RigidbodyVolume::IntegrateVelocity(float frameTime)
{
	acceleration = forcesSum * GetInvMass();

	velocity = velocity + acceleration * frameTime;
	velocity = velocity * DampingFactor(damping, frameTime);

	// invTensorWorld here is one frame stale (it reflects the orientation
	// from the last SynchCollisionVolumes call, not the one about to be
	// computed after this integrates). That's the standard semi-implicit
	// approach and not a bug - the same way `acceleration` above is computed
	// from this frame's forces but velocity before this frame's motion.
	Vec3 angularAcceleration = MultiplyMat3Vec3(invTensorWorld, torquesSum);
	angVel = angVel + angularAcceleration * frameTime;
	angVel = angVel * DampingFactor(angularDamping, frameTime);

	// Rolling resistance: distinct from Coulomb friction (which only opposes
	// slip - see ApplyImpulses) and from angularDamping (which acts
	// unconditionally, in the air too). Only meaningful for spheres (nothing
	// else in this engine "rolls" continuously), and only while actually
	// touching something.
	if (bodyType == BodyType::B_SPHERE && contactCount > 0)
		angVel = angVel * DampingFactor(rollingResistance, frameTime);

	// Per-axis rotation lock: zero angular velocity on any locked world axis,
	// every frame, regardless of what added angular velocity there this
	// frame (torque, a collision impulse resolved in SolveImpulses last
	// frame, a direct AddRotationalImpulse call from gameplay code, etc.).
	// Applied last and unconditionally so nothing upstream can bypass it.
	angVel = angVel * rotationFreeMask;
}

void RigidbodyVolume::IntegratePosition(float frameTime)
{
	position = position + velocity * frameTime;

	CoreMath::RotateByVector(orientation, angVel, frameTime);

	SynchCollisionVolumes();
}

void RigidbodyVolume::AddForce(const Vec3& force)
{
	forcesSum = forcesSum + force;
}

void RigidbodyVolume::AddTorque(const Vec3& torque)
{
	torquesSum = torquesSum + torque;
}

void RigidbodyVolume::ApplyGravityForce()
{
	forcesSum = forcesSum + GRAVITY * mass;
}

void RigidbodyVolume::ClearForces()
{
	forcesSum = Vec3(0.0f, 0.0f, 0.0f);
	torquesSum = Vec3(0.0f, 0.0f, 0.0f);
}

void RigidbodyVolume::SynchCollisionVolumes()
{
	if (bodyType == BodyType::B_BOX)
	{
		box.center = position;
		box.orientation = CoreMath::ToMat3(orientation);
	}
	else if (bodyType == BodyType::B_SPHERE)
		sphere.center = position;

	UpdateWorldInertiaTensor();
}

float RigidbodyVolume::GetInvMass()
{
	return mass == 0.0f ? 0.0f : 1.0f / mass;
}

void RigidbodyVolume::AddLinearImpulse(const Vec3& impulse)
{
	velocity = velocity + impulse;
}

void RigidbodyVolume::NotifyCollisionEnter(RigidbodyHandle self, RigidbodyHandle other, const CollisionData& data)
{
	contactCount++;

	for (auto l : colliders)
        l->OnCollisionEnter(self, other, data);
}

void RigidbodyVolume::NotifyCollisionStay(RigidbodyHandle self, RigidbodyHandle other, const CollisionData& data)
{
	for (auto l : colliders)
        l->OnCollisionStay(self, other, data);
}

void RigidbodyVolume::NotifyCollisionExit(RigidbodyHandle self, RigidbodyHandle other)
{
	if (contactCount > 0)
		contactCount--;

	for (auto l : colliders)
        l->OnCollisionExit(self, other);
}

void RigidbodyVolume::RecomputeLocalInertiaTensor()
{
	float ix = 0.0f;
	float iy = 0.0f;
	float iz = 0.0f;

	if (mass != 0.0f && bodyType == BodyType::B_SPHERE)
	{
		float r2 = sphere.radius * sphere.radius;
		float fraction = (2.0f / 5.0f);
		ix = r2 * mass * fraction;
		iy = r2 * mass * fraction;
		iz = r2 * mass * fraction;
	}
	else if (mass != 0.0f && bodyType == BodyType::B_BOX)
	{
		Vec3 size = box.halfExtents * 2.0f;
		float fraction = (1.0f / 12.0f);
		float x2 = size.x * size.x;
		float y2 = size.y * size.y;
		float z2 = size.z * size.z;
		ix = (y2 + z2) * mass * fraction;
		iy = (x2 + z2) * mass * fraction;
		iz = (x2 + y2) * mass * fraction;
	}

	// mass == 0 means "static / infinite mass" - same convention GetInvMass()
	// already uses. The inverse tensor must be the TRUE zero matrix in that
	// case, not the identity that CoreMath::Inverse() falls back to for a
	// singular (all-zero) matrix - otherwise a static body would incorrectly
	// gain angular acceleration the moment any torque is ever applied to it.
	if (mass == 0.0f || ix == 0.0f || iy == 0.0f || iz == 0.0f)
	{
		invTensorLocal = Mat3(0, 0, 0, 0, 0, 0, 0, 0, 0);
	}
	else
	{
		// This is a diagonal matrix - invert it by just reciprocating each
		// entry, instead of paying for CoreMath::Inverse(Mat3)'s general
		// adjugate/cofactor/determinant path (which the old code used here).
		invTensorLocal = Mat3(
			1.0f / ix, 0.0f,      0.0f,
			0.0f,      1.0f / iy, 0.0f,
			0.0f,      0.0f,      1.0f / iz
		);
	}

	UpdateWorldInertiaTensor();
}

void RigidbodyVolume::UpdateWorldInertiaTensor()
{
	if (bodyType == BodyType::B_BOX)
	{
		Mat3 R = box.orientation;
		invTensorWorld = R * invTensorLocal * CoreMath::Transpose(R);
	}
	else
	{
		// Spheres are rotationally symmetric: the local (diagonal, isotropic)
		// tensor is already correct in every orientation - rotating it would
		// be wasted work for the exact same result.
		invTensorWorld = invTensorLocal;
	}
}

void RigidbodyVolume::SetMass(float newMass)
{
	mass = newMass;
	RecomputeLocalInertiaTensor();
}

void RigidbodyVolume::SetBoxHalfExtents(const Vec3& halfExtents)
{
	box.halfExtents = halfExtents;
	RecomputeLocalInertiaTensor();
}

void RigidbodyVolume::SetSphereRadius(const float radius)
{
	sphere.radius = radius;
	RecomputeLocalInertiaTensor();
}

Mat3 RigidbodyVolume::GetInvTensor()
{
	return invTensorWorld;
}

void RigidbodyVolume::AddRotationalImpulse(const Vec3& point, const Vec3& impulse)
{
	Vec3 centerOfMass = position;
	Vec3 torque = Cross(point - centerOfMass, impulse);

	Vec3 angAccel = MultiplyMat3Vec3(GetInvTensor(), torque);
	angVel = angVel + angAccel;
}
