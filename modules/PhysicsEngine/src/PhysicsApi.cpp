#include <iostream>
#include "PhysicsEngine/PhysicsApi.h"
#include "PhysicsEngine/PhysicsContext.h"
#include "PhysicsEngine/RigidbodyVolume.h"

Physics::Physics()
{
	physicsContext = new PhysicsContext(this);
}

Physics::~Physics()
{
	delete physicsContext;
}

RigidbodyHandle Physics::CreateRigidbody(BodyType bodyType, const Vec3& position,
		float mass, float friction,	float restitution)
{
    uint32_t index = 0;

    for (uint32_t i = 0; i < RBSlots.size(); i++)
    {
        if (!RBSlots[i].alive)
        {
            index = i;
            RBSlots[i].alive = true;
            RBSlots[i].generation++;

            RBSlots[i].rigidbody = std::make_unique<RigidbodyVolume>(
                bodyType, position, mass, friction, restitution
            );

            break;
        }
    }

    if (index == 0 && (RBSlots.empty() || RBSlots[0].alive))
    {
        index = (uint32_t)RBSlots.size();

        RBSlots.emplace_back(
            std::make_unique<RigidbodyVolume>(
                bodyType, position, mass, friction, restitution
            ),
            0,
            true
        );
    }

    auto& slot = RBSlots[index];
    auto* rb = slot.rigidbody.get();

    RigidbodyHandle handle{ index, slot.generation };
    rb->SetHandle(handle);

    if (physicsContext && rb)
        physicsContext->AddRigidbody(rb);

    return handle;
}

void Physics::DestroyRigidbody(RigidbodyHandle rbHandle)
{
    if (!IsValidRigidbodyHandle(rbHandle))
	{
		std::cout << "[PhysicsEngine] Invalid RigidbodyHandle." << std::endl;
		return;
	}

    auto& slot = RBSlots[rbHandle.index];
    slot.rigidbody.reset();
    slot.alive = false;
    slot.generation++;
}

void Physics::SetRigidbodyBoxHalfExtents(RigidbodyHandle rbHandle, const Vec3& halfExtents)
{
	auto* volume = GetVolume(rbHandle);
	if (!volume || volume->bodyType != BodyType::B_BOX) return;

	volume->SetBoxHalfExtents(halfExtents);
}

void Physics::SetRigidbodyBoxCenter(RigidbodyHandle rbHandle, const Vec3& center)
{
	auto* volume = GetVolume(rbHandle);
	if (!volume || volume->bodyType != BodyType::B_BOX) return;

	volume->SetBoxCenter(center);
}

void Physics::SetRigidbodyBoxOrientation(RigidbodyHandle rbHandle, const Mat3& orientation)
{
	auto* volume = GetVolume(rbHandle);
	if (!volume || volume->bodyType != BodyType::B_BOX) return;

	volume->SetBoxOrientation(orientation);
}

void Physics::SetRigidbodySphereRadius(RigidbodyHandle rbHandle, const float radius)
{
	auto* volume = GetVolume(rbHandle);
	if (!volume || volume->bodyType != BodyType::B_SPHERE) return;

	volume->SetSphereRadius(radius);
}

void Physics::SetRigidbodySphereCenter(RigidbodyHandle rbHandle, const Vec3& center)
{
	auto* volume = GetVolume(rbHandle);
	if (!volume || volume->bodyType != BodyType::B_SPHERE)
	{
		std::cout << "[PhysicsEngine] Invalid RigidbodyHandle." << std::endl;
		return;
	}

	volume->SetSphereCenter(center);
}

void Physics::SetRigidbodySphereRollingResistance(RigidbodyHandle rbHandle, float rollingResistance)
{
	auto* volume = GetVolume(rbHandle);
	if (!volume || volume->bodyType != BodyType::B_SPHERE) return;

	volume->SetRollingResistance(rollingResistance);
}

Vec3 Physics::GetRigidbodyPosition(RigidbodyHandle rbHandle)
{
	if(!IsValidRigidbodyHandle(rbHandle))
	{
		std::cout << "[PhysicsEngine] Invalid RigidbodyHandle." << std::endl;
		return Vec3(0.0f);
	} 

	auto* rb = RBSlots[rbHandle.index].rigidbody.get();
	if (!rb) return Vec3(0.0f);

	return rb->GetPosition();
}

void Physics::SetRigidbodyPosition(RigidbodyHandle rbHandle, const Vec3& position)
{
	if(!IsValidRigidbodyHandle(rbHandle))
	{
		std::cout << "[PhysicsEngine] Invalid RigidbodyHandle." << std::endl;
		return;
	} 

	auto* rb = RBSlots[rbHandle.index].rigidbody.get();
	if (!rb) return;

	rb->SetPosition(position);
}

void Physics::SetRigidbodyLinearVelocity(RigidbodyHandle rbHandle, const Vec3& velocity)
{
	if(!IsValidRigidbodyHandle(rbHandle))
	{
		std::cout << "[PhysicsEngine] Invalid RigidbodyHandle." << std::endl;
		return;
	} 

	auto* rb = RBSlots[rbHandle.index].rigidbody.get();
	if (!rb) return;

	rb->SetVelocity(velocity);
}

