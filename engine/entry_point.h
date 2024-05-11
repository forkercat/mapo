//
// Created by Junhao Wang (@forkercat) on 5/4/24.
//

#pragma once

#include "engine/application.h"

// This will be defined by client.
extern int MapoMain(int argc, char** argv);

extern Mapo::Application* Mapo::CreateApplication(ApplicationCommandLineArgs cmdArgs);

int main(int argc, char** argv)
{
	Mapo::Log::Init();

	MapoMain(argc, argv);
}
