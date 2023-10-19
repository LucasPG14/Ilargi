#pragma once

#include "Application.h"

Ilargi::Application* Ilargi::CreateApp();

int main(int argc, char* argv[])
{
	Ilargi::Application* app = Ilargi::CreateApp();

	app->Update();
	
	delete app;
}