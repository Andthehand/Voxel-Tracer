#include "MainLayer.h"

#include <imgui.h>

namespace VoxelTracer {
	MainLayer::MainLayer()
		: Layer("MainLayer")
		, m_Camera(glm::vec3(50.0f, 50.0f, 1.0f), glm::vec3(0.0f), glm::vec2(0.0f), 70.0f) {}

	void MainLayer::OnAttach() {
		RE_PROFILE_FUNCTION();

		RenderCommands::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });

		Window& appWindow = Application::Get().GetWindow();
		m_Camera.extent = glm::vec2(appWindow.GetWidth(), appWindow.GetHeight());
		m_Camera.look_at(glm::vec3(0.0f));

		m_PushConstants.PixelToRay = m_Camera.pixel_to_ray_matrix();
		m_PushConstants.VoxelResolution = VOXEL_RESOLUTION;
		m_PushConstants.RenderMode = 0;
		m_PushConstantsBuffer = UniformBuffer::Create(&m_PushConstants, sizeof(m_PushConstants), 0);

		//m_Shader = Shader::Create("assets/temp.comp");
		m_Shader = Shader::Create("assets/traverse.comp");

		struct Uint128 {
			uint64_t low;
			uint64_t high;
		};
		std::vector<Uint128> voxels((VOXEL_RESOLUTION * VOXEL_RESOLUTION * VOXEL_RESOLUTION) / 128);
		for (auto& v : voxels)
			v = { UINT64_MAX, UINT64_MAX };

		Texture3DCreateInfo voxelInfo;
		voxelInfo.Width = VOXEL_RESOLUTION / 4;
		voxelInfo.Height = VOXEL_RESOLUTION / 4;
		voxelInfo.Depth = VOXEL_RESOLUTION / 8;
		voxelInfo.InternalFormat = TextureDataType::RGBA32UI;
		voxelInfo.DataFormat = TextureFormat::RGBAI;
		m_VoxelData = Texture3D::Create(voxelInfo, voxels.data());

		FramebufferSpecification fbSpec;
		fbSpec.Width = appWindow.GetWidth();
		fbSpec.Height = appWindow.GetHeight();
		fbSpec.Attachments = { FramebufferTextureFormat::RGBA8 };
		m_Viewport = Framebuffer::Create(fbSpec);
		m_ViewportSize = { (float)fbSpec.Width, (float)fbSpec.Height };
	}

	void MainLayer::OnUpdate(const float deltaTime) {
		RE_PROFILE_FUNCTION();

		m_Viewport->Bind();
		RenderCommands::Clear();
		m_Viewport->Unbind();

		constexpr float cameraSpeed = 5.0f;
		glm::vec4 v(0.0f);
		if (Input::IsKeyPressed(Key::W))
			v.y += cameraSpeed;
		if (Input::IsKeyPressed(Key::S))
			v.y -= cameraSpeed;
		if (Input::IsKeyPressed(Key::A))
			v.x -= cameraSpeed;
		if (Input::IsKeyPressed(Key::D))
			v.x += cameraSpeed;
		if (Input::IsKeyPressed(Key::E))
			v.z += cameraSpeed;
		if (Input::IsKeyPressed(Key::Q))
			v.z -= cameraSpeed;

		m_Camera.position += glm::vec3(m_Camera.rotation_matrix() * v * deltaTime);
		m_Camera.look_at(glm::vec3(0.0f));
		m_PushConstants.PixelToRay = m_Camera.pixel_to_ray_matrix();
		m_PushConstantsBuffer->SetData(&m_PushConstants, sizeof(m_PushConstants));

		// Assumed compute shader local size (adjust if shader differs)
		constexpr uint32_t LOCAL_SIZE_X = 8;
		constexpr uint32_t LOCAL_SIZE_Y = 8;

		const uint32_t viewportWidth  = static_cast<uint32_t>(m_ViewportSize.x);
		const uint32_t viewportHeight = static_cast<uint32_t>(m_ViewportSize.y);

		const uint32_t groupsX = (viewportWidth  + LOCAL_SIZE_X - 1) / LOCAL_SIZE_X;
		const uint32_t groupsY = (viewportHeight + LOCAL_SIZE_Y - 1) / LOCAL_SIZE_Y;

		m_Shader->Bind();
		glBindImageTexture(0, m_Viewport->GetAttachmentRendererID(), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA8);
		m_VoxelData->BindImage(1);
		RenderCommands::DispatchCompute(groupsX, groupsY, 1);
	}

	void MainLayer::OnImGui() {
		RE_PROFILE_FUNCTION();
		ImGui::Begin("Viewport");

		ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
		uint64_t textureID = m_Viewport->GetAttachmentRendererID();
		ImGui::Image(reinterpret_cast<void*>(textureID), viewportPanelSize, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });

		ImGui::End();
	}

	void MainLayer::OnEvent(Event& event) {
		EventDispatcher dispatcher(event);
		dispatcher.Dispatch<WindowResizeEvent>([this](WindowResizeEvent& e) {
			m_Camera.extent = glm::vec2(e.GetWidth(), e.GetHeight());
			m_PushConstants.PixelToRay = m_Camera.pixel_to_ray_matrix();
			m_PushConstantsBuffer->SetData(&m_PushConstants, sizeof(m_PushConstants));

			return false;
		});
	}
}