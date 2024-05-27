//
// Created by Junhao Wang (@forkercat) on 5/10/24.
//

#include "editor_layer.h"

#include "engine/application.h"
#include "engine/window.h"
#include "engine/model.h"

#include "engine/scene/scene.h"
#include "engine/scene/game_object.h"
#include "engine/scene/component.h"
#include "engine/scene/script_rotate.h"

#include "engine/input/input.h"

#include "engine/ui/imgui_utils.h"

#include "engine/renderer/render_context.h"
#include "engine/renderer/renderer.h"
#include "engine/renderer/device.h"
#include "engine/renderer/buffer.h"
#include "engine/renderer/descriptors.h"
#include "engine/renderer/frame_info.h"

#include "engine/system/simple_render_system.h"
#include "engine/system/point_light_system.h"

#include <imgui/imgui.h>

#include <ImGuizmo.h>

namespace Mapo
{
	struct GlobalUbo
	{
		Matrix4 projection{ 1.0f };
		Matrix4 view{ 1.0f };
		Vector4 ambientLightColor{ 1.0f, 1.0f, 1.0f, 0.02f }; // w is intensity
		Vector3 lightPosition{ 0.0f, 1.0f, 0.0f };
		alignas(16) Vector4 lightColor{ 1.0f, 1.0f, 1.0f, 1.0f }; // w is intensity
	};

	/////////////////////////////////////////////////////////////////////////////////

	// TODO: Should not be owned by the layer.
	static std::vector<UniqueRef<Buffer>> s_uboBuffers;
	static std::vector<VkDescriptorSet>	  s_globalDescriptorSets;

	/////////////////////////////////////////////////////////////////////////////////

	EditorLayer::EditorLayer()
		: Layer("EditorLayer"), m_camera(45.0f, RenderContext::GetRenderer().GetAspectRatio())
	{
	}

	void EditorLayer::OnAttach()
	{
		Init();
		CreateScene();
	}

	void EditorLayer::Init()
	{
		Renderer&		renderer = RenderContext::GetRenderer();
		DescriptorPool& globalDescriptorPool = RenderContext::GetDescriptorPool();

		// Uniform buffers
		s_uboBuffers.resize(RenderContext::GetMaxFramesInFlight());

		for (int i = 0; i < s_uboBuffers.size(); ++i)
		{
			s_uboBuffers[i] = MakeUnique<Buffer>(
				sizeof(GlobalUbo),
				1,
				VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT); // or add VK_MEMORY_PROPERTY_HOST_COHERENT_BIT

			s_uboBuffers[i]->Map();
		}

		// Descriptors
		UniqueRef<DescriptorSetLayout> globalSetLayout =
			DescriptorSetLayout::Builder()
				.AddBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS)
				.Build();

		s_globalDescriptorSets.resize(RenderContext::GetMaxFramesInFlight()); // one set per frame

		for (int i = 0; i < s_globalDescriptorSets.size(); ++i)
		{
			VkDescriptorBufferInfo bufferInfo = s_uboBuffers[i]->DescriptorInfo();

			DescriptorWriter(*globalSetLayout, globalDescriptorPool)
				.WriteBuffer(0, &bufferInfo)
				.Build(s_globalDescriptorSets[i]);
		}

		// Set up systems.
		m_renderSystem = MakeUnique<SimpleRenderSystem>(renderer.GetRenderPass(), globalSetLayout->GetDescriptorSetLayout());
		m_pointLightSystem = MakeUnique<PointLightSystem>(renderer.GetRenderPass(), globalSetLayout->GetDescriptorSetLayout());

