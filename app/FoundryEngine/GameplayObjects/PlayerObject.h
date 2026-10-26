#pragma once

#include "GameplayObject.h"

class PlayerObject : public GameplayObject
{
public:
	PlayerObject(RigidbodyHandle _body, MeshRendererHandle _renderer,
		IPhysics* _physics, IGraphics* _graphics);

	~PlayerObject() = default;

	bool IsGrounded() const;
	void Reset(const Vec3& spawnPosition);
	void Move(const Vec3& velocity);
	void Jump(float impulse);

	void OnCollisionEnter(RigidbodyHandle self, RigidbodyHandle other, const CollisionData& data) override;
	void OnCollisionStay(RigidbodyHandle self, RigidbodyHandle other, const CollisionData& data) override;
	void OnCollisionExit(RigidbodyHandle self, RigidbodyHandle other) override;

private:
	bool isGrounded = true;
	Vec3 lastGroundNormal = Vec3(0.0f, 1.0f, 0.0f);
	// Jump direction blend: 0 = pure contact normal, 1 = pure world up,
	// values in between mix the two (then renormalize - see Jump()).
	// Defaults to a straight average.
	float jumpUpBlend = 0.5f;

	void StoreGroundNormal(const Vec3& normal);
};
