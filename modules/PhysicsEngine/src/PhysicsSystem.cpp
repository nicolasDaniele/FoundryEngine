#include "PhysicsEngine/PhysicsSystem.h"
#include "PhysicsEngine/Rigidbody.h"
#include "PhysicsEngine/RigidbodyVolume.h"
#include "Core/Geometry3D.h"
#include "Core/MathDefinitions.h"
#include "Core/Matrices.h"

#include <iostream>

PhysicsSystem::PhysicsSystem()
{
	// @TODO: Pass in this values as parameters
	linearProjectionPercent = 0.4f;
	penetrationSlack = 0.01f;
	impulseIteration = 5;

	colliders1.reserve(100);
	colliders2.reserve(100);
	collisions.reserve(100);
}

PhysicsSystem::~PhysicsSystem()
{
	ClearRigidbodies();
	ClearConstraints();
}

void PhysicsSystem::Update(float frameTime)
{
	colliders1.clear();
	colliders2.clear();
	collisions.clear();

	for (int i = 0; i < bodies.size(); ++i)
		bodies[i]->ApplyGravityForce();

	for (int i = 0; i < bodies.size(); ++i)
	{
		bodies[i]->IntegrateVelocity(frameTime);
		RigidbodyVolume* rb = (RigidbodyVolume*)bodies[i];
		if (rb && rb->HasVolume())
			rb->SynchCollisionVolumes();
	}

	DetectCollisions();

	for (int k = 0; k < impulseIteration; ++k)
		SolveImpulses(frameTime);

	for (int i = 0; i < bodies.size(); ++i)
		bodies[i]->IntegratePosition(frameTime);

	CorrectPositions();

	for (int i = 0; i < bodies.size(); ++i)
	{
		RigidbodyVolume* rbv = (RigidbodyVolume*)bodies[i];
		if(rbv) rbv->ClearForces();
	}
}

void PhysicsSystem::DetectCollisions()
{
	for (int i = 0; i < bodies.size(); ++i)
	{
		for (int j = i; j < bodies.size(); ++j)
		{
			if (i == j)
				continue;

			CollisionData collision;
			ResetCollisionData(&collision);

			if (bodies[i]->HasVolume() &&
				bodies[j]->HasVolume())
			{
				RigidbodyVolume* rb1 = (RigidbodyVolume*)bodies[i];
				RigidbodyVolume* rb2 = (RigidbodyVolume*)bodies[j];

				if (rb1->type == RIGIDBODY_TYPE_BOX && rb2->type == RIGIDBODY_TYPE_BOX)
					collision = FindCollisionFeatures(rb1->GetBox(), rb2->GetBox());
				else if (rb1->type == RIGIDBODY_TYPE_BOX && rb2->type == RIGIDBODY_TYPE_SPHERE)
				{
					collision = FindCollisionFeatures(rb1->GetBox(), rb2->GetSphere());
				}
				else if (rb1->type == RIGIDBODY_TYPE_SPHERE && rb2->type == RIGIDBODY_TYPE_BOX)
				{
					collision = FindCollisionFeatures(rb2->GetBox(), rb1->GetSphere());
					collision.normal = collision.normal * -1.0f;
				}
				else if (rb1->type == RIGIDBODY_TYPE_SPHERE && rb2->type == RIGIDBODY_TYPE_SPHERE)
					collision = FindCollisionFeatures(rb1->GetSphere(), rb2->GetSphere());
			}

			if (collision.colliding)
			{
				colliders1.push_back(bodies[i]);
				colliders2.push_back(bodies[j]);
				collisions.push_back(collision);
			}
		}
	}
}

void PhysicsSystem::SolveImpulses(float frameTime)
{
	for (int i = 0; i < collisions.size(); ++i)
	{
		int jSize = collisions[i].contacts.size();
		for (int j = 0; j < jSize; ++j)
		{
			if (colliders1[i]->HasVolume()
				&& colliders2[i]->HasVolume())
			{
				RigidbodyVolume* rbv1 = (RigidbodyVolume*)colliders1[i];
				RigidbodyVolume* rbv2 = (RigidbodyVolume*)colliders2[i];
				ApplyImpulses(rbv1, rbv2, collisions[i], j, frameTime);
			}
		}
	}
}

void PhysicsSystem::CorrectPositions()
{
	for (int i = 0; i < collisions.size(); ++i)
	{
		if (!colliders1[i]->HasVolume()
			&& !colliders2[i]->HasVolume())
		{
			continue;
		}

		RigidbodyVolume* rb1 = (RigidbodyVolume*)colliders1[i];
		RigidbodyVolume* rb2 = (RigidbodyVolume*)colliders2[i];
		float totalMass = rb1->GetInvMass() + rb2->GetInvMass();

		if (totalMass == 0.0f)
			continue;

		if (rb1->GetInvMass() == 0.0f)
			rb2->SetPosition(rb2->GetPosition() + collisions[i].normal * collisions[i].depth);
		else if (rb2->GetInvMass() == 0.0f)
			rb1->SetPosition(rb1->GetPosition() - collisions[i].normal * collisions[i].depth);
		else
		{
			float totalMass = rb1->GetInvMass() + rb2->GetInvMass();
			Vec3 correction = collisions[i].normal * (collisions[i].depth / totalMass) * linearProjectionPercent;

			rb1->SetPosition(rb1->GetPosition() - correction * rb1->GetInvMass());
			rb2->SetPosition(rb2->GetPosition() + correction * rb2->GetInvMass());
		}

		rb1->SynchCollisionVolumes();
		rb2->SynchCollisionVolumes();
	}
}

