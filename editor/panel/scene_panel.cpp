//
// Created by Junhao Wang (@forkercat) on 5/18/24.
//

#include "scene_panel.h"

#include "engine/application.h"
#include "engine/window.h"

#include "engine/model.h"

#include "engine/input/input.h"
#include "engine/ui/imgui_utils.h"

#include "engine/renderer/render_context.h"
#include "engine/renderer/renderer.h"

#include "engine/scene/editor_camera.h"
#include "engine/scene/game_object.h"
#include "engine/scene/component.h"

#include <imgui/imgui.h>
#include <IconFontCppHeaders/IconsFontAwesome5.h>

namespace Mapo
{
	static const String IconNameShapes{ ICON_FA_CUBE };
	static const String IconNameLight{ ICON_FA_LIGHTBULB };

	ScenePanel::ScenePanel()
		: Panel("Scene")
	{
	}

	ScenePanel::ScenePanel(Ref<Scene> scene)
		: Panel("Scene")
	{
		m_scene = scene;
	}

	void ScenePanel::SetContext(Ref<Scene> context)
	{
		m_scene = context;
		m_selectedGameObject = {};
	}

	void ScenePanel::OnImGuiRender(EditorCamera& camera)
	{
		ImGuiWindowFlags flags = ImGuiWindowFlags_None;

		ImGui::SetNextWindowSize(ImVec2(230.0f, 0.0f));

		static const String panelName = ICON_NAME2(ICON_FA_SITEMAP, GetPanelName());

		ImGui::Begin(panelName.c_str(), nullptr, flags);

		DrawHierarchy(camera);
		DrawSelectedGameObject();

		ImGui::End();
	}

	void ScenePanel::DrawHierarchy(EditorCamera& camera)
	{
		Renderer& renderer = RenderContext::GetRenderer();

		// Left click on empty space: De-select game object.
		// if (ImGui::IsMouseClicked(ImGuiMouseButton_Left, false) && ImGui::IsWindowHovered(0) && !ImGui::IsAnyItemHovered())
		if (Input::IsKeyPressed(Key::LeftSuper) && ImGui::IsMouseClicked(ImGuiMouseButton_Left, false) &&
			ImGui::IsWindowHovered(0))
		{
			m_selectedGameObject = {};
		}

		// Right click on empty space: Popup for creation.
		if (ImGui::BeginPopupContextWindow(nullptr, ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems))
		{
			DrawGameObjectCreationMenu();
			ImGui::EndPopup();
		}

		// Scene
		ImGui::SeparatorText(ICON_NAME(ICON_FA_CAMERA, "Editor Camera"));
		ImGui::DragFloat3("Position", GLM_PTR(camera.GetPosition()));

		if (ImGui::SliderFloat("Fov", &camera.GetFov(), 10.0f, 120.0f, "%.1f"))
		{
			camera.UpdateProjection();
		}

		ImGui::ColorEdit3("Clear color", GLM_PTR(renderer.ClearColor()));

		// Game Objects
		std::vector<GameObject> gameObjectList = m_scene->GetGameObjects();

		// ImGui::NewLine();
		ImGui::SeparatorText(ICON_NAME(ICON_FA_CUBES, "Game Objects"));

		static F32 indent = 4.0f;
		ImGui::Unindent(indent);

		for (auto& gameObject : gameObjectList)
		{
			DrawGameObjectNode(gameObject, camera);
		}

		ImGui::Indent(indent);
	}

