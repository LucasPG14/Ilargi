#pragma once

#include "Application.h"

Ilargi::Application* Ilargi::CreateApp(int argc, char* argv[]);

int main(int argc, char* argv[])
{
	Ilargi::Application* app = Ilargi::CreateApp(argc, argv);

	app->Update();
	
	delete app;
}