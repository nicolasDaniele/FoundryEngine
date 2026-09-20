#pragma once

#ifdef PHYSICSENGINE_EXPORTS
	#define PHYSICS_API __declspec(dllexport)
#else
	#define PHYSICS_API __declspec(dllimport)
#endif

#include "Core/Vectors.h"
#include "Core/Quaternions.h"
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
using Quaternion = CoreMath::Quaternion;

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
	/// @param mass Amount of mass for the Rigidbody. Can be changed later via SetRigidbodyMass.
	/// @param friction Amount of friction for the Rigidbody. Can be changed later via SetRigidbodyFriction.
	/// @param restitution Coefficient of restitution for the Rigidbody. Can be changed later via SetRigidbodyRestitution.
	/// @return RigidbodyHandle used to reference the object.
    virtual RigidbodyHandle CreateRigidbody(BodyType bodyType, const Vec3& position,
		float mass = 1.0f, float friction = 0.6f, float restitution = 0.5f) = 0;

	/// Sets the half extents of a Rigidbody's Box collider.
	/// If the handle is invalid or the Rigidbody's type is not a Box, the call is ignored.
	virtual void SetRigidbodyBoxHalfExtents(RigidbodyHandle rbHandle, const Vec3& halfExtents) = 0;
	/// Sets the center of a Rigidbody's Box collider.
	/// If the handle is invalid or the Rigidbody's type is not a Box, the call is ignored.
	virtual void SetRigidbodyBoxCenter(RigidbodyHandle rbHandle, const Vec3& center) = 0;
	/// Sets the orientation of a Rigidbody's Box collider.
	/// If the handle is invalid or the Rigidbody's type is not a Box, the call is ignored.
	virtual void SetRigidbodyBoxOrientation(RigidbodyHandle rbHandle, const Mat3& orientation) = 0;
	/// Sets the radius of a Rigidbody's Sphere collider.
	/// If the handle is invalid or the Rigidbody's type is not a Sphere, the call is ignored.
	virtual void SetRigidbodySphereRadius(RigidbodyHandle rbHandle, const float radius) = 0;
	/// Sets the center of a Rigidbody's Sphere collider.
	/// If the handle is invalid or the Rigidbody's type is not a Sphere, the call is ignored.
	virtual void SetRigidbodySphereCenter(RigidbodyHandle rbHandle, const Vec3& center) = 0;
	/// Sets a Sphere Rigidbody's rolling resistance: a strength value
	/// (0.0 = none, 1.0 = fully stops rotation every frame) applied to
	/// angular velocity only while the Rigidbody is touching something.
	/// Distinct from friction (which only opposes slip while it's happening)
	/// and from angular damping (which acts unconditionally, in the air too).
	/// Defaults to 0.0 (a sphere rolls without ever losing angular velocity
	/// from this) until explicitly set. Values outside [0,1] are clamped.
	/// If the handle is invalid or the Rigidbody's type is not a Sphere, the call is ignored.
	virtual void SetRigidbodySphereRollingResistance(RigidbodyHandle rbHandle, float rollingResistance) = 0;
	
	/// Returns the current position of a Rigidbody.
	/// If the handle is invalid, returns Vec3(0.0f).
	virtual Vec3 GetRigidbodyPosition(RigidbodyHandle rbHandle) = 0;
	/// Set the position of a Rigidbody.
	/// If the handle is invalid, the call is ignored. 
	virtual void SetRigidbodyPosition(RigidbodyHandle rbHandle, const Vec3& position) = 0;
	/// Set the linear velocity of a Rigidbody.
	/// If the handle is invalid, the call is ignored.
	virtual void SetRigidbodyLinearVelocity(RigidbodyHandle rbHandle, const Vec3& velocity) = 0;

	/// Sets a Rigidbody's mass. Recomputes its local inertia tensor, same as
	/// changing a Box's half extents or a Sphere's radius does.
	/// If the handle is invalid, the call is ignored.
	virtual void SetRigidbodyMass(RigidbodyHandle rbHandle, float mass) = 0;
	/// Sets a Rigidbody's friction coefficient (used in Coulomb friction -
	/// only opposes slip at a contact point; see SetRigidbodySphereRollingResistance
	/// for what keeps a sphere from rolling forever once it stops slipping).
	/// If the handle is invalid, the call is ignored.
	virtual void SetRigidbodyFriction(RigidbodyHandle rbHandle, float friction) = 0;
	/// Sets a Rigidbody's restitution (bounciness) coefficient.
	/// If the handle is invalid, the call is ignored.
	virtual void SetRigidbodyRestitution(RigidbodyHandle rbHandle, float restitution) = 0;
	/// Sets a Rigidbody's linear damping: a strength value (0.0 = none,
	/// 1.0 = fully stops linear motion every frame). Applied every frame,
	/// regardless of contact - this also competes with gravity every frame,
	/// so it's meant as a mild numerical stabilizer, not a "drag" dial.
	/// Values outside [0,1] are clamped.
	/// If the handle is invalid, the call is ignored.
	virtual void SetRigidbodyDamping(RigidbodyHandle rbHandle, float damping) = 0;
	/// Sets a Rigidbody's angular damping: a strength value (0.0 = none,
	/// 1.0 = fully stops rotation every frame). Applied every frame
	/// regardless of contact, unlike rolling resistance. Values outside
	/// [0,1] are clamped.
	/// If the handle is invalid, the call is ignored.
	virtual void SetRigidbodyAngularDamping(RigidbodyHandle rbHandle, float angularDamping) = 0;

	/// Returns the current orientation of a Rigidbody.
	/// If the handle is invalid, returns the identity quaternion.
	virtual Quaternion GetRigidbodyOrientation(RigidbodyHandle rbHandle) = 0;
	/// Sets the orientation of a Rigidbody. Does not immediately resync the
	/// collision volume or the world inertia tensor - both catch up on the
	/// next Update(), same as SetRigidbodyPosition already behaves.
	/// If the handle is invalid, the call is ignored.
	virtual void SetRigidbodyOrientation(RigidbodyHandle rbHandle, const Quaternion& orientation) = 0;
	/// Sets the angular velocity of a Rigidbody.
	/// If the handle is invalid, the call is ignored.
	virtual void SetRigidbodyAngularVelocity(RigidbodyHandle rbHandle, const Vec3& angularVelocity) = 0;
	/// Adds a continuous torque to a Rigidbody, accumulated until the next
	/// Update() call (same accumulate-then-clear lifecycle as
	/// AddLinearImpulseToRigidbody's underlying force accumulation).
	/// If the handle is invalid, the call is ignored.
	virtual void AddTorqueToRigidbody(RigidbodyHandle rbHandle, const Vec3& torque) = 0;
	/// Adds an instantaneous rotational impulse to the given RigidbodyHandle,
	/// as if it were struck at `point` (in world space) with `impulse`.
	/// Not part of the automatic collision-resolution cycle - this is a
	/// standalone tool for gameplay code to call directly (e.g. an explosion,
	/// a power-up, a scripted hit).
	/// If the handle is invalid, the call is ignored.
	/// @param rbHandle The handle of the Rigidbody to add the impulse to.
	/// @param point World-space point where the impulse is applied.
	/// @param impulse The impulse vector to apply at that point.
	virtual void AddRotationalImpulseToRigidbody(RigidbodyHandle rbHandle, const Vec3& point, const Vec3& impulse) = 0;
	
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
