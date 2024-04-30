//
// Created by Junhao Wang (@forkercat) on 4/29/24.
//

#pragma once

#include "core/core.h"

namespace mapo
{
	class Model;

	struct TagComponent
	{
		String tag{};
	};

	struct TransformComponent
	{
		Vector3 translation{};
		Vector3 rotation{};
		Vector3 scale{ 1.0f, 1.0f, 1.0f };

		Matrix4 GetTransform();

		Matrix3 GetNormalMatrix();
	};

	struct MeshComponent
	{
		MeshComponent(Ref<Model> m)
			: model(m) {}

		Ref<Model> model{};
	};

	struct MaterialComponent
	{
		Vector3 color;
	};

	struct LightComponent
	{
		F32 intensity;
		Vector3 color;
	};

} // namespace mapo