		// Default gizmo type.
		m_gizmoType = ImGuizmo::OPERATION::TRANSLATE;
	}

	void EditorLayer::OnDetach()
	{
		// Need to do this to make sure model objects are deleted.
		m_pointLightSystem.release();
		m_renderSystem.release();

		for (auto& pBuffer : s_uboBuffers)
		{
			pBuffer.release();
		}

		s_globalDescriptorSets.resize(0);
	}

	void EditorLayer::OnUpdate(Timestep dt)
	{
		Renderer& renderer = RenderContext::GetRenderer();

		// Retrieve all game objects before the loop. // GameObject is a lightweight class that just contains entity ids.
		std::vector<GameObject> sceneGameObjects = m_scene->GetGameObjects();

		// / Resize
		Window& window = Application::Get().GetWindow();
		if (window.WasFramebufferResized())
		{
			m_camera.SetViewportSize(window.GetWidth(), window.GetHeight());
		}

		// Update
		m_camera.OnUpdate(dt);
		m_scene->OnUpdateEditor(dt, m_camera);

		/////////////////////////////////////////////////////////////////////////////////

		// TODO: The below code should be moved to the scene class?

		// Prepare frame info
		U32 frameIndex = renderer.GetCurrentFrameIndex();

		FrameInfo frameInfo{
			.frameIndex = frameIndex,
			.frameTime = dt,
			.commandBuffer = renderer.GetCurrentCommandBuffer(),
			.globalDescriptorSet = s_globalDescriptorSets[frameIndex],
			.camera = m_camera,
			.gameObjects = sceneGameObjects
		};

		// Update
		GlobalUbo ubo{};

		ubo.projection = m_camera.GetProjectionMatrix();
		ubo.view = m_camera.GetViewMatrix();

		s_uboBuffers[frameInfo.frameIndex]->WriteToBuffer(&ubo);
		s_uboBuffers[frameInfo.frameIndex]->Flush();

		m_renderSystem->RenderGameObjects(frameInfo);
		m_pointLightSystem->Render(frameInfo);
	}

	void EditorLayer::CreateScene()
	{
		m_scene = MakeRef<Scene>();
		m_scenePanel.SetContext(m_scene);

		// Model
		Ref<Model> bunnyModel = Model::CreateModelFromFile("assets/models/bunny.obj");
		Ref<Model> smoothModel = Model::CreateModelFromFile("assets/models/smooth_vase.obj");
		Ref<Model> flatModel = Model::CreateModelFromFile("assets/models/flat_vase.obj");
		Ref<Model> vikingRoomModel = Model::CreateModelFromFile("assets/models/viking_room/viking_room.obj");
		Ref<Model> quadModel = Model::CreateModelFromFile("assets/models/quad.obj");
		Ref<Model> coloredCube = Model::CreateModelFromFile("assets/models/colored_cube.obj");
		Ref<Model> cubeModel = Model::CreateModelFromFile("assets/models/cube.obj");
		Ref<Model> catModel = Model::CreateModelFromFile("assets/models/cat/cat.obj");
		Ref<Model> fatCatModel = Model::CreateModelFromFile("assets/models/fat_cat/fat_cat.obj");
		// Ref<Model> cubeModel = Model::CreateCubeModel();

		// Plane
		GameObject planeObject = m_scene->CreateGameObject("Plane");
		planeObject.AddComponent<MeshComponent>(quadModel);
		auto& planeTransform = planeObject.GetComponent<TransformComponent>();
		planeTransform.translation = { 0.0f, -1.0f, 0.0f };
		planeTransform.scale = { 3.0f, 1.0f, 3.0f };

		// Cubes
		GameObject cubeX = m_scene->CreateGameObject("Cube +X");
		cubeX.AddComponent<MeshComponent>(cubeModel);
		cubeX.GetComponent<TransformComponent>().translation = { 3.0f, 0.0f, 0.0f };
		cubeX.GetComponent<TransformComponent>().scale = Vector3(0.1f);
		GameObject cubeZ = m_scene->CreateGameObject("Cube +Z");
		cubeZ.AddComponent<MeshComponent>(cubeModel);
		cubeZ.GetComponent<TransformComponent>().translation = { 0.0f, 0.0f, 5.0f };
		cubeZ.GetComponent<TransformComponent>().scale = Vector3(0.1f);

		// Bunny
		GameObject bunnyObject = m_scene->CreateGameObject("Bunny");
		bunnyObject.AddComponent<MeshComponent>(bunnyModel);
		auto& bunnyTransform = bunnyObject.GetComponent<TransformComponent>();
		bunnyTransform.translation = { -1.0f, 0.5f, 0.0f };
		bunnyTransform.scale = Vector3(0.6f);
		bunnyObject.AddComponent<NativeScriptComponent>().Bind<RotateScript>();

		// Smooth Vase
		GameObject vaseObject = m_scene->CreateGameObject("SmoothVase");
		vaseObject.AddComponent<MeshComponent>(flatModel);
		auto& vaseTransform = vaseObject.GetComponent<TransformComponent>();
		vaseTransform.translation = { 1.0f, 1.0f, 0.5f };
		vaseTransform.rotation.x = 180.0f;
		vaseTransform.scale = Vector3(2.0f);

		// Viking Room
		GameObject roomObject = m_scene->CreateGameObject("Viking Room");
		roomObject.AddComponent<MeshComponent>(vikingRoomModel);
		auto& roomTransform = roomObject.GetComponent<TransformComponent>();
		roomTransform.translation = { 2.0f, -0.9f, -1.0f };
		roomTransform.rotation = { -90, -180, 3.0f };

		// Cat
		GameObject catObject = m_scene->CreateGameObject("Cat V1");
		catObject.AddComponent<MeshComponent>(catModel);
		auto& catTransform = catObject.GetComponent<TransformComponent>();
		catTransform.translation = { -1.5f, -1.0f, 2.0f };
		catTransform.rotation.x = -90.0f;
		catTransform.scale = Vector3(0.03f);
		catObject.AddComponent<NativeScriptComponent>().Bind<RotateScript>();

		// Fat Cat
		GameObject fatCatObject = m_scene->CreateGameObject("Fat Cat");
		fatCatObject.AddComponent<MeshComponent>(fatCatModel);
		auto& fatCatTransform = fatCatObject.GetComponent<TransformComponent>();
		fatCatTransform.translation = { 1.5f, -0.7f, 2.0f };
		fatCatTransform.scale = Vector3(0.03f);
		fatCatObject.AddComponent<NativeScriptComponent>().Bind<RotateScript>();
	}

	/////////////////////////////////////////////////////////////////////////////////
	// ImGui
	/////////////////////////////////////////////////////////////////////////////////

	void EditorLayer::OnImGuiRender()
	{
		m_scenePanel.OnImGuiRender(m_camera);
		m_infoPanel.OnImGuiRender();
		m_logPanel.OnImGuiRender();

		OnGizmoUpdate();
	}

	void EditorLayer::OnGizmoUpdate()
	{
		GameObject& selected = m_scenePanel.GetSelection();

		if (selected.IsValid() && m_gizmoType != INVALID_GIZMO_TYPE)
		{
			bool isViewportFocused = ImGui::IsWindowFocused();
			bool isViewportHovered = ImGui::IsWindowHovered();
			// TODO: Block events!
			// Application::Get().GetImGuiLayer()->BlockEvents(!isViewportFocused && !isViewportHovered);

			ImGuizmo::SetOrthographic(false);
			ImGuizmo::SetDrawlist(ImGui::GetBackgroundDrawList());

			ImGuiIO& io = ImGui::GetIO();
			ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);

			ImGuizmo::AllowAxisFlip(false);

			// Snapping
			bool snap = Input::IsKeyPressed(Key::LeftShift);
			F32	 snapValue = m_gizmoType == ImGuizmo::OPERATION::ROTATE ? 45.0f : 0.5f;
			F32	 snapValues[3]{ snapValue, snapValue, snapValue };

			auto&	transform = selected.GetComponent<TransformComponent>();
			Matrix4 transformMatrix = transform.GetTransformMatrix();

			Matrix4 projectionMatrix = m_camera.GetProjectionMatrix();
			projectionMatrix[1][1] *= -1; // flip back to the OpenGL version
			const Matrix4& viewMatrix = m_camera.GetViewMatrix();

			ImGuizmo::Enable(true);

			ImGuizmo::Manipulate(
				GLM_PTR(viewMatrix),
				GLM_PTR(projectionMatrix),
				(ImGuizmo::OPERATION)m_gizmoType,
				ImGuizmo::LOCAL,
				// ImGuizmo::WORLD,
				GLM_PTR(transformMatrix),
				nullptr,
				snap ? snapValues : nullptr);

			if (ImGuizmo::IsUsing())
			{
				// Update object's transform.
				Vector3 newTranslation{};
				Vector3 newRotation{};
				Vector3 newScale{};
				MathOp::DecomposeTransform(transformMatrix, newTranslation, newRotation, newScale);
				Vector3 rotationDelta = newRotation - transform.rotation; // avoid gimbal lock

				transform.translation = newTranslation;
				transform.rotation += rotationDelta;
				transform.scale = newScale;
			}

			// Cube and grid
			// ImGuizmo::DrawCubes(GLM_PTR(viewMatrix), GLM_PTR(projectionMatrix), GLM_PTR(transformMatrix), 1);
			// ImGuizmo::DrawGrid(GLM_PTR(viewMatrix), GLM_PTR(projectionMatrix), GLM_PTR(transformMatrix), 10);
		}

		// Draw
		ImGuiUtils::DrawGizmoControls(m_gizmoType);
	}

	/////////////////////////////////////////////////////////////////////////////////
	// Events
	/////////////////////////////////////////////////////////////////////////////////

	void EditorLayer::OnEvent(Event& event)
	{
		// Handles camera updates.
		m_camera.OnEvent(event);

		EventDispatcher dispatcher(event);
		dispatcher.Dispatch<KeyPressedEvent>(MP_BIND_EVENT_FN(EditorLayer::OnKeyPressed));
		dispatcher.Dispatch<MouseButtonPressedEvent>(MP_BIND_EVENT_FN(EditorLayer::OnMouseButtonPressed));
	}

	bool EditorLayer::OnKeyPressed(KeyPressedEvent& event)
	{
		// Skip since this function only handles shortcuts.
		if (event.GetRepeatCount() > 0)
		{
			return false;
		}

#ifdef MP_MACOS_BUILD
		bool control = Input::IsKeyPressed(Key::LeftSuper) || Input::IsKeyPressed(Key::RightSuper); // for macos
#else
		bool control = Input::IsKeyPressed(Key::LeftControl) || Input::IsKeyPressed(Key::RightControl);
#endif
		bool shift = Input::IsKeyPressed(Key::LeftShift) || Input::IsKeyPressed(Key::RightShift);

		bool mouse = Input::IsMouseButtonPressed(Mouse::ButtonLeft) || Input::IsMouseButtonPressed(Mouse::ButtonRight) || Input::IsMouseButtonPressed(Mouse::ButtonMiddle);

		if (!mouse)
		{
			switch (event.GetKeyCode())
			{
					// Scenes
				case Key::N:
					if (control)
					{
						NewScene();
					}
					break;
				case Key::O:
					if (control)
					{
						OpenScene();
					}
					break;
				case Key::S:
					if (control && shift)
					{
						SaveSceneAs();
					}
					break;

					// Gizmos
				case Key::Q: // Select
				{
					if (!ImGuizmo::IsUsing())
					{
						MP_WARN("SELECT");
						m_gizmoType = INVALID_GIZMO_TYPE;
					}
					break;
				}
				case Key::W: // Translate
				{
					if (!ImGuizmo::IsUsing())
					{
						MP_WARN("TRANSLATE");
						m_gizmoType = ImGuizmo::OPERATION::TRANSLATE;
					}
					break;
				}
				case Key::E: // Rotate
				{
					if (!ImGuizmo::IsUsing())
					{
						MP_WARN("ROTATE");
						m_gizmoType = ImGuizmo::OPERATION::ROTATE;
					}
					break;
				}
				case Key::R: // Scale
				{
					if (!ImGuizmo::IsUsing())
					{
						MP_WARN("SCALE");
						m_gizmoType = ImGuizmo::OPERATION::SCALE;
					}
					break;
				}
					// Focus camera on the selected object.
				case Key::F:
				{
					FocusOnGameObject();
				}
			}
		}

		return true;
	}

	bool EditorLayer::OnMouseButtonPressed(MouseButtonPressedEvent& event)
	{
		// TODO: Handles entity selection.
		return false;
	}

	/////////////////////////////////////////////////////////////////////////////////

	void EditorLayer::DisplayMenuBar()
	{
		bool openScenePopup = false;
		bool saveSceneAsPopup = false;

		// MenuBars
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				// Disabling fullscreen would allow the window to be moved to the front of other windows,
				// which we can't undo at the moment without finer window depth/z control.

				// New
				if (ImGui::MenuItem("New", "Ctrl+N"))
				{
					NewScene();
				}

				if (ImGui::MenuItem("Open...", "Cmd+O"))
				{
					openScenePopup = true;
				}

				if (ImGui::MenuItem("Save As...", "Cmd+Shift+S"))
				{
					saveSceneAsPopup = true;
				}

				if (ImGui::MenuItem("Exit"))
				{
					Application::Get().Close();
				}

				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}
	}

	void EditorLayer::FocusOnGameObject()
	{
		if (GameObject& selected = m_scenePanel.GetSelection(); selected.IsValid())
		{
			static F32 focusLength = 5.0f;

			Vector3 newPosition = selected.GetComponent<TransformComponent>().translation;
			newPosition += m_camera.GetForwardDirection() * focusLength;
			m_camera.SetPosition(newPosition);
		}
	}

	void EditorLayer::ShowCameraMatrix()
	{
		Matrix4& viewMatrix = m_camera.GetViewMatrix();
		Matrix4& projMatrix = m_camera.GetProjectionMatrix();

		ImGui::Text("View matrix:");
		if (ImGui::BeginTable("View matrix", 4))
		{
			for (int row = 0; row < 4; row++)
			{
				ImGui::TableNextRow();
				for (int column = 0; column < 4; column++)
				{
					ImGui::TableSetColumnIndex(column);
					ImGui::Text("%.1f", viewMatrix[column][row]);
				}
			}
			ImGui::EndTable();
		}

		ImGui::Text("Projection matrix:");
		if (ImGui::BeginTable("Projection matrix", 4))
		{
			for (int row = 0; row < 4; row++)
			{
				ImGui::TableNextRow();
				for (int column = 0; column < 4; column++)
				{
					ImGui::TableSetColumnIndex(column);
					ImGui::Text("%.1f", projMatrix[column][row]);
				}
			}
			ImGui::EndTable();
		}
	}

} // namespace Mapo
