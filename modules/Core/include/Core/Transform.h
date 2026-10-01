#pragma once

#include "Core/Vectors.h"
#include "Core/Matrices.h"
#include "Core/Quaternions.h"

namespace CoreMath
{
	/// General-purpose spatial transform (position + rotation + scale),
	/// meant to be used wherever a module needs to describe or pass around
	/// a full 3D placement. MeshRenderer already exposes matching
	/// SetPosition/SetRotation/SetScale individually; Graphics offers
	/// SetMeshRendererTransform/GetMeshRendererTransform as a convenience
	/// that sets/reads all three through a single Transform instead.
	///
	/// Deliberately NOT used by RigidbodyVolume/Physics: a sphere collider
	/// has no meaningful rotation or non-uniform scale (only a radius), so
	/// a single "create from Transform" call wouldn't actually simplify
	/// anything there - box-specific properties (half extents, initial
	/// orientation) are already set through their own dedicated calls after
	/// CreateRigidbody, which is the right pattern to keep for that module.
	struct Transform
	{
		Vec3 position = Vec3(0.0f);
		Quaternion rotation; // identity by default
		Vec3 scale = Vec3(1.0f);

		Transform() = default;
		Transform(const Vec3& _position, const Quaternion& _rotation, const Vec3& _scale)
			: position(_position), rotation(_rotation), scale(_scale) { }
	};

	// Builds a model matrix from a Transform, using the same T * R * S
	// composition order MeshRenderer::Draw already applies by hand.
	inline Mat4 ToMat4(const Transform& transform)
	{
		return Translation(transform.position) * ToMat4(transform.rotation) * Scale(transform.scale);
	}
}
