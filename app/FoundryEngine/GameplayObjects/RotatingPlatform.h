#pragma once

#include "GameplayObject.h"

class RotatingPlatform : public GameplayObject
{
public:
	// spinAxis: world-space axis this platform pivots around (normalized
	// internally). Meant to be a HORIZONTAL axis (e.g. X or Z): the spin
	// comes from the lever torque of the landing (see OnCollisionEnter), and
	// a purely vertical landing produces no torque around a vertical axis.
	// spinGain: angular speed (rad/s) per unit of lever arm, i.e. a landing
	// 2 units from the pivot line spins at 2 * spinGain.
	// maxSpinSpeed: clamp (rad/s) so landing on the far edge can't spin the
	// platform absurdly fast.
	RotatingPlatform(RigidbodyHandle _body, MeshRendererHandle _renderer,
		IPhysics* _physics, IGraphics* _graphics,
		Vec3 spinAxis = Vec3(1.0f, 0.0f, 0.0f), float spinGain = 1.0f,
		float maxSpinSpeed = 6.0f);

	~RotatingPlatform() = default;

	void OnCollisionEnter(RigidbodyHandle self, RigidbodyHandle other, const CollisionData& data) override;

private:
	Vec3 spinAxis;
	float spinGain;
	float maxSpinSpeed;
};
