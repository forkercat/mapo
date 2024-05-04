//
// Created by Junhao Wang (@forkercat) on 5/4/24.
//

#include "application.h"

#include "engine/system/simple_render_system.h"
#include "engine/system/point_light_system.h"
#include "engine/system/rainbow_system.h"
#include "engine/system/imgui_system.h"

#include "engine/keyboard_controller.h"

#include "engine/component.h"

#include <chrono>

namespace Mapo
{
	Application* Application::s_appInstance = nullptr;

	struct GlobalUbo
	{
		Matrix4 projection{ 1.0f };
		Matrix4 view{ 1.0f };
		Vector4 ambientLightColor{ 1.0f, 1.0f, 1.0f, 0.02f }; // w is intensity
		Vector3 lightPosition{ 0.0f, 0.0f, 0.0f };
		alignas(16) Vector4 lightColor{ 1.0f, 1.0f, 1.0f, 0.5f }; // w is intensity
	};

	Application::Application(const String& name, ApplicationCommandLineArgs args)
		: m_commandLineArgs(args)
	{
		m_globalDescriptorPool =
			VulkanDescriptorPool::Builder(m_device)
				// How many descriptor sets can be created from the pool.
				.SetMaxSets(VulkanSwapchain::MAX_FRAMES_IN_FLIGHT)
				// How many descriptors of this type are available in the pool.
				.AddPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VulkanSwapchain::MAX_FRAMES_IN_FLIGHT)
				.Build();

		// TODO: Set window name.

		// Self assign.
		s_appInstance = this;
	}

	Application::~Application()
	{
	}

	bool Application::Start()
	{
		// Cube
		// Ref<Model> model = Model::CreateCubeModel(m_device, { 0.f, 0.f, 0.f });

		// Model
		Ref<Model> smoothModel = Model::CreateModelFromFile(m_device, "assets/models/smooth_vase.obj");
		Ref<Model> flatModel = Model::CreateModelFromFile(m_device, "assets/models/flat_vase.obj");
		Ref<Model> quadModel = Model::CreateModelFromFile(m_device, "assets/models/quad.obj");

		m_scene = MakeUniqueRef<Scene>();

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
	}

	void Application::Run()
	{
		// Create ImGui system.
		ImGuiSystem imguiSystem{
			m_window, m_device, m_renderer.GetSwapchainRenderPass(), m_renderer.GetSwapchainImageCount()
		};

		// Uniform buffers
		std::vector<UniqueRef<VulkanBuffer>> uboBuffers(VulkanSwapchain::MAX_FRAMES_IN_FLIGHT);

		for (int i = 0; i < uboBuffers.size(); ++i)
		{
			uboBuffers[i] = MakeUniqueRef<VulkanBuffer>(
				m_device,
				sizeof(GlobalUbo),
				1,
				VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT); // or add VK_MEMORY_PROPERTY_HOST_COHERENT_BIT

			uboBuffers[i]->Map();
		}

		// Descriptors
		UniqueRef<VulkanDescriptorSetLayout> globalSetLayout =
			VulkanDescriptorSetLayout::Builder(m_device)
				.AddBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS)
				.Build();

		std::vector<VkDescriptorSet> globalDescriptorSets(VulkanSwapchain::MAX_FRAMES_IN_FLIGHT); // one set per frame
		for (int i = 0; i < globalDescriptorSets.size(); ++i)
		{
			VkDescriptorBufferInfo bufferInfo = uboBuffers[i]->DescriptorInfo();

			VulkanDescriptorWriter(*globalSetLayout, *m_globalDescriptorPool)
				.WriteBuffer(0, &bufferInfo)
				.Build(globalDescriptorSets[i]);
		}

		// Render system, camera, and controller
		SimpleRenderSystem simpleRenderSystem(
			m_device, m_renderer.GetSwapchainRenderPass(), globalSetLayout->GetDescriptorSetLayout());
		PointLightSystem pointLightSystem(
			m_device, m_renderer.GetSwapchainRenderPass(), globalSetLayout->GetDescriptorSetLayout());
		RainbowSystem rainbowSystem(0.4f);

		// Camera
		Camera camera;
		camera.SetViewTarget(Vector3(-1.0f, -2.0f, 2.0f), Vector3(0.0f, 0.0f, 2.5f));

		GameObject viewerObject = m_scene->CreateGameObject("Viewer");
		viewerObject.GetComponent<TransformComponent>().translation.z = -2.5f;

		KeyboardController cameraController{};

		// Retrieve all game objects before the loop. // GameObject is a lightweight class that just contains entity ids.
		std::vector<GameObject> sceneGameObjects = m_scene->GetGameObjects();

		std::chrono::time_point currentTime = std::chrono::high_resolution_clock::now();

		while (!m_window.ShouldClose())
		{
			glfwPollEvents();

			// Update time after polling because polling might block.
			std::chrono::time_point newTime = std::chrono::high_resolution_clock::now();
			F32 frameTime = std::chrono::duration<F32, std::chrono::seconds::period>(newTime - currentTime).count();
			currentTime = newTime;

			cameraController.MoveInPlaneXZ(m_window.GetNativeWindow(), frameTime, viewerObject);
			auto& viewerTransform = viewerObject.GetComponent<TransformComponent>();
			camera.SetViewYXZ(viewerTransform.translation, viewerTransform.rotation);

			F32 aspect = m_renderer.GetAspectRatio();
			camera.SetPerspectiveProjection(MathOp::Radians(50.f), aspect, 0.1f, 100.f);

			// Could be nullptr if, for example, the swapchain needs to be recreated.
			if (VkCommandBuffer commandBuffer = m_renderer.BeginFrame())
			{
				// ImGui
				imguiSystem.NewFrame();

				// Prepare frame info
				U32 frameIndex = m_renderer.GetCurrentFrameIndex();
				VulkanFrameInfo frameInfo{
					.frameIndex = frameIndex,
					.frameTime = frameTime,
					.commandBuffer = commandBuffer,
					.globalDescriptorSet = globalDescriptorSets[frameIndex],
					.camera = camera,
					.gameObjects = sceneGameObjects
				};

				// Update
				GlobalUbo ubo{};
				ubo.projection = camera.GetProjection();
				ubo.view = camera.GetView();
				uboBuffers[frameInfo.frameIndex]->WriteToBuffer(&ubo);
				uboBuffers[frameInfo.frameIndex]->Flush();

				// The reason why BeginFrame and BeginSwapchainRenderPass are separate functions is
				// we want the app to control over this to enable us easily integrating multiple render passes.
				//
				// - BeginFrame to acquire image and begin command buffer
				// - Begin offscreen shadow pass
				// -   Render shadow casting objects
				// - End offscreen shadow pass
				// - Begin shading pass
				// -   Render objects
				// - End shading pass
				// - Post processing...
				m_renderer.BeginSwapchainRenderPass(commandBuffer);

				simpleRenderSystem.RenderGameObjects(frameInfo);
				pointLightSystem.Render(frameInfo);

				// Draw ImGui.
				imguiSystem.RunExample();
				imguiSystem.Render(commandBuffer);

				m_renderer.EndSwapchainRenderPass(commandBuffer);
				m_renderer.EndFrame();
			}
		}

		vkDeviceWaitIdle(m_device.GetDevice());
	}

} // namespace Mapo
