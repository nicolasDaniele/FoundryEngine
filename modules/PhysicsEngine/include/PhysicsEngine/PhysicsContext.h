#pragma once

#include <vector>
#include <unordered_map>
#include "PhysicsEngine/Collisions3D.h"

using OBB = CoreGeometry::OBB;

class Physics;
class Rigidbody;
class RigidbodyVolume;

class PhysicsContext
{
public:
	PhysicsContext(Physics* _physics);
	~PhysicsContext();
	void Update(float frameTime);
	void DetectCollisions();
	void SolveImpulses(float frameTime);
	void CorrectPositions();
	void GenerateCollisionEvents();
	void AddRigidbody(Rigidbody* body);
	void RemoveRigidbody(Rigidbody* body);
	void ClearRigidbodies();

private:
	Physics* physics = nullptr;
	std::vector<Rigidbody*> bodies;
	std::vector<Rigidbody*> colliders1;
	std::vector<Rigidbody*> colliders2;
	std::vector<CollisionData> collisions;

	std::unordered_map<CollisionKey, CollisionData, CollisionKeyHash> currentCollisions;
	std::unordered_map<CollisionKey, CollisionData, CollisionKeyHash> previousCollisions;

	float linearProjectionPercent;
	float penetrationSlack;
	int impulseIteration;

	void ApplyImpulses(RigidbodyVolume* body1, RigidbodyVolume* body2,
		const CollisionData& hitData, int contanctIndex, float frameTime);

	void NotifyCollisionEnter(const CollisionKey& key, const CollisionData& data);
	void NotifyCollisionStay(const CollisionKey& key, const CollisionData& data);
	void NotifyCollisionExit(const CollisionKey& key);
}; 