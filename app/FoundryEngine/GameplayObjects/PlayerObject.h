#pragma once

#include "EngineInterfaces/PhysicsInterfaces.h"
#include "EngineInterfaces/IPhysics.h"
#include "EngineInterfaces/IGraphics.h"

class PlayerObject : public ICollisionListener
{
public:
	PlayerObject(RigidbodyHandle _body, MeshRendererHandle _renderer,
	             IPhysics* _physics, IGraphics* _graphics)
		: body(_body), renderer(_renderer),
		  physics(_physics), graphics(_graphics)
	{
		physics->AddCollisionListenerToRigidbody(_body, this);
	}

	~PlayerObject() = default;

	bool IsGrounded() const { return isGrounded; }
	Vec3 GetPosition() const { return position; }

	void Move(const Vec3& velocity)
	{
		physics->AddLinearImpulseToRigidbody(body, velocity);
	}

	void Jump(float impulse)
	{
		if (isGrounded)
			physics->AddLinearImpulseToRigidbody(body, Vec3(0.0f, impulse, 0.0f));
	}

	void Update(float frameTime)
	{
		position = physics->GetRigidbodyPosition(body);
		graphics->UpdateMeshRendererPosition(renderer, position);
	}

	void Reset(const Vec3& spawnPosition)
	{
		position = spawnPosition;

		physics->SetRigidbodyPosition(body, spawnPosition);
		physics->SetRigidbodyLinearVelocity(body, Vec3(0.0f));

		graphics->UpdateMeshRendererPosition(renderer, spawnPosition);

		isGrounded = false;
	}

	void OnCollisionEnter(RigidbodyHandle self, RigidbodyHandle other, const CollisionData& data) override
	{
		isGrounded = true;
	}

	void OnCollisionStay(RigidbodyHandle self, RigidbodyHandle other, const CollisionData& data) override
	{
	}

	void OnCollisionExit(RigidbodyHandle self, RigidbodyHandle other) override
	{
		isGrounded = false;
	}

private:
	RigidbodyHandle body;
	MeshRendererHandle renderer;
	IPhysics* physics;
	IGraphics* graphics;
	bool isGrounded = true;
	Vec3 position = Vec3(0.0f);
};