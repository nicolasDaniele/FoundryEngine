#include <iostream>
#include "Core/Geometry3D.h"
#include "Core/Matrices.h"
#include "Core/Vectors.h"
#include "Core/MathDefinitions.h"
#include "PhysicsEngine/RigidbodyVolume.h"
#include "EngineInterfaces/PhysicsInterfaces.h"

RigidbodyVolume::RigidbodyVolume(int _bodyType, const Vec3& _position, float _mass, float _friction, float _restitution)
{
	type = _bodyType;
	position = _position;
	mass = _mass;
	friction = _friction;
	restitution = _restitution;
	
	// @TODO: Pass in damping as a parameter
	damping = 0.8f;
}

void RigidbodyVolume::IntegrateVelocity(float frameTime)
{
	acceleration = forcesSum * GetInvMass();

	velocity = velocity + acceleration * frameTime;
	velocity = velocity * POW(damping, frameTime);
}

void RigidbodyVolume::IntegratePosition(float frameTime)
{
	position = position + velocity * frameTime;
	SynchCollisionVolumes();
}

void RigidbodyVolume::AddForce(const Vec3& force)
{
	forcesSum = forcesSum + force;
}

void RigidbodyVolume::ApplyGravityForce()
{
	forcesSum = forcesSum + GRAVITY * mass;
}

void RigidbodyVolume::ClearForces()
{
	forcesSum = Vec3(0.0f, 0.0f, 0.0f);
}

void RigidbodyVolume::SynchCollisionVolumes()
{
	if (type == RIGIDBODY_TYPE_BOX)
	{
		box.center = position;

		box.orientation = CoreMath::Rotation3x3(
			RAD2DEG(orientation.x),
			RAD2DEG(orientation.y),
			RAD2DEG(orientation.z)
		);
	}
	else if (type == RIGIDBODY_TYPE_SPHERE)
		sphere.center = position;
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
	for (auto l : colliders)
        l->OnCollisionExit(self, other);
}

Mat3 RigidbodyVolume::GetInvTensor()
{
	float ix = 0.0f;
	float iy = 0.0f;
	float iz = 0.0f;

	if (mass != 0 && type == RIGIDBODY_TYPE_SPHERE)
	{
		float r2 = sphere.radius * sphere.radius;
		float fraction = (2.0f / 5.0f);
		ix = r2 * mass * fraction;
		iy = r2 * mass * fraction;
		iz = r2 * mass * fraction;
	}
	else if (mass != 0 && type == RIGIDBODY_TYPE_BOX)
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

	return CoreMath::Inverse(Mat3(
		ix, 0, 0,
		0, iy, 0,
		0, 0, iz));
}

void RigidbodyVolume::AddRotationalImpulse(const Vec3& point, const Vec3& impulse)
{
	Vec3 centerOfMass = position;
	Vec3 torque = Cross(point - centerOfMass, impulse);

	Vec3 angAccel = MultiplyMat3Vec3(GetInvTensor(), torque);
	angVel = angVel + angAccel;
}