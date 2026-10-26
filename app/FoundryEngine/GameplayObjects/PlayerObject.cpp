#include "PlayerObject.h"
#include "PhysicsEngine/Collisions3D.h"

PlayerObject::PlayerObject(RigidbodyHandle _body, MeshRendererHandle _renderer,
		IPhysics* _physics, IGraphics* _graphics)
	: GameplayObject(_body, _renderer, _physics, _graphics)
{
}

bool PlayerObject::IsGrounded() const
{
	return isGrounded;
}

void PlayerObject::Reset(const Vec3& spawnPosition)
{
	position = spawnPosition;
	rotation = Quaternion();

	physics->SetRigidbodyPosition(body, spawnPosition);
	physics->SetRigidbodyLinearVelocity(body, Vec3(0.0f));
	physics->SetRigidbodyOrientation(body, rotation);
	physics->SetRigidbodyAngularVelocity(body, Vec3(0.0f));

	graphics->UpdateMeshRendererPosition(renderer, spawnPosition);
	graphics->SetMeshRendererRotation(renderer, rotation);

	isGrounded = false;
	lastGroundNormal = Vec3(0.0f, 1.0f, 0.0f);
}

void PlayerObject::Move(const Vec3& velocity)
{
	physics->AddLinearImpulseToRigidbody(body, velocity);
}

void PlayerObject::Jump(float impulse)
{
	if (!isGrounded)
		return;

	Vec3 worldUp = Vec3(0.0f, 1.0f, 0.0f);

	// Blend world-up with the actual contact normal, then renormalize so
	// jump strength stays consistent (== impulse) regardless of platform
	// tilt - only the direction shifts toward/away from the slope, never
	// the magnitude.
	Vec3 blendedUp = worldUp * jumpUpBlend + lastGroundNormal * (1.0f - jumpUpBlend);
	Vec3 jumpDirection = CoreMath::Normalized(blendedUp);

	physics->AddLinearImpulseToRigidbody(body, jumpDirection * impulse);
}


// COLLISION EVENTS
void PlayerObject::OnCollisionEnter(RigidbodyHandle self, RigidbodyHandle other, const CollisionData& data)
{
	isGrounded = true;
	StoreGroundNormal(data.normal);
}

void PlayerObject::OnCollisionStay(RigidbodyHandle self, RigidbodyHandle other, const CollisionData& data)
{
	StoreGroundNormal(data.normal);
}

void PlayerObject::OnCollisionExit(RigidbodyHandle self, RigidbodyHandle other)
{
	isGrounded = false;
}

void PlayerObject::StoreGroundNormal(const Vec3& normal)
{
	lastGroundNormal = (normal.y >= 0.0f) ? normal : normal * -1.0f;
}
