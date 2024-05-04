//
// Created by Junhao Wang (@forkercat) on 4/1/24.
//

#include "core/core.h"

#include "engine/entry_point.h"

/////////////////////////////////////////////////////////////////////////////////
// Application
/////////////////////////////////////////////////////////////////////////////////

namespace Mapo
{
	class EditorApp : public Application
	{
	public:
		EditorApp(ApplicationCommandLineArgs args)
			: Application("Editor App (Vulkan)", args)
		{
		}

		bool Start() override;
	};

	// Defines the creation function.
	Application* CreateApplication(ApplicationCommandLineArgs args)
	{
		return new EditorApp(args);
	}

	bool EditorApp::Start()
	{
		return Application::Start();
	}

} // namespace Mapo

/////////////////////////////////////////////////////////////////////////////////
// Main
/////////////////////////////////////////////////////////////////////////////////

int MapoMain(int argc, char** argv)
{
	Mapo::Application* app = Mapo::CreateApplication({ argc, argv });

	app->Start();
	app->Run();

	return 0;
}
