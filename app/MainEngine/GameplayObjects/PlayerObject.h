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

	void OnCollisionEnter(RigidbodyHandle self, RigidbodyHandle other, const CollisionData& data) override
	{
		isGrounded = true;
		//std::cout << "Enter Collision" << std::endl;
	}

	void OnCollisionStay(RigidbodyHandle self, RigidbodyHandle other, const CollisionData& data) override
	{
		//std::cout << "Stay Collision" << std::endl;
	}

	void OnCollisionExit(RigidbodyHandle self, RigidbodyHandle other) override
	{
		isGrounded = false;
		//std::cout << "Exit Collision" << std::endl;
	}

private:
	RigidbodyHandle body;
	MeshRendererHandle renderer;
	IPhysics* physics;
	IGraphics* graphics;
	bool isGrounded = true;
	Vec3 position = Vec3(0.0f);
};