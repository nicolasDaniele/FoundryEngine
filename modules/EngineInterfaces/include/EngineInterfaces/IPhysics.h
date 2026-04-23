#pragma once

#ifdef PHYSICSENGINE_EXPORTS
	#define PHYSICS_API __declspec(dllexport)
#else
	#define PHYSICS_API __declspec(dllimport)
#endif

#include "Core/Vectors.h"

struct RigidbodyHandle
{
    uint32_t index;
    uint32_t generation;
};

using Vec3 = CoreMath::Vec3;
using Mat3 = CoreMath::Mat3;

class IPhysics
{
public:
    virtual ~IPhysics() = default;

    virtual RigidbodyHandle CreateRigidbody(int bodyType, const Vec3& position,
		float mass = 1.0f, float friction = 0.6f,
		float coefitientOfRestitution = 0.5f) = 0;

	virtual void SetRigidbodyBoxHalfExtents(RigidbodyHandle rbHandle, const Vec3& halfExtents) = 0;
	virtual void SetRigidbodyBoxCenter(RigidbodyHandle rbHandle, const Vec3& center) = 0;
	virtual void SetRigidbodyBoxOrientation(RigidbodyHandle rbHandle, const Mat3& orientation) = 0;
	virtual void SetRigidbodySphereRadius(RigidbodyHandle rbHandle, const float radius) = 0;
	virtual void SetRigidbodySphereCenter(RigidbodyHandle rbHandle, const Vec3& center) = 0;
	virtual Vec3 GetRigidbodyPosition(RigidbodyHandle rbHandle) = 0;
	
	virtual void AddLinearImpulseToRigidbody(RigidbodyHandle rbHandle, const Vec3& impulse) = 0;
	virtual void Update(float frameTime) = 0;
};

extern "C"
{
	PHYSICS_API IPhysics* GetPhysicsEngine();
	PHYSICS_API void DestroyPhysicsEngine(IPhysics*);
}