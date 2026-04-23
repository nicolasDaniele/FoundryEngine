#include "PhysicsEngine/PhysicsApi.h"
#include "PhysicsEngine/PhysicsSystem.h"
#include "PhysicsEngine/RigidbodyVolume.h"

Physics::Physics()
{
	physicsSystem = new PhysicsSystem();
}

Physics::~Physics()
{
	delete physicsSystem;
}

// @TODO: Print error message if Rigidbody is not valid (in every function)

RigidbodyHandle Physics::CreateRigidbody(int bodyType, const Vec3& position,
		float mass, float friction,	float coefitientOfRestitution)
{
	for (uint32_t i = 0; i < RBSlots.size(); i++)
	{
		if (!RBSlots[i].alive)
		{
			RBSlots[i].alive = true;
			RBSlots[i].generation++;

			RBSlots[i].rigidbody = std::make_unique<RigidbodyVolume>(
						bodyType, position, mass, friction, coefitientOfRestitution);

			return { i, RBSlots[i].generation };
		}
	}

	RBSlots.emplace_back(
		std::make_unique<RigidbodyVolume>(
			bodyType, position, mass, friction, coefitientOfRestitution),
		0,
		true
	);

	uint32_t index = (uint32_t)(RBSlots.size() - 1);
	auto* rb = RBSlots[index].rigidbody.get();
	if (physicsSystem && rb)
		physicsSystem->AddRigidbody(rb);
	
	return { index, 0 };
}

void Physics::SetRigidbodyBoxHalfExtents(RigidbodyHandle rbHandle, const Vec3& halfExtents)
{
	auto* volume = GetVolume(rbHandle);
	if (!volume || volume->type != 3) return;

	volume->SetBoxHalfExtents(halfExtents);
}

void Physics::SetRigidbodyBoxCenter(RigidbodyHandle rbHandle, const Vec3& center)
{
	auto* volume = GetVolume(rbHandle);
	if (!volume || volume->type != 3) return;

	volume->SetBoxCenter(center);
}

void Physics::SetRigidbodyBoxOrientation(RigidbodyHandle rbHandle, const Mat3& orientation)
{
	auto* volume = GetVolume(rbHandle);
	if (!volume || volume->type != 3) return;

	volume->SetBoxOrientation(orientation);
}

void Physics::SetRigidbodySphereRadius(RigidbodyHandle rbHandle, const float radius)
{
	auto* volume = GetVolume(rbHandle);
	if (!volume || volume->type != 2) return;

	volume->SetSphereRadius(radius);
}

void Physics::SetRigidbodySphereCenter(RigidbodyHandle rbHandle, const Vec3& center)
{
	auto* volume = GetVolume(rbHandle);
	if (!volume || volume->type != 2) return;

	volume->SetSphereCenter(center);
}

Vec3 Physics::GetRigidbodyPosition(RigidbodyHandle rbHandle)
{
	auto* rb = RBSlots[rbHandle.index].rigidbody.get();
	if (!rb) return Vec3(0.0f, 0.0f, 0.0f);

	return rb->GetPosition();
}

void Physics::AddLinearImpulseToRigidbody(RigidbodyHandle rbHandle, const Vec3& impulse)
{
	auto* rb = RBSlots[rbHandle.index].rigidbody.get();
	auto* volume = dynamic_cast<RigidbodyVolume*>(rb);
	if (!volume) return;

	volume->AddLinearImpulse(impulse);
}

bool Physics::IsValidRigidbody(RigidbodyHandle rbHandle)
{
    return rbHandle.index < RBSlots.size() &&
           RBSlots[rbHandle.index].alive &&
           RBSlots[rbHandle.index].generation == rbHandle.generation;
}

RigidbodyVolume* Physics::GetVolume(RigidbodyHandle handle)
{
	if (!IsValidRigidbody(handle)) return nullptr;

	auto* rb = RBSlots[handle.index].rigidbody.get();
	if (!rb) return nullptr;

	return dynamic_cast<RigidbodyVolume*>(rb);
}

void Physics::Update(float frameTime)
{
	if(!physicsSystem) return;

	physicsSystem->Update(frameTime);
}

extern "C"
{
	PHYSICS_API IPhysics* GetPhysicsEngine()
	{
		return new Physics();
	}

	PHYSICS_API void DestroyPhysicsEngine(IPhysics* physicsEngine)
	{
		delete physicsEngine;
	}
}