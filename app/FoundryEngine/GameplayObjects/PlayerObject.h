#pragma once

#include "EngineInterfaces/PhysicsInterfaces.h"
#include "EngineInterfaces/IPhysics.h"
#include "EngineInterfaces/IGraphics.h"

class PlayerObject : public ICollisionListener
{
public:
	PlayerObject(RigidbodyHandle _body, MeshRendererHandle _renderer,
		IPhysics* _physics, IGraphics* _graphics);

	~PlayerObject() = default;

	bool IsGrounded() const;
	Vec3 GetPosition() const;
	Quaternion GetRotation() const;

	void Update(float frameTime);
	void Reset(const Vec3& spawnPosition);
	void Move(const Vec3& velocity);
	void Jump(float impulse);

	void OnCollisionEnter(RigidbodyHandle self, RigidbodyHandle other, const CollisionData& data) override;
	void OnCollisionStay(RigidbodyHandle self, RigidbodyHandle other, const CollisionData& data) override;
	void OnCollisionExit(RigidbodyHandle self, RigidbodyHandle other) override;

private:
	RigidbodyHandle body;
	MeshRendererHandle renderer;
	IPhysics* physics;
	IGraphics* graphics;
	bool isGrounded = true;
	Vec3 position = Vec3(0.0f);
	Quaternion rotation;
	Vec3 lastGroundNormal = Vec3(0.0f, 1.0f, 0.0f);
	// Jump direction blend: 0 = pure contact normal, 1 = pure world up,
	// values in between mix the two (then renormalize - see Jump()).
	// Defaults to a straight average.
	float jumpUpBlend = 0.5f;

	void StoreGroundNormal(const Vec3& normal);
};
