//
// Created by Junhao Wang (@forkercat) on 5/10/24.
//

#include "editor_layer.h"

#include "engine/application.h"
#include "engine/window.h"
#include "engine/model.h"
#include "engine/game_object.h"
#include "engine/component.h"

#include "engine/input/input.h"

#include "engine/renderer/render_context.h"
#include "engine/renderer/renderer.h"
#include "engine/renderer/device.h"
#include "engine/renderer/buffer.h"
#include "engine/renderer/descriptors.h"
#include "engine/renderer/frame_info.h"

#include "engine/system/simple_render_system.h"
#include "engine/system/point_light_system.h"
#include "engine/system/rainbow_system.h"
#include "engine/system/imgui_system.h"

#include <imgui/imgui.h>

namespace Mapo
{
	struct GlobalUbo
	{
		Matrix4 projection{ 1.0f };
		Matrix4 view{ 1.0f };
		Vector4 ambientLightColor{ 1.0f, 1.0f, 1.0f, 0.02f }; // w is intensity
		Vector3 lightPosition{ 0.0f, 0.0f, 0.0f };
		alignas(16) Vector4 lightColor{ 1.0f, 1.0f, 1.0f, 0.5f }; // w is intensity
	};

	/////////////////////////////////////////////////////////////////////////////////

	// TODO: Should not be owned by the layer.
	static std::vector<UniqueRef<Buffer>> s_uboBuffers;
	static std::vector<VkDescriptorSet>	  s_globalDescriptorSets;

	/////////////////////////////////////////////////////////////////////////////////

	EditorLayer::EditorLayer()
		: Layer("EditorLayer")
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
		Window& window = Application::Get().GetWindow();
		m_renderSystem = MakeUnique<SimpleRenderSystem>(renderer.GetRenderPass(), globalSetLayout->GetDescriptorSetLayout());
		m_pointLightSystem = MakeUnique<PointLightSystem>(renderer.GetRenderPass(), globalSetLayout->GetDescriptorSetLayout());
		m_rainbowSystem = MakeUnique<RainbowSystem>(0.4f);
	}

	void EditorLayer::OnDetach()
	{
		// Need to do this to make sure model objects are deleted.
		m_scene.release();
		m_rainbowSystem.release();
		m_pointLightSystem.release();
		m_renderSystem.release();

		for (auto& pBuffer : s_uboBuffers)
		{
			pBuffer.release();
		}

		s_globalDescriptorSets.resize(0);
	}

	void EditorLayer::OnUpdate(const Timestep dt)
	{
		Renderer& renderer = RenderContext::GetRenderer();

		// Retrieve all game objects before the loop. // GameObject is a lightweight class that just contains entity ids.
		std::vector<GameObject> sceneGameObjects = m_scene->GetGameObjects();

		m_controller.MoveInPlaneXZ(dt, m_player);

		auto& viewerTransform = m_player.GetComponent<TransformComponent>();
		m_camera.SetViewYXZ(viewerTransform.translation, viewerTransform.rotation);

		F32 aspect = renderer.GetAspectRatio();
		m_camera.SetPerspectiveProjection(MathOp::Radians(50.f), aspect, 0.1f, 100.f);

		/////////////////////////////////////////////////////////////////////////////////

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
		ubo.projection = m_camera.GetProjection();
		ubo.view = m_camera.GetView();
		s_uboBuffers[frameInfo.frameIndex]->WriteToBuffer(&ubo);
		s_uboBuffers[frameInfo.frameIndex]->Flush();

		m_renderSystem->RenderGameObjects(frameInfo);
		m_pointLightSystem->Render(frameInfo);
	}

	void EditorLayer::CreateScene()
	{
		// Model
		Ref<Model> smoothModel = Model::CreateModelFromFile("assets/models/smooth_vase.obj");
		Ref<Model> flatModel = Model::CreateModelFromFile("assets/models/flat_vase.obj");
		Ref<Model> quadModel = Model::CreateModelFromFile("assets/models/quad.obj");

		m_scene = MakeUnique<Scene>();

		// 1
		GameObject gameObject1 = m_scene->CreateGameObject("SmoothVase");
		gameObject1.AddComponent<MeshComponent>(smoothModel);

		auto& transform1 = gameObject1.GetComponent<TransformComponent>();
		transform1.translation = { -0.5f, 0.5f, 0.0f };
		transform1.scale = Vector3(2.0f);

		// 2
		GameObject gameObject2 = m_scene->CreateGameObject("FlatVase");
		gameObject2.AddComponent<MeshComponent>(flatModel);

		auto& transform2 = gameObject2.GetComponent<TransformComponent>();
		transform2.translation = { 0.5f, 0.5f, 0.0f };
		transform2.scale = Vector3(2.0f);

		// 3
		GameObject gameObject3 = m_scene->CreateGameObject("Quad");
		gameObject3.AddComponent<MeshComponent>(quadModel);

		auto& transform3 = gameObject3.GetComponent<TransformComponent>();
		transform3.translation = { 0.0f, 0.5f, 0.0f };
		transform3.scale = { 3.0f, 1.0f, 3.0f };

		// Set up camera and player.
		m_camera.SetViewTarget(Vector3(-1.0f, -2.0f, 2.0f), Vector3(0.0f, 0.0f, 2.5f));
		m_player = m_scene->CreateGameObject("Player");
		m_player.GetComponent<TransformComponent>().translation.z = -2.5f;
	}

	/////////////////////////////////////////////////////////////////////////////////
	// ImGui
	/////////////////////////////////////////////////////////////////////////////////

	void EditorLayer::OnImGuiRender()
	{
	}

	/////////////////////////////////////////////////////////////////////////////////
	// Events
	/////////////////////////////////////////////////////////////////////////////////

	void EditorLayer::OnEvent(Event& event)
	{
		// Handles camera updates.
		// m_camera
		// m_controller

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
				// TODO: Add ImGuizmo support.
		}

		return true;
	}

	bool EditorLayer::OnMouseButtonPressed(MouseButtonPressedEvent& event)
	{
		// TODO: Handles entity selection.
		return false;
	}

} // namespace Mapo