void Physics::SetRigidbodyMass(RigidbodyHandle rbHandle, float mass)
{
	auto* volume = GetVolume(rbHandle);
	if (!volume) return;

	volume->SetMass(mass);
}

void Physics::SetRigidbodyFriction(RigidbodyHandle rbHandle, float friction)
{
	auto* volume = GetVolume(rbHandle);
	if (!volume) return;

	volume->SetFriction(friction);
}

void Physics::SetRigidbodyRestitution(RigidbodyHandle rbHandle, float restitution)
{
	auto* volume = GetVolume(rbHandle);
	if (!volume) return;

	volume->SetRestitution(restitution);
}

void Physics::SetRigidbodyDamping(RigidbodyHandle rbHandle, float damping)
{
	auto* volume = GetVolume(rbHandle);
	if (!volume) return;

	volume->SetDamping(damping);
}

void Physics::SetRigidbodyAngularDamping(RigidbodyHandle rbHandle, float angularDamping)
{
	auto* volume = GetVolume(rbHandle);
	if (!volume) return;

	volume->SetAngularDamping(angularDamping);
}

Quaternion Physics::GetRigidbodyOrientation(RigidbodyHandle rbHandle)
{
	auto* volume = GetVolume(rbHandle);
	if (!volume) return Quaternion();

	return volume->GetOrientation();
}

void Physics::SetRigidbodyOrientation(RigidbodyHandle rbHandle, const Quaternion& orientation)
{
	auto* volume = GetVolume(rbHandle);
	if (!volume) return;

	volume->SetOrientation(orientation);
}

void Physics::SetRigidbodyAngularVelocity(RigidbodyHandle rbHandle, const Vec3& angularVelocity)
{
	auto* volume = GetVolume(rbHandle);
	if (!volume) return;

	volume->SetAngularVelocity(angularVelocity);
}

void Physics::AddTorqueToRigidbody(RigidbodyHandle rbHandle, const Vec3& torque)
{
	auto* volume = GetVolume(rbHandle);
	if (!volume) return;

	volume->AddTorque(torque);
}

void Physics::AddRotationalImpulseToRigidbody(RigidbodyHandle rbHandle, const Vec3& point, const Vec3& impulse)
{
	auto* volume = GetVolume(rbHandle);
	if (!volume) return;

	volume->AddRotationalImpulse(point, impulse);
}

void Physics::SetRigidbodyRotationLock(RigidbodyHandle rbHandle, bool lockX, bool lockY, bool lockZ)
{
	auto* volume = GetVolume(rbHandle);
	if (!volume) return;

	volume->SetRotationLock(lockX, lockY, lockZ);
}

void Physics::AddCollisionListenerToRigidbody(RigidbodyHandle rbHandle, ICollisionListener* listener)
{
	if(!IsValidRigidbodyHandle(rbHandle) || !listener)
	{
		std::cout << "[PhysicsEngine] Invalid RigidbodyHandle or ICollisionListener." << std::endl;
		return;
	}

	auto* rb = RBSlots[rbHandle.index].rigidbody.get();
	rb->AddCollisionListener(listener);
}

void Physics::AddLinearImpulseToRigidbody(RigidbodyHandle rbHandle, const Vec3& impulse)
{
	if(!IsValidRigidbodyHandle(rbHandle))
	{
		std::cout << "[PhysicsEngine] Invalid RigidbodyHandle." << std::endl;
		return;
	} 

	auto* rb = RBSlots[rbHandle.index].rigidbody.get();
	auto* volume = dynamic_cast<RigidbodyVolume*>(rb);
	if (!volume) return;

	volume->AddLinearImpulse(impulse);
}

bool Physics::IsValidRigidbodyHandle(RigidbodyHandle rbHandle)
{
    return rbHandle.index < RBSlots.size() &&
           RBSlots[rbHandle.index].alive &&
           RBSlots[rbHandle.index].generation == rbHandle.generation;
}

RigidbodyVolume* Physics::GetVolume(RigidbodyHandle rbHandle)
{
	if(!IsValidRigidbodyHandle(rbHandle))
	{
		std::cout << "[PhysicsEngine] Invalid RigidbodyHandle." << std::endl;
		return nullptr;
	} 

	auto* rb = RBSlots[rbHandle.index].rigidbody.get();
	return dynamic_cast<RigidbodyVolume*>(rb);
}

void Physics::Update(float frameTime)
{
	if(!physicsContext)
	{
		std::cout << "[PhysicsEngine] PhysicsContext is null." << std::endl;
		return;
	}

	physicsContext->Update(frameTime);
}

Rigidbody* Physics::GetRigidbodyFromHandle(RigidbodyHandle rbHandle)
{
	if(!IsValidRigidbodyHandle(rbHandle))
	{
		std::cout << "[PhysicsEngine] Invalid RigidbodyHandle." << std::endl;
		return nullptr;
	} 

	return RBSlots[rbHandle.index].rigidbody.get();
}

extern "C"
{
	PHYSICS_API IPhysics* GetPhysicsEngine()
	{
		return new Physics();
	}

	PHYSICS_API void DestroyPhysicsEngine(IPhysics* physics)
	{
		delete physics;
	}
}
