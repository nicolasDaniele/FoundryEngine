#pragma once

#include <memory>
#include "Core/Geometry3D.h"
#include "PhysicsEngine/Rigidbody.h"
#include "EngineInterfaces/IPhysics.h"

class PhysicsSystem;
class RigidbodyVolume;

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

	RigidbodyHandle CreateRigidbody(int bodyType, const Vec3& position,
		float mass = 1.0f, float friction = 0.6f,
		float coefitientOfRestitution = 0.5f) override;
	void DestroyRigidbody(RigidbodyHandle rbHandle);

	void SetRigidbodyBoxHalfExtents(RigidbodyHandle rbHandle, const Vec3& halfExtents) override;
	void SetRigidbodyBoxCenter(RigidbodyHandle rbHandle, const Vec3& center) override;
	void SetRigidbodyBoxOrientation(RigidbodyHandle rbHandle, const Mat3& orientation) override;
	void SetRigidbodySphereRadius(RigidbodyHandle rbHandle, const float radius) override;
	void SetRigidbodySphereCenter(RigidbodyHandle rbHandle, const Vec3& center) override;
	Vec3 GetRigidbodyPosition(RigidbodyHandle rbHandle) override;

	void AddCollisionListenerToRigidbody(RigidbodyHandle rbHandle, ICollisionListener* listener) override;
	void AddLinearImpulseToRigidbody(RigidbodyHandle rbHandle, const Vec3& impulse) override;
	
	void Update(float frameTime) override;
	
	Rigidbody* GetRigidbodyFromHandle(RigidbodyHandle rbHandle);
	
private:
	bool IsValidRigidbodyHandle(RigidbodyHandle rbHandle);
	RigidbodyVolume* GetVolume(RigidbodyHandle rbHandle);
	PhysicsSystem* physicsSystem = nullptr;
	std::vector<RigidbodySlot> RBSlots;
};