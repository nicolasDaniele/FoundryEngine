#include "RotatingPlatform.h"
#include "PhysicsEngine/Collisions3D.h" // full CollisionData definition (contacts, normal)
#include "Core/Utils.h"

RotatingPlatform::RotatingPlatform(RigidbodyHandle _body, MeshRendererHandle _renderer,
		IPhysics* _physics, IGraphics* _graphics,
		Vec3 _spinAxis, float _spinGain, float _maxSpinSpeed)
	: GameplayObject(_body, _renderer, _physics, _graphics),
	  spinAxis(CoreMath::Normalized(_spinAxis)), spinGain(_spinGain), maxSpinSpeed(_maxSpinSpeed)
{
}

void RotatingPlatform::OnCollisionEnter(RigidbodyHandle self, RigidbodyHandle other, const CollisionData& data)
{
	if (data.contacts.empty())
		return;

	// Contact point: average of the reported contacts (a sphere-box hit
	// reports one, box-box can report several).
	Vec3 contact = Vec3(0.0f);
	for (const Vec3& c : data.contacts)
		contact = contact + c;
	contact = contact * (1.0f / (float)data.contacts.size());

	Vec3 platformPos = physics->GetRigidbodyPosition(body);
	Vec3 otherPos = physics->GetRigidbodyPosition(other);

	// Lever arm: from the platform's center to where it was hit.
	Vec3 leverArm = contact - platformPos;

	// CollisionData::normal doesn't have a fixed "toward me" direction (it
	// depends on internal body ordering), so orient it using the actual
	// positions: make it point from the platform toward the other body.
	// The force the other body applies to this platform is the opposite way
	// (pushing into it).
	Vec3 normal = data.normal;
	if (CoreMath::Dot(normal, otherPos - platformPos) < 0.0f)
		normal = normal * -1.0f;
	Vec3 force = normal * -1.0f;

	// Lever torque (r x F). Its component along the spin axis gives both the
	// direction (which side was hit) and a magnitude proportional to how far
	// from the pivot the hit landed.
	Vec3 torque = CoreMath::Cross(leverArm, force);
	float spin = CoreMath::Dot(torque, spinAxis) * spinGain;
	Utils::Clamp(spin, -maxSpinSpeed, maxSpinSpeed);

	// Mass 0 => zero inverse inertia tensor, so impulse/torque APIs do
	// nothing here; assigning angular velocity directly bypasses that.
	physics->SetRigidbodyAngularVelocity(body, spinAxis * spin);
}
