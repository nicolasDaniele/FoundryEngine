#include "PhysicsEngine/PhysicsApi.h"
#include "PhysicsEngine/PhysicsSystem.h"
#include "PhysicsEngine/RigidbodyVolume.h"

extern "C"
{
	PHYSICS_API PhysicsSystem* GetPhysicsSystem()
	{
		return new PhysicsSystem();
	}

	PHYSICS_API void UpdatePhysicsSystem(PhysicsSystem* physicsSystem, float frameTime)
	{
		physicsSystem->Update(frameTime);
	}

	PHYSICS_API void DestroyPhysicsSystem(PhysicsSystem* physicsSystemToDestroy)
	{
		delete physicsSystemToDestroy;
	}


	PHYSICS_API RigidbodyVolume* GetRigidbody(int bodyType, const Vec3& position,
		float mass, float friction,	float coefitientOfRestitution)
	{
		return new RigidbodyVolume(bodyType, position, mass, friction, coefitientOfRestitution);
	}

	PHYSICS_API void AddRigidbodyToPhysicsSystem(Rigidbody* rigidbody, PhysicsSystem* physicsSystem)
	{
		physicsSystem->AddRigidbody(rigidbody);
	}

	PHYSICS_API void AddConstraintToPhysicsSystem(const OBB& constraint, PhysicsSystem* physicsSystem)
	{
		physicsSystem->AddConstraint(constraint);
	}

	PHYSICS_API void SetRigidbodyBoxHalfExtents(RigidbodyVolume* rigidbody, const Vec3& halfExtents)
	{
		if (rigidbody->type != 3) return;
		rigidbody->SetBoxHalfExtents(halfExtents);
	}

	void SetRigidbodyBoxCenter(RigidbodyVolume* rigidbody, const Vec3& center)
	{
		if (rigidbody->type != 3) return;
		rigidbody->SetBoxCenter(center);
	}

	void SetRigidbodyBoxOrientation(RigidbodyVolume* rigidbody, const Mat3& orientation)
	{
		if (rigidbody->type != 3) return;
		rigidbody->SetBoxOrientation(orientation);
	}

	PHYSICS_API void SetRigidbodySphereRadius(RigidbodyVolume* rigidbody, const float radius)
	{
		if (rigidbody->type != 2) return;
		rigidbody->SetSphereRadius(radius);
	}

	void SetRigidbodySphereCenter(RigidbodyVolume* rigidbody, const Vec3& center)
	{
		if (rigidbody->type != 2) return;
		rigidbody->SetSphereCenter(center);
	}

	PHYSICS_API Vec3 GetRigidbodyPosition(RigidbodyVolume* rigidbody)
	{
		return rigidbody->GetPosition();
	}

	PHYSICS_API void AddLinearImpulseToRigidbody(RigidbodyVolume* rigidbody, const Vec3& impulse)
	{
		rigidbody->AddLinearImpulse(impulse);
	}
	PHYSICS_API void AddRotationalImpulseToRigidbody(RigidbodyVolume* rigidbody, const Vec3& point, const Vec3& impulse)
	{
		rigidbody->AddRotationalImpulse(point, impulse);
	}
}