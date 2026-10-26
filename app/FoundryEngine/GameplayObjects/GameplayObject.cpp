#include "GameplayObject.h"

GameplayObject::GameplayObject(RigidbodyHandle _body, MeshRendererHandle _renderer,
		IPhysics* _physics, IGraphics* _graphics)
	: body(_body), renderer(_renderer), physics(_physics), graphics(_graphics)
{
	physics->AddCollisionListenerToRigidbody(_body, this);
}

Vec3 GameplayObject::GetPosition() const
{
	return position;
}

Quaternion GameplayObject::GetRotation() const
{
	return rotation;
}

void GameplayObject::Update(float frameTime)
{
	position = physics->GetRigidbodyPosition(body);
	rotation = physics->GetRigidbodyOrientation(body);

	graphics->UpdateMeshRendererPosition(renderer, position);
	graphics->SetMeshRendererRotation(renderer, rotation);
}
