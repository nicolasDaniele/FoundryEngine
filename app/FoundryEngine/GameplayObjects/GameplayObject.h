#pragma once

#include "EngineInterfaces/PhysicsInterfaces.h"
#include "EngineInterfaces/IPhysics.h"
#include "EngineInterfaces/IGraphics.h"

class GameplayObject : public ICollisionListener
{
public:
	GameplayObject(RigidbodyHandle _body, MeshRendererHandle _renderer,
		IPhysics* _physics, IGraphics* _graphics);

	virtual ~GameplayObject() = default;

	Vec3 GetPosition() const;
	Quaternion GetRotation() const;

	// Syncs graphics (position + rotation) from the physics body. Shared by
	// every GameplayObject, since anything with a Rigidbody + MeshRenderer
	// needs this same sync every frame. Virtual so a derived class can
	// extend it (call GameplayObject::Update(frameTime) first, then add its
	// own per-frame logic) instead of having to duplicate the sync itself.
	virtual void Update(float frameTime);

	// Empty default hooks, same pattern as Rigidbody's own
	// NotifyCollisionEnter/Stay/Exit - a derived class only overrides the
	// event it actually cares about.
	void OnCollisionEnter(RigidbodyHandle self, RigidbodyHandle other, const CollisionData& data) override { }
	void OnCollisionStay(RigidbodyHandle self, RigidbodyHandle other, const CollisionData& data) override { }
	void OnCollisionExit(RigidbodyHandle self, RigidbodyHandle other) override { }

protected:
	RigidbodyHandle body;
	MeshRendererHandle renderer;
	IPhysics* physics;
	IGraphics* graphics;
	Vec3 position = Vec3(0.0f);
	Quaternion rotation;
};
