#pragma once

#include "Application.h"

Ilargi::Application* Ilargi::CreateApp();

int main()
{
	Ilargi::Application* app = Ilargi::CreateApp();

	app->Update();
	
	app->CleanUp();
}