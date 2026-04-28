#pragma once

#include "Core/Vectors.h"
#include "Core/Geometry3D.h"
#include "PhysicsEngine/Collisions3D.h"
#include "EngineInterfaces/IPhysics.h"

#define RIGIDBODY_TYPE_BASE		0
#define RIGIDBODY_TYPE_PARTICLE		1
#define RIGIDBODY_TYPE_SPHERE		2
#define RIGIDBODY_TYPE_BOX		3

class ICollisionListener;

using Vec3 = CoreMath::Vec3;
using OBB = CoreGeometry::OBB;

class Rigidbody
{
public:
	int type;
	std::vector<ICollisionListener*> colliders;
	
	inline Rigidbody() { type = RIGIDBODY_TYPE_BASE; }
	virtual ~Rigidbody() = default;
	
	Rigidbody(const Rigidbody&) = delete;
	Rigidbody& operator=(const Rigidbody&) = delete;
	
	Rigidbody(Rigidbody&&) = default;
	Rigidbody& operator=(Rigidbody&&) = default;
	
	inline bool HasVolume() 
	{
		return type == RIGIDBODY_TYPE_SPHERE
		|| type == RIGIDBODY_TYPE_BOX;
	}
	
	virtual void Update(float frameTime) { }
	virtual void IntegrateVelocity(float frameTime) { }
	virtual void IntegratePosition(float frameTime) { }
	virtual void ApplyGravityForce() { }
	
	virtual void NotifyCollisionEnter(RigidbodyHandle self, RigidbodyHandle other, const CollisionData& data) { }
	virtual void NotifyCollisionStay(RigidbodyHandle self, RigidbodyHandle other, const CollisionData& data) { }
	virtual void NotifyCollisionExit(RigidbodyHandle self, RigidbodyHandle other) { }
	
	void AddCollisionListener(ICollisionListener* collider)
	{
		colliders.push_back(collider);
	}
	
	bool operator==(const Rigidbody& other) const
	{
		return handle.index == other.GetHandle().index &&
				handle.generation == other.GetHandle().generation;
	}

	void SetHandle(const RigidbodyHandle& newHandle) { handle = newHandle; }
	void SetPosition(const Vec3& newPosition) { position = newPosition; }
	void SetVelocity(const Vec3& newVelocity) { velocity = newVelocity; }
	
	inline RigidbodyHandle GetHandle() const { return handle; }
	inline Vec3 GetPosition() const { return position; }
	inline Vec3 GetVelocity() const { return velocity; }
	inline Vec3 GetAcceleration() const { return acceleration; }
	
protected:
	RigidbodyHandle handle;
	
	Vec3 position;
	Vec3 velocity;
	Vec3 acceleration;
};