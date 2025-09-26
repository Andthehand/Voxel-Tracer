#pragma once
#include "RealEngine.h"

#include "VoxelCamera.h"

using namespace RealEngine;

namespace VoxelTracer {
	class MainLayer : public Layer {
	public:
		MainLayer();

		virtual void OnAttach() override;
		virtual void OnUpdate(const float deltaTime) override;
		virtual void OnImGui() override;
		virtual void OnEvent(Event& event) override;

	private:
		Ref<UniformBuffer> m_PushConstantsBuffer;
		Ref<Texture3D> m_VoxelData;
		Ref<Framebuffer> m_Viewport;
		glm::vec2 m_ViewportSize = { 1280, 720 };
		Ref<Shader> m_Shader;
		constexpr static uint32_t VOXEL_RESOLUTION = 24;

		VoxelCamera m_Camera;

		struct PushConstants {
			glm::mat4 PixelToRay;
			uint32_t VoxelResolution;
			uint32_t RenderMode;
		} m_PushConstants;
	};
}
