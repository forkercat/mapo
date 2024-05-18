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

#include "engine/renderer/render_context.h"
#include "engine/renderer/renderer.h"
#include "engine/renderer/device.h"
#include "engine/renderer/buffer.h"
#include "engine/renderer/descriptors.h"
#include "engine/renderer/frame_info.h"

#include "engine/system/simple_render_system.h"
#include "engine/system/point_light_system.h"

#include <imgui/imgui.h>

namespace Mapo
{
	struct GlobalUbo
	{
		Matrix4 projection{ 1.0f };
		Matrix4 view{ 1.0f };
		Vector4 ambientLightColor{ 1.0f, 1.0f, 1.0f, 0.02f }; // w is intensity
		Vector3 lightPosition{ 0.0f, 1.0f, 0.0f };
		alignas(16) Vector4 lightColor{ 1.0f, 1.0f, 0.0f, 0.5f }; // w is intensity
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
	}

	void EditorLayer::OnDetach()
	{
		// Need to do this to make sure model objects are deleted.
		m_scene.release();
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
		// Model
		Ref<Model> bunnyModel = Model::CreateModelFromFile("assets/models/bunny.obj");
		Ref<Model> smoothModel = Model::CreateModelFromFile("assets/models/smooth_vase.obj");
		Ref<Model> flatModel = Model::CreateModelFromFile("assets/models/flat_vase.obj");
		Ref<Model> quadModel = Model::CreateModelFromFile("assets/models/quad.obj");
		Ref<Model> cubeModel = Model::CreateCubeModel();

		m_scene = MakeUnique<Scene>();

		// Bunny
		GameObject bunnyObject = m_scene->CreateGameObject("Bunny");
		bunnyObject.AddComponent<MeshComponent>(bunnyModel);

		auto& transform1 = bunnyObject.GetComponent<TransformComponent>();
		transform1.translation = { -1.0f, 0.0f, 0.0f };
		transform1.scale = Vector3(4.0f);

		bunnyObject.AddComponent<NativeScriptComponent>().Bind<RotateScript>();

		// Smooth Vase
		GameObject vaseObject = m_scene->CreateGameObject("SmoothVase");
		vaseObject.AddComponent<MeshComponent>(flatModel);

		auto& transform2 = vaseObject.GetComponent<TransformComponent>();
		transform2.translation = { 1.0f, 1.0f, 0.5f };
		transform2.rotation = { MathOp::Radians(180), 0.0f, 0.0f };
		transform2.scale = Vector3(2.0f);

		// Cubes
		GameObject cubeX = m_scene->CreateGameObject("Cube +X");
		cubeX.AddComponent<MeshComponent>(cubeModel);
		cubeX.GetComponent<TransformComponent>().translation = { 3.0f, 0.0f, 0.0f };
		cubeX.GetComponent<TransformComponent>().scale = Vector3(0.2f);

		GameObject cubeZ = m_scene->CreateGameObject("Cube +Z");
		cubeZ.AddComponent<MeshComponent>(cubeModel);
		cubeZ.GetComponent<TransformComponent>().translation = { 0.0f, 0.0f, 5.0f };
		cubeZ.GetComponent<TransformComponent>().scale = Vector3(0.2f);

		// Plane
		GameObject planeObject = m_scene->CreateGameObject("Plane");
		planeObject.AddComponent<MeshComponent>(quadModel);

		auto& transform3 = planeObject.GetComponent<TransformComponent>();
		transform3.translation = { 0.0f, -1.0f, 0.0f };
		transform3.scale = { 3.0f, 1.0f, 3.0f };
	}

	/////////////////////////////////////////////////////////////////////////////////
	// ImGui
	/////////////////////////////////////////////////////////////////////////////////

	void EditorLayer::OnImGuiRender()
	{
		ImGui::Begin("Test");

		static bool showDemo = false;
		ImGui::ShowDemoWindow(&showDemo);
		ImGui::Checkbox("Show demo", &showDemo);

		ImGui::NewLine();

		ImGui::Text("Editor Camera");
		ImGui::Separator();
		ImGui::DragFloat3("Position##1", GLM_PTR(m_camera.GetPosition()));

		auto gameObjects = m_scene->GetGameObjects();

		ImGui::NewLine();

		ImGui::Text("Game Objects");
		ImGui::Separator();

		for (GameObject& go : gameObjects)
		{
			auto& transform = go.GetComponent<TransformComponent>();
			ImGui::DragFloat3(go.GetName().c_str(), GLM_PTR(transform.translation));
		}

		ImGui::NewLine();

		ImGui::Text("Stats");
		ImGui::Separator();
		ImGui::Text("FPS: %.1f  (%.2f ms)", ImGui::GetIO().Framerate, 1000.0f / ImGui::GetIO().Framerate);

		Window& window = Application::Get().GetWindow();
		ImGui::Text("Window: %u x %u", window.GetWidth(), window.GetHeight());

		Renderer& renderer = RenderContext::GetRenderer();
		ImGui::Text("Swapchain: %u x %u", renderer.GetSwapchainWidth(), renderer.GetSwapchainHeight());

		ImGui::Text("Image count: %u", renderer.GetImageCount());

		ImGui::NewLine();
		ImGui::Separator();
		static F32 color[3] { 1.0f, 0.0f, 0.0f };
		ImGui::ColorEdit3("Test color", color);

		ImGui::End(); // Test
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

	/////////////////////////////////////////////////////////////////////////////////

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
