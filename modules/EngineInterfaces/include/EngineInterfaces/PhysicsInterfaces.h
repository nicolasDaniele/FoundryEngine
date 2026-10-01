#pragma once

#include <vector>
#include "Core/Vectors.h"

using Vec3 = CoreMath::Vec3;

struct RigidbodyHandle;
//struct CollisionData;

typedef struct CollisionData
{
	bool colliding;
	Vec3 normal;
	float depth;
	std::vector<Vec3> contacts;
} CollisionData;

/// Interface for receiving collision events.
/// Implemented by gameplay objects.
class ICollisionListener {
public:
    virtual void OnCollisionEnter(RigidbodyHandle self, RigidbodyHandle other, const CollisionData&) = 0;
    virtual void OnCollisionStay(RigidbodyHandle self, RigidbodyHandle other, const CollisionData&) = 0;
    virtual void OnCollisionExit(RigidbodyHandle self, RigidbodyHandle other) = 0;
};