	void ScenePanel::DrawGameObjectNode(GameObject& gameObject, EditorCamera& camera)
	{
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8, 6));
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 4));

		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_OpenOnArrow;

		// Checks if this game object has been selected.
		flags |= (gameObject == m_selectedGameObject) ? ImGuiTreeNodeFlags_Selected : ImGuiTreeNodeFlags_None;

		const bool hasLight = gameObject.HasComponent<LightComponent>();

		String name = ICON_NAME2(hasLight ? IconNameLight : IconNameShapes, gameObject.GetName());

		std::stringstream ss;
		ss << name << "##" << (U32)gameObject; // Name + ##id to prevent conflict!

		bool isTreeOpened = ImGui::TreeNodeEx(ss.str().c_str(), flags);

		// Handles left-click selection.
		if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
		{
			m_selectedGameObject = gameObject;

			// Focus on the selected object. TODO: Duplicate code!
			if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
			{
				if (m_selectedGameObject.IsValid())
				{
					static F32 focusLength = 5.0f;

					Vector3 newPosition = m_selectedGameObject.GetComponent<TransformComponent>().translation;
					newPosition += camera.GetForwardDirection() * focusLength;
					camera.SetPosition(newPosition);
				}
			}
		}

		// Detects deletion.
		bool isGameObjectDeleted = false;
		if (ImGui::BeginPopupContextItem(nullptr, 1))
		{
			if (ImGui::MenuItem(ICON_NAME(ICON_FA_TRASH_ALT, "Delete")))
			{
				isGameObjectDeleted = true;
			}

			ImGui::EndPopup();
		}

		if (isTreeOpened)
		{
			ImGui::Text("Entity id: %u", (U32)gameObject);
			ImGui::TreePop();
		}

		ImGui::PopStyleVar(2);

		// Handles deletion.
		if (isGameObjectDeleted)
		{
			// TODO: Consider sending a deletion event.
			MP_WARN("(TODO) Deleting game object: {} (id: {})", gameObject.GetName(), (U32)gameObject);
			// m_scene->DestroyGameObject(gameObject);
			// m_selectedGameObject = {};
		}
	}

	void ScenePanel::DrawSelectedGameObject()
	{
		// ImGui::NewLine();

		if (m_selectedGameObject.IsValid())
		{
			String name = ICON_NAME2(m_selectedGameObject.HasComponent<LightComponent>() ? IconNameLight : IconNameShapes, m_selectedGameObject.GetName());
			ImGui::SeparatorText(name.c_str());

			DrawComponents(m_selectedGameObject);

			ImGui::Separator();
			ImVec2 windowSize = ImGui::GetWindowSize();
			F32	   buttonSize = 118.0f;
			ImGui::SetCursorPosX(windowSize.x / 2.0f - buttonSize / 2.0f);
			ImGui::Button(ICON_NAME(ICON_FA_PLUS, "Add Component"));
		}
		else
		{
			ImGui::SeparatorText(ICON_NAME(ICON_FA_QUESTION, "No Selection"));
		}
	}

	template <typename ComponentType, typename UIFunction>
	static void DrawComponent(GameObject& gameObject, UIFunction drawWidgetsFn)
	{
		static ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed
			| ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_FramePadding;

		if (gameObject.HasComponent<ComponentType>())
		{
			auto& component = gameObject.GetComponent<ComponentType>();

			ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail(); // Get it before TreeNode

			// Headers
			F32 fontSize = ImGui::GetFont()->FontSize / ImGuiConfig::Dpi;
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(3, 3));
			F32 lineHeight = fontSize + ImGui::GetStyle().FramePadding.y * 2.0f;

			String componentTitle = ICON_NAME2(ComponentType::GetIcon(), ComponentType::GetName());
			bool   treeOpened = ImGui::TreeNodeEx(componentTitle.c_str(), treeNodeFlags);

			ImGui::PopStyleVar(1); // FramePadding

			// Component status
			if (typeid(ComponentType) != typeid(TransformComponent)) // Transform cannot be disabled
			{
				ImGui::SameLine();

				ImGui::SameLine(contentRegionAvailable.x - lineHeight - 22.0f + 4.0f, -1.0f);
				ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.3f, 0.31f, 0.31f, 1.0f));
				// TODO: Checkmark Color
				ImGui::Checkbox("##ComponentCheckboxEnabled", &component.enabled);
				ImGui::PopStyleColor(1);
			}

			// Component settings button
			ImGui::SameLine(contentRegionAvailable.x - lineHeight + 4.0f, -1.0f); // -1.0 is default value
			if (ImGui::Button(ICON_FA_BARS))
			{
				ImGui::OpenPopup("##ComponentSettings");
			}

			bool shouldRemoveComponent = false;
			if (ImGui::BeginPopup("##ComponentSettings"))
			{
				if (ImGui::MenuItem(ICON_NAME(ICON_FA_TRASH_ALT, "Delete")))
				{
					shouldRemoveComponent = true;
				}
				ImGui::EndPopup();
			}

			// Show tree
			if (treeOpened)
			{
				ImGui::Unindent(10.0f);
				drawWidgetsFn(component);
				ImGui::Indent(10.0f);
				ImGui::TreePop();
			}

			if (shouldRemoveComponent)
			{
				// TODO: Will crash!
				// gameObject.RemoveComponent<ComponentType>();
			}
		}
	}

	void ScenePanel::DrawComponents(GameObject& gameObject)
	{
		DrawComponent<TransformComponent>(gameObject, [](auto& component) {
			ImGui::DragFloat3("Position", GLM_PTR(component.translation), 1.0f, 0.0f, 0.0f, "%.2f");
			ImGui::DragFloat3("Rotation", GLM_PTR(component.rotation), 1.0f, 0.0f, 0.0f, "%.2f");
			ImGui::DragFloat3("Scale", GLM_PTR(component.scale), 1.0f, 0.0f, 0.0f, "%.2f");
		});

		DrawComponent<MeshComponent>(gameObject, [](auto& component) {
			if (component.model)
			{
				static char modelNameInput[128] = "";
				strncpy(modelNameInput, component.model->GetModelName().c_str(), 128);

				ImGui::InputText("Model", modelNameInput, IM_ARRAYSIZE(modelNameInput), ImGuiInputTextFlags_ReadOnly);
				// ImGui::LabelText("Model name: %s", component.model->GetModelName().c_str());
				ImGui::Text("Vertex count: %u", component.model->GetVertexCount());
				ImGui::Text("Index count: %u", component.model->GetIndexCount());
			}
			else
			{
				ImGui::Text("No model assigned!");
			}
		});

		DrawComponent<LightComponent>(gameObject, [](auto& component) {
			ImGui::ColorEdit3("Color", GLM_PTR(component.color));
			ImGui::ColorEdit3("Intensity", &component.intensity);
		});

		DrawComponent<CameraComponent>(gameObject, [](auto& component) {
			const char* projectionTypes[] = { "Perspective", "Orthographic" };
			static int	currentProjection = 0;
			currentProjection = (int)component.camera.GetProjectionType();
			ImGui::Combo("Projection", &currentProjection, projectionTypes, IM_ARRAYSIZE(projectionTypes));
			ImGui::Checkbox("Primary", &component.primary);
		});

		DrawComponent<NativeScriptComponent>(gameObject, [](auto& component) {
			if (component.scriptable)
			{
				static char scriptNameInput[128] = "";
				strncpy(scriptNameInput, component.scriptable->GetScriptName().c_str(), 128);

				ImGui::InputText("Script", scriptNameInput, IM_ARRAYSIZE(scriptNameInput), ImGuiInputTextFlags_ReadOnly);
			}
			else
			{
				ImGui::Text("No model assigned!");
			}
		});
	}

	/////////////////////////////////////////////////////////////////////////////////

	void ScenePanel::DrawGameObjectCreationMenu()
	{
		if (ImGui::MenuItem(ICON_NAME(ICON_FA_CUBE, "Create GameObject")))
		{
			m_scene->CreateGameObject();
		}

		if (ImGui::BeginMenu(ICON_NAME(ICON_FA_SHAPES, "Create Primitives")))
		{
			if (ImGui::MenuItem("Cube"))
			{
				// TODO: TBA
			}
			if (ImGui::MenuItem("Sphere"))
			{
				// TODO: TBA
			}
			if (ImGui::MenuItem("Plane"))
			{
				// TODO: TBA
			}
			if (ImGui::MenuItem("Capsule"))
			{
				// TODO: TBA
			}

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu(ICON_NAME(ICON_FA_LIGHTBULB, "Create Lights")))
		{
			if (ImGui::MenuItem("Directional Light"))
			{
				// TODO: TBA
			}
			if (ImGui::MenuItem("Point Light"))
			{
				// TODO: TBA
			}
			if (ImGui::MenuItem("Spot Light"))
			{
				// TODO: TBA
			}
			if (ImGui::MenuItem("Ambient Light"))
			{
				// TODO: TBA
			}

			ImGui::EndMenu();
		}
	}

} // namespace Mapo
