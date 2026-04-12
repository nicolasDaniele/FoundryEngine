#pragma once

#include <vector>
#include "PhysicsEngine/Collisions3D.h"

using OBB = CoreGeometry::OBB;

class Rigidbody;
class RigidbodyVolume;

class PhysicsSystem
{
public:
	PhysicsSystem();
	~PhysicsSystem();
	void Update(float frameTime);
	void DetectCollisions();
	void SolveImpulses(float frameTime);
	void CorrectPositions();
	void AddRigidbody(Rigidbody* body);
	void ClearRigidbodies();
	void AddConstraint(const OBB& constraint);
	void ClearConstraints();

private:
	std::vector<Rigidbody*> bodies;
	std::vector<OBB> constraints;
	std::vector<Rigidbody*> colliders1;
	std::vector<Rigidbody*> colliders2;
	std::vector<CollisionData> collisions;

	float linearProjectionPercent;
	float penetrationSlack;
	int impulseIteration;

	void ApplyImpulses(RigidbodyVolume* body1, RigidbodyVolume* body2,
		const CollisionData& hitData, int contanctIndex, float frameTime);
}; 