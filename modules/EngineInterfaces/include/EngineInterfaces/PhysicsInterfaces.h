#pragma once

class Rigidbody;
struct CollisionData;

class ICollisionListener {
public:
    virtual void OnCollisionEnter(Rigidbody* self, Rigidbody* other, const CollisionData&) = 0;
    virtual void OnCollisionStay(Rigidbody* self, Rigidbody* other, const CollisionData&) = 0;
    virtual void OnCollisionExit(Rigidbody* self, Rigidbody* other) = 0;
};