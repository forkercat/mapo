//
// Created by Junhao Wang (@forkercat) on 4/29/24.
//

#include "component.h"

#include "core/core.h"

namespace Mapo
{
	// Matrix corresponds to Translate * Ry * Rx * Rz * Scale
	// Rotations correspond to Tait-bryan angles of Y(1), X(2), Z(3)
	// https://en.wikipedia.org/wiki/Euler_angles#Rotation_matrix
	Matrix4 TransformComponent::GetTransform()
	{
		const float c3 = MathOp::Cos(rotation.z);
		const float s3 = MathOp::Sin(rotation.z);
		const float c2 = MathOp::Cos(rotation.x);
		const float s2 = MathOp::Sin(rotation.x);
		const float c1 = MathOp::Cos(rotation.y);
		const float s1 = MathOp::Sin(rotation.y);
		return Matrix4{
			{
				scale.x * (c1 * c3 + s1 * s2 * s3),
				scale.x * (c2 * s3),
				scale.x * (c1 * s2 * s3 - c3 * s1),
				0.0f,
			},
			{
				scale.y * (c3 * s1 * s2 - c1 * s3),
				scale.y * (c2 * c3),
				scale.y * (c1 * c3 * s2 + s1 * s3),
				0.0f,
			},
			{
				scale.z * (c2 * s1),
				scale.z * (-s2),
				scale.z * (c1 * c2),
				0.0f,
			},
			{ translation.x, translation.y, translation.z, 1.0f }
		};
	}

	Matrix3 TransformComponent::GetNormalMatrix()
	{
		const float c3 = MathOp::Cos(rotation.z);
		const float s3 = MathOp::Sin(rotation.z);
		const float c2 = MathOp::Cos(rotation.x);
		const float s2 = MathOp::Sin(rotation.x);
		const float c1 = MathOp::Cos(rotation.y);
		const float s1 = MathOp::Sin(rotation.y);
		const Vector3 invScale = 1.0f / scale;

		return Matrix3{
			{
				invScale.x * (c1 * c3 + s1 * s2 * s3),
				invScale.x * (c2 * s3),
				invScale.x * (c1 * s2 * s3 - c3 * s1),
			},
			{
				invScale.y * (c3 * s1 * s2 - c1 * s3),
				invScale.y * (c2 * c3),
				invScale.y * (c1 * c3 * s2 + s1 * s3),
			},
			{
				invScale.z * (c2 * s1),
				invScale.z * (-s2),
				invScale.z * (c1 * c2),
			},
		};
	}

} // namespace Mapo
