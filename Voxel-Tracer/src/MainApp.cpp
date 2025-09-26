#include <RealEngine/Core/EntryPoint.h>
#include <RealEngine.h>

#include "MainLayer.h"

using namespace RealEngine;

namespace VoxelTracer {
	class MainApp : public Application {
	public:
		MainApp(const ApplicationSpecification& specification)
			: Application(specification) {
			PushLayer(new MainLayer());
		}

		~MainApp() = default;
	};
}

Application* RealEngine::CreateApplication(const ApplicationCommandLineArgs& args) {
	RE_PROFILE_FUNCTION();

	RE_CORE_WARN("Current Working Directory: {0}", std::filesystem::current_path());

	ApplicationSpecification specification = { "VoxelTracer", args };
	return new VoxelTracer::MainApp(specification);
}
