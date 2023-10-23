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
	include "Ilargi/dependencies/Assimp"
	include "Ilargi/dependencies/glfw"
	include "Ilargi/dependencies/imgui"
	include "Ilargi/dependencies/spirvcross"
group ""

VULKAN_SDK = os.getenv("VULKAN_SDK")

IncludeDir = {}
IncludeDir["Assimp"] = "Ilargi/dependencies/Assimp/include"
IncludeDir["GLFW"] = "Ilargi/dependencies/glfw/include"
IncludeDir["ImGUI"] = "Ilargi/dependencies/imgui"
IncludeDir["ImGuizmo"] = "Ilargi/dependencies/imguizmo"
IncludeDir["entt"] = "Ilargi/dependencies/entt"
IncludeDir["stb"] = "Ilargi/dependencies/stb"
IncludeDir["SPIRVCross"] = "Ilargi/dependencies/spirvcross"
IncludeDir["VulkanSDK"] = "%{VULKAN_SDK}/Include"

LibraryDir = {}
LibraryDir["VulkanSDK"] = "%{VULKAN_SDK}/lib"

Library = {}
Library["ShaderC_Debug"] = "%{LibraryDir.VulkanSDK}/shaderc_sharedd.lib"

Library["ShaderC_Release"] = "%{LibraryDir.VulkanSDK}/shaderc_shared.lib"

project "Ilargi"
	location "Ilargi"
	kind "StaticLib"
	language "C++"
	cppdialect "C++20"
	staticruntime "off"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "ilargipch.h"
	pchsource "Ilargi/source/ilargipch.cpp"

	files
	{
		"%{prj.name}/source/**.h",
		"%{prj.name}/source/**.cpp",
		"%{prj.name}/dependencies/imguizmo/**.h",
		"%{prj.name}/dependencies/imguizmo/**.cpp"
	}

	includedirs
	{
		"%{prj.name}/source",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.ImGUI}",
		"%{IncludeDir.ImGuizmo}",
		"%{IncludeDir.Assimp}",
		"%{IncludeDir.entt}",
		"%{IncludeDir.stb}",
		"%{IncludeDir.SPIRVCross}",
		"%{IncludeDir.VulkanSDK}"
	}

	links 
	{
		"GLFW",
		"ImGUI",
		"Assimp",
		"SpirvCrossCore",
		"SpirvCrossGLSL",
		"%{LibraryDir.VulkanSDK}/vulkan-1.lib"
	}

	defines
	{
		--"_CRT_SECURE_NO_WARNINGS"
		"GLFW_INCLUDE_NONE"
	}

	filter "files:Ilargi/dependencies/imguizmo/**.cpp"
	flags { "NoPCH" }

	filter "system:windows"
		systemversion "latest"

		defines
		{
			"ILG_PLATFORM_WINDOWS",
		}

	filter "configurations:Debug"
		defines "ILG_DEBUG"
		runtime "Debug"
		symbols "on"

		links
		{
			"%{Library.ShaderC_Debug}",
		}

	filter "configurations:Release"
		defines "ILG_RELEASE"
		runtime "Release"
		optimize "on"

		links
		{
			"%{Library.ShaderC_Release}",
		}

	filter "configurations:Dist"
		defines "ILG_DIST"
		runtime "Release"
		optimize "on"

		links
		{
			"%{Library.ShaderC_Release}",
		}

-- Creating Ilargi Editor project

project "IlargiEditor"
	location "IlargiEditor"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++20"
	staticruntime "off"

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
		"Ilargi/dependencies",
		"Ilargi/dependencies/entt",
		"Ilargi/dependencies/imguizmo",
		"Ilargi/dependencies/stb"
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