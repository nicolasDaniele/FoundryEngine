#pragma once

#include "PhysicsEngine/PhysicsApi.h"
#include "EngineInterfaces/PhysicsInterfaces.h"

class MeshRenderer;
class RigidbodyVolume;

class PlayerObject : public ICollisionListener
{
public:
	PlayerObject(RigidbodyVolume* _body, MeshRenderer* _renderer, float _moveSpeed, float _jumpImpulse)
		: body(_body), renderer(_renderer), moveSpeed(_moveSpeed), jumpImpulse(_jumpImpulse) 
	{
		AddCollisionListenerToRigidbody(body, this);
	}

	bool IsGrounded() const { return isGrounded; }
	Vec3 GetPosition() const { return position; }

	void Move(const Vec3& velocity)
	{
		AddLinearImpulseToRigidbody((RigidbodyVolume*)body, velocity);
	}

	void Jump(float _impulse)
	{
		if (isGrounded)
			AddLinearImpulseToRigidbody((RigidbodyVolume*)body, Vec3(0.0f, _impulse, 0.0f));
	}

	void Update(float frameTime)
	{
		position = GetRigidbodyPosition((RigidbodyVolume*)body);
		UpdateMeshRendererPosition(renderer, position);
	}

	void OnCollisionEnter(Rigidbody* self, Rigidbody* other, const CollisionData& data) override
	{
		isGrounded = true;
	}

	void OnCollisionStay(Rigidbody* self, Rigidbody* other, const CollisionData& data) override
	{
	}

	void OnCollisionExit(Rigidbody* self, Rigidbody* other) override
	{
		isGrounded = false;
	}

private:
	RigidbodyVolume* body;
	MeshRenderer* renderer;
	bool isGrounded = true;

	Vec3 position = Vec3(0.0f, 0.0f, 0.0f);

	float moveSpeed = 10.0f;
	float jumpImpulse = 5.0f;
};