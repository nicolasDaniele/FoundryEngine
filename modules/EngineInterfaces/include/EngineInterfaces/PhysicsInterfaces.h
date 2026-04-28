#pragma once

struct RigidbodyHandle;
struct CollisionData;

class ICollisionListener {
public:
    virtual void OnCollisionEnter(RigidbodyHandle self, RigidbodyHandle other, const CollisionData&) = 0;
    virtual void OnCollisionStay(RigidbodyHandle self, RigidbodyHandle other, const CollisionData&) = 0;
    virtual void OnCollisionExit(RigidbodyHandle self, RigidbodyHandle other) = 0;
};