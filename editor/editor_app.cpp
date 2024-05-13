//
// Created by Junhao Wang (@forkercat) on 4/1/24.
//

#include "core/core.h"

#include "engine/entry_point.h"

#include "editor/editor_layer.h"

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
			PushLayer(MP_NEW(EditorLayer));
		}
	};

	// Defines the creation function.
	Application* CreateApplication(ApplicationCommandLineArgs args)
	{
		return MP_NEW(EditorApp)(args);
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

	MP_DELETE(app);

	return 0;
}
