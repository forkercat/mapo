//
// Created by Junhao Wang (@forkercat) on 4/29/24.
//

#pragma once

#include "core/core.h"

#include "engine/scene/scene_camera.h"
#include "engine/scene/scriptable.h"

#include <IconFontCppHeaders/IconsFontAwesome5.h>

#define MP_COMPONENT_NAME(name)              \
	static const String& GetName()           \
	{                                        \
		static String componentName{ name }; \
		return componentName;                \
	}

#define MP_COMPONENT_ICON(name)     \
	static const String& GetIcon()  \
	{                               \
		static String icon{ name }; \
		return icon;                \
	}

namespace Mapo
{
	class Model;

	struct Component
	{
		virtual ~Component() = default;

		bool enabled{ true };
	};

	struct InfoComponent : public Component
	{
		String objectName{};

		MP_COMPONENT_NAME("Info");
		MP_COMPONENT_ICON(ICON_FA_INFO);
	};

	struct TransformComponent : public Component
	{
		Vector3 translation{};
		Vector3 rotation{};
		Vector3 scale{ 1.0f, 1.0f, 1.0f };

		Matrix4 GetTransformMatrix();

		Matrix3 GetNormalMatrix();

		MP_COMPONENT_NAME("Transform");
		MP_COMPONENT_ICON(ICON_FA_LOCATION_ARROW);
	};

	struct MeshComponent : public Component
	{
		MeshComponent(Ref<Model> m)
			: model(m) { }

		Ref<Model> model{};

		MP_COMPONENT_NAME("Mesh");
		MP_COMPONENT_ICON(ICON_FA_VECTOR_SQUARE);
	};

	struct MaterialComponent : public Component
	{
		Vector3 color;

		MP_COMPONENT_NAME("Material");
		MP_COMPONENT_ICON(ICON_FA_BRUSH);
	};

	struct LightComponent : public Component
	{
		F32		intensity;
		Vector3 color;

		MP_COMPONENT_NAME("Light");
		MP_COMPONENT_ICON(ICON_FA_LIGHTBULB);
	};

	struct CameraComponent : public Component
	{
		SceneCamera camera;
		bool		primary = true;

		MP_COMPONENT_NAME("Camera");
		MP_COMPONENT_ICON(ICON_FA_CAMERA);
	};

	struct NativeScriptComponent : public Component
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

		MP_COMPONENT_NAME("Script");
		MP_COMPONENT_ICON(ICON_FA_CODE);
	};

} // namespace Mapo
