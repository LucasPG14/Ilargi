workspace "Ilargi"
	architecture "x86_64"
	startproject "IlargiEditor"

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

	flags
	{
		"MultiProcessorCompile"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- Creating Ilargi project

group "Dependencies"
	include "Ilargi/dependencies/glfw"
group ""

IncludeDir = {}
IncludeDir["GLFW"] = "Ilargi/dependencies/glfw/include"

project "Ilargi"
	location "Ilargi"
	kind "StaticLib"
	language "C++"
	cppdialect "C++20"
	staticruntime "on"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "ilargipch.h"
	pchsource "Ilargi/source/ilargipch.cpp"

	files
	{
		"%{prj.name}/source/**.h",
		"%{prj.name}/source/**.cpp"
	}

	includedirs
	{
		"%{prj.name}/source",
		"%{IncludeDir.GLFW}"
	}

	links 
	{
		"GLFW"
	}

	defines
	{
		--"_CRT_SECURE_NO_WARNINGS"
		--"GLFW_INCLUDE_NONE"
	}

	filter "system:windows"
		systemversion "latest"

		defines
		{
			"ILG_PLATFORM_WINDOWS",
			--"ILG_BUILD_DLL",
		}

	filter "configurations:Debug"
		defines "ILG_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "ILG_RELEASE"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines "ILG_DIST"
		runtime "Release"
		optimize "on"

-- Creating Ilargi Editor project

project "IlargiEditor"
	location "IlargiEditor"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++20"
	staticruntime "on"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"%{prj.name}/source/**.h",
		"%{prj.name}/source/**.cpp"
	}

	includedirs
	{
		"Ilargi/source",
		"Ilargi/dependencies"
	}

	links
	{
		"Ilargi"
	}

	defines
	{
		--"_CRT_SECURE_NO_WARNINGS",
	}

	filter "system:windows"
		systemversion "latest"

		defines
		{
			"ILG_PLATFORM_WINDOWS"
		}

	filter "configurations:Debug"
		defines "ILG_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "ILG_RELEASE"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines "ILG_DIST"
		runtime "Release"
		optimize "on"