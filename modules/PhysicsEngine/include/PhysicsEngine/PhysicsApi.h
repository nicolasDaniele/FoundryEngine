#pragma once

#include <memory>
#include "Core/Geometry3D.h"
#include "PhysicsEngine/Rigidbody.h"
#include "EngineInterfaces/IPhysics.h"

class PhysicsContext;
class RigidbodyVolume;

/// Internal storage slot for Rigidbodies.
///
/// Uses a generation counter to invalidate stale handles.
/// When a Rigidbody is destroyed, the slot can be reused
/// but its generation is incremented.
struct RigidbodySlot
{
	std::unique_ptr<Rigidbody> rigidbody;
	uint32_t generation;
	bool alive;

	RigidbodySlot() = default;

	RigidbodySlot(std::unique_ptr<Rigidbody> rb, uint32_t gen, bool alive)
        : rigidbody(std::move(rb)), generation(gen), alive(alive) {}

	RigidbodySlot(const RigidbodySlot&) = delete;
    RigidbodySlot& operator=(const RigidbodySlot&) = delete;

    RigidbodySlot(RigidbodySlot&&) = default;
    RigidbodySlot& operator=(RigidbodySlot&&) = default;
};

class PHYSICS_API Physics : public IPhysics
{
public:
	Physics();
	~Physics();

	Physics(const Physics&) = delete;
    Physics& operator=(const Physics&) = delete;

    Physics(Physics&&) = default;
    Physics& operator=(Physics&&) = default;

	RigidbodyHandle CreateRigidbody(BodyType bodyType, const Vec3& position,
		float mass = 1.0f, float friction = 0.6f, float restitution = 0.5f) override;
	void DestroyRigidbody(RigidbodyHandle rbHandle);

	void SetRigidbodyBoxHalfExtents(RigidbodyHandle rbHandle, const Vec3& halfExtents) override;
	void SetRigidbodyBoxCenter(RigidbodyHandle rbHandle, const Vec3& center) override;
	void SetRigidbodyBoxOrientation(RigidbodyHandle rbHandle, const Mat3& orientation) override;
	void SetRigidbodySphereRadius(RigidbodyHandle rbHandle, const float radius) override;
	void SetRigidbodySphereCenter(RigidbodyHandle rbHandle, const Vec3& center) override;
	void SetRigidbodySphereRollingResistance(RigidbodyHandle rbHandle, float rollingResistance) override;

	Vec3 GetRigidbodyPosition(RigidbodyHandle rbHandle) override;
	void SetRigidbodyPosition(RigidbodyHandle rbHandle, const Vec3& position) override;
	void SetRigidbodyLinearVelocity(RigidbodyHandle rbHandle, const Vec3& velocity) override;

	void SetRigidbodyMass(RigidbodyHandle rbHandle, float mass) override;
	void SetRigidbodyFriction(RigidbodyHandle rbHandle, float friction) override;
	void SetRigidbodyRestitution(RigidbodyHandle rbHandle, float restitution) override;
	void SetRigidbodyDamping(RigidbodyHandle rbHandle, float damping) override;
	void SetRigidbodyAngularDamping(RigidbodyHandle rbHandle, float angularDamping) override;

	Quaternion GetRigidbodyOrientation(RigidbodyHandle rbHandle) override;
	void SetRigidbodyOrientation(RigidbodyHandle rbHandle, const Quaternion& orientation) override;
	void SetRigidbodyAngularVelocity(RigidbodyHandle rbHandle, const Vec3& angularVelocity) override;
	void AddTorqueToRigidbody(RigidbodyHandle rbHandle, const Vec3& torque) override;
	void AddRotationalImpulseToRigidbody(RigidbodyHandle rbHandle, const Vec3& point, const Vec3& impulse) override;
	void SetRigidbodyRotationLock(RigidbodyHandle rbHandle, bool lockX, bool lockY, bool lockZ) override;

	void AddCollisionListenerToRigidbody(RigidbodyHandle rbHandle, ICollisionListener* listener) override;
	void AddLinearImpulseToRigidbody(RigidbodyHandle rbHandle, const Vec3& impulse) override;
	
	bool IsValidRigidbodyHandle(RigidbodyHandle rbHandle) override;
	
	void Update(float frameTime) override;
	
	Rigidbody* GetRigidbodyFromHandle(RigidbodyHandle rbHandle);
	
private:
	RigidbodyVolume* GetVolume(RigidbodyHandle rbHandle);
	PhysicsContext* physicsContext = nullptr;
	std::vector<RigidbodySlot> RBSlots;
};
