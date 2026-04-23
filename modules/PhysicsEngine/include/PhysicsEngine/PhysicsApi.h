#pragma once

#ifdef PHYSICSENGINE_EXPORTS
	#define PHYSICS_API __declspec(dllexport)
#else
	#define PHYSICS_API __declspec(dllimport)
#endif

#include "Core/Vectors.h"
#include "Core/Geometry3D.h"
#include "Rigidbody.h"

using Vec3 = CoreMath::Vec3;
using Mat3 = CoreMath::Mat3;
using OBB = CoreGeometry::OBB;

class PhysicsSystem;
class RigidbodyVolume;
class Rigidbody;

extern "C"
{
	PHYSICS_API PhysicsSystem* GetPhysicsSystem();
	PHYSICS_API void UpdatePhysicsSystem(PhysicsSystem* physicsSystem, float frameTime);
	PHYSICS_API void DestroyPhysicsSystem(PhysicsSystem* physicsSystemToDestroy);

	PHYSICS_API RigidbodyVolume* GetRigidbody(int bodyType, const Vec3& position,
		float mass = 1.0f, float friction = 0.6f,
		float coefitientOfRestitution = 0.5f);
	PHYSICS_API void AddRigidbodyToPhysicsSystem(Rigidbody* rigidbody, PhysicsSystem* physicsSystem);
	
	PHYSICS_API void SetRigidbodyBoxHalfExtents(RigidbodyVolume* rigidbody, const Vec3& halfExtents);
	PHYSICS_API void SetRigidbodyBoxCenter(RigidbodyVolume* rigidbody, const Vec3& center);
	PHYSICS_API void SetRigidbodyBoxOrientation(RigidbodyVolume* rigidbody, const Mat3& orientation);
	PHYSICS_API void SetRigidbodySphereRadius(RigidbodyVolume* rigidbody, const float radius);
	PHYSICS_API void SetRigidbodySphereCenter(RigidbodyVolume* rigidbody, const Vec3& center);
	PHYSICS_API Vec3 GetRigidbodyPosition(RigidbodyVolume* rigidbody);

	PHYSICS_API void AddCollisionListenerToRigidbody(RigidbodyVolume* rigidbody, ICollisionListener* collisionListener);
	PHYSICS_API void AddLinearImpulseToRigidbody(RigidbodyVolume* rigidbody, const Vec3& impulse);
}