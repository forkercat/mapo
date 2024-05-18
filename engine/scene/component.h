//
// Created by Junhao Wang (@forkercat) on 4/29/24.
//

#pragma once

#include "core/core.h"

#include "engine/scene/scene_camera.h"
#include "engine/scene/scriptable.h"

namespace Mapo
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
			: model(m) { }

		Ref<Model> model{};
	};

	struct MaterialComponent
	{
		Vector3 color;
	};

	struct LightComponent
	{
		F32		intensity;
		Vector3 color;
	};

	struct CameraComponent
	{
		SceneCamera camera;
		bool		primary = true;
	};

	struct NativeScriptComponent
	{
		Scriptable* scriptable = nullptr;

		// Create the scriptable instance.
		Scriptable* (*InstantiateScript)();

		void (*DestroyScript)(NativeScriptComponent*);

		template <typename ScriptableType>
		void Bind()
		{
			// Create script lambda.
			InstantiateScript = []() {
				return static_cast<Scriptable*>(MP_NEW(ScriptableType));
			};

			DestroyScript = [](NativeScriptComponent* scriptComponent) {
				scriptComponent->scriptable->OnDestroy();

				MP_DELETE(scriptComponent->scriptable);
				scriptComponent->scriptable = nullptr;
			};
		}

		bool runInEditor = true;
	};

} // namespace Mapo