void PhysicsSystem::AddRigidbody(Rigidbody* body)
{
	bodies.push_back(body);
}

void PhysicsSystem::ClearRigidbodies()
{
	bodies.clear();
}

void PhysicsSystem::AddConstraint(const OBB& constraint)
{
	constraints.push_back(constraint);
}

void PhysicsSystem::ClearConstraints()
{
	constraints.clear();
}

void PhysicsSystem::ApplyImpulses(RigidbodyVolume* body1, RigidbodyVolume* body2, const CollisionData& hitData, 
	int contanctIndex, float frameTime)
{
	float invMass1 = body1->GetInvMass();
	float invMass2 = body2->GetInvMass();
	float invMassSum = invMass1 + invMass2;

	if (invMassSum == 0.0f)
		return;

	Vec3 dir1 = hitData.contacts[contanctIndex] - body1->GetPosition();
	Vec3 dir2 = hitData.contacts[contanctIndex] - body2->GetPosition();

	Mat3 invTensor1 = body1->GetInvTensor();
	Mat3 invTensor2 = body2->GetInvTensor();

	Vec3 relativeVel = (body2->GetVelocity() + Cross(body2->GetAngularVelocity(), dir2)) - (body1->GetVelocity() + Cross(body1->GetAngularVelocity(), dir1));
	Vec3 hitNormal = hitData.normal;

	if (Dot(relativeVel, hitNormal) > 0.0f)
		return;

	float minRestitution = fminf(body1->GetRestitution(), body2->GetRestitution());

	// Baumgarte
	//float beta = 0.2f;
	//float baumgarte = (beta / frameTime) * fmaxf(0.0f, hitData.depth - penetrationSlack);

	//float numerator = (-(1.0f + minRestitution) * Dot(relativeVel, hitNormal)) + baumgarte;
	float numerator = (-(1.0f + minRestitution) * Dot(relativeVel, hitNormal));

	float d1 = invMassSum;
	Vec3 d2 = Cross(MultiplyMat3Vec3(invTensor1, Cross(dir1, hitNormal)), dir1);
	Vec3 d3 = Cross(MultiplyMat3Vec3(invTensor2, Cross(dir2, hitNormal)), dir2);
	float denominator = d1 + Dot(hitNormal, d2 + d3);

	float j = (denominator == 0.0f) ? 0.0f : numerator / denominator;

	if (hitData.contacts.size() > 0.0f && j != 0.0f)
		j /= (float)hitData.contacts.size();

	bool isBody1Static = CMP(body1->GetInvMass(), 0.0f);
	bool isBody2Static = CMP(body2->GetInvMass(), 0.0f);

	Vec3 impulse = hitNormal * j;
	if (!isBody1Static)
	{
		body1->SetVelocity(body1->GetVelocity() - impulse * invMass1);
		body1->SetAngularVelocity(body1->GetAngularVelocity() - MultiplyMat3Vec3(invTensor1, Cross(dir1, impulse)));
	}
	if (!isBody2Static)
	{
		body2->SetVelocity(body2->GetVelocity() + impulse * invMass2);
		body2->SetAngularVelocity(body2->GetAngularVelocity() + MultiplyMat3Vec3(invTensor2, Cross(dir2, impulse)));
	}

	Vec3 t = relativeVel - (hitNormal * Dot(relativeVel, hitNormal));
	if (MagnitudeSq(t) < 1e-6f)
		return;

	Normalize(t);

	numerator = -Dot(relativeVel, t);
	d1 = invMassSum;
	d2 = Cross(MultiplyMat3Vec3(invTensor1, Cross(dir1, t)), dir1);
	d3 = Cross(MultiplyMat3Vec3(invTensor2, Cross(dir2, t)), dir2);
	denominator = d1 + Dot(t, d2 + d3);

	if (denominator == 0.0f)
		return;

	float jt = numerator / denominator;
	
	if (hitData.contacts.size() > 0.0f && jt != 0.0f)
		jt /= (float)hitData.contacts.size();
	
	if (CMP(jt, 0.0f))
		return;

	float friction = sqrtf(body1->GetFriction() * body2->GetFriction());
	
	if (jt > j * friction)
		jt = j * friction;
	else if (jt < -j * friction)
		jt = -j * friction;

	Vec3 tangentImpuse = t * jt;

	if (!isBody1Static)
	{
		body1->SetVelocity(body1->GetVelocity() - tangentImpuse * invMass1);
		body1->SetAngularVelocity(body1->GetAngularVelocity() - MultiplyMat3Vec3(invTensor1, Cross(dir1, tangentImpuse)));
	}
	if (!isBody2Static)
	{
		body2->SetVelocity(body2->GetVelocity() + tangentImpuse * invMass2);
		body2->SetAngularVelocity(body2->GetAngularVelocity() + MultiplyMat3Vec3(invTensor2, Cross(dir2, tangentImpuse)));
	}
}