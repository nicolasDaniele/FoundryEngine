#pragma once

#ifdef PHYSICSENGINE_EXPORTS
	#define PHYSICS_API __declspec(dllexport)
#else
	#define PHYSICS_API __declspec(dllimport)
#endif

#include "Core/Vectors.h"
#include "EngineInterfaces/PhysicsTypes.h"

/// Lightweight handle used to reference a Rigidbody.
///
/// A handle is composed of:
/// - index: position in the internal slot array
/// - generation: used to validate that the slot has not been reused
///
/// This prevents accessing destroyed objects.
struct RigidbodyHandle
{
    uint32_t index;
    uint32_t generation;
};

using Vec3 = CoreMath::Vec3;
using Mat3 = CoreMath::Mat3;

class ICollisionListener;

class IPhysics
{
public:
    virtual ~IPhysics() = default;

	/// Creates a Rigidbody and returns a handle to it.
	/// A handle is used to reference the Rigidbody without exposing internal pointers.
	/// The handle becomes invalid if:
	/// - The Rigidbody is destroyed.
	/// - The slot is reused (generation mismatch).
	///
	/// @param bodyType Type of Rigidbody (see PhysicsTypes).
	/// @param position Initial world position.
	/// @param mass Amount of mass for the Rigidbody.
	/// @param friction Amount of friction for the Rigidbody.
	/// @param restitution Coefficient of restitution for the Rigidbody.
	/// @return RigidbodyHandle used to reference the object.
	///
	///  NOTE:
	/// Friction and restitution are not fully implemented yet.
	/// These parameters are currently ignored in the simulation.
	/// Planned for future updates.
    virtual RigidbodyHandle CreateRigidbody(BodyType bodyType, const Vec3& position,
		float mass = 1.0f, float friction = 0.6f, float restitution = 0.5f) = 0;

	/// Sets the half extents of a Rigidbody's Box.
	/// If the handle is invalid or the Rigidbody's type is not a Box, the call is ignored.
	virtual void SetRigidbodyBoxHalfExtents(RigidbodyHandle rbHandle, const Vec3& halfExtents) = 0;
	/// Sets the center of a Rigidbody's Box.
	/// If the handle is invalid or the Rigidbody's type is not a Box, the call is ignored.
	virtual void SetRigidbodyBoxCenter(RigidbodyHandle rbHandle, const Vec3& center) = 0;
	/// Sets the orientation of a Rigidbody's Box.
	/// If the handle is invalid or the Rigidbody's type is not a Box, the call is ignored.
	virtual void SetRigidbodyBoxOrientation(RigidbodyHandle rbHandle, const Mat3& orientation) = 0;
	/// Sets the radius of a Rigidbody's Sphere.
	/// If the handle is invalid or the Rigidbody's type is not a Sphere, the call is ignored.
	virtual void SetRigidbodySphereRadius(RigidbodyHandle rbHandle, const float radius) = 0;
	/// Sets the center of a Rigidbody's Sphere.
	/// If the handle is invalid or the Rigidbody's type is not a Sphere, the call is ignored.
	virtual void SetRigidbodySphereCenter(RigidbodyHandle rbHandle, const Vec3& center) = 0;
	
	/// Returns the current position of a Rigidbody.
	/// If the handle is invalid, returns Vec3(0.0f).
	virtual Vec3 GetRigidbodyPosition(RigidbodyHandle rbHandle) = 0;
	
	/// Adds a collision listener to the given RigidbodyHandle.
	///
	/// NOTE:
	/// The physics system does NOT take ownership of the listener.
	/// The caller is responsible for ensuring the listener remains valid.
	/// If the handle is invalid or the listener is null, the call is ignored.
	///
	/// @param rbHandle The handle of the Rigidbody to add the listener to.
	/// @param listener The collision listener to add to the given Rigidbody.
	virtual void AddCollisionListenerToRigidbody(RigidbodyHandle rbHandle, ICollisionListener* listener) = 0;	
	
	/// Adds a linear impulse to the given RigidbodyHandle.
	/// If the handle is invalid, the call is ignored.
	/// @param rbHandle The handle of the Rigidbody to add the impulse to.
	/// @param impulse The amount of impulse to add to the given Rigidbody.
	virtual void AddLinearImpulseToRigidbody(RigidbodyHandle rbHandle, const Vec3& impulse) = 0;
	
	/// Checks whether a handle is still valid.
	///
	/// A handle is valid if:
	/// - index is within bounds
	/// - slot is alive
	/// - generation matches
	virtual bool IsValidRigidbodyHandle(RigidbodyHandle rbHandle) = 0;

	/// Updates the physics simulation.
	/// @param frameTime Time elapsed since last frame (in seconds).
	///
	/// NOTE: 
	/// The physics simulations are currently being updated every frame.
	/// TODO:
	/// Implement a fixed update for physics (e.g. 1/60 sec.)
	virtual void Update(float frameTime) = 0;
};

extern "C"
{
	/// Creates an instance of the IPhysics interface for external use.
	///
	/// NOTE:
	/// The caller owns the returned pointer and must destroy it using DestroyPhysicsEngine.
	/// @return The created instance of IPhysics.
	PHYSICS_API IPhysics* GetPhysicsEngine();
	
	/// Destroys a given instance of the IPhysics interface.
	/// @param physics The instance of IPhysics to destroy.
	PHYSICS_API void DestroyPhysicsEngine(IPhysics* physics);
}