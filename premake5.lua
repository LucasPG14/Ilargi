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
group ""

VULKAN_SDK = os.getenv("VULKAN_SDK")

IncludeDir = {}
IncludeDir["Assimp"] = "Ilargi/dependencies/Assimp/include"
IncludeDir["GLFW"] = "Ilargi/dependencies/glfw/include"
IncludeDir["ImGUI"] = "Ilargi/dependencies/imgui"
IncludeDir["GLM"] = "Ilargi/dependencies/glm"
IncludeDir["entt"] = "Ilargi/dependencies/entt"
IncludeDir["VulkanSDK"] = "%{VULKAN_SDK}/Include"

LibraryDir = {}
LibraryDir["VulkanSDK"] = "%{VULKAN_SDK}/lib"

Library = {}
Library["ShaderC_Debug"] = "%{LibraryDir.VulkanSDK}/shaderc_sharedd.lib"
Library["SPIRV_Cross_Debug"] = "%{LibraryDir.VulkanSDK}/spirv-cross-cored.lib"
Library["SPIRV_Cross_GLSL_Debug"] = "%{LibraryDir.VulkanSDK}/spirv-cross-glsld.lib"

Library["ShaderC_Release"] = "%{LibraryDir.VulkanSDK}/shaderc_shared.lib"
Library["SPIRV_Cross_Release"] = "%{LibraryDir.VulkanSDK}/spirv-cross-core.lib"
Library["SPIRV_Cross_GLSL_Release"] = "%{LibraryDir.VulkanSDK}/spirv-cross-glsl.lib"

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
		"%{prj.name}/source/**.cpp"
	}

	includedirs
	{
		"%{prj.name}/source",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.ImGUI}",
		"%{IncludeDir.Assimp}",
		"%{IncludeDir.GLM}",
		"%{IncludeDir.entt}",
		"%{IncludeDir.VulkanSDK}"
	}

	links 
	{
		"GLFW",
		"ImGUI",
		"Assimp",
		"%{LibraryDir.VulkanSDK}/vulkan-1.lib"
	}

	defines
	{
		--"_CRT_SECURE_NO_WARNINGS"
		"GLFW_INCLUDE_NONE"
	}

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
			"%{Library.SPIRV_Cross_Debug}",
			"%{Library.SPIRV_Cross_GLSL_Debug}"
		}

	filter "configurations:Release"
		defines "ILG_RELEASE"
		runtime "Release"
		optimize "on"

		links
		{
			"%{Library.ShaderC_Release}",
			"%{Library.SPIRV_Cross_Release}",
			"%{Library.SPIRV_Cross_GLSL_Release}"
		}

	filter "configurations:Dist"
		defines "ILG_DIST"
		runtime "Release"
		optimize "on"

		links
		{
			"%{Library.ShaderC_Release}",
			"%{Library.SPIRV_Cross_Release}",
			"%{Library.SPIRV_Cross_GLSL_Release}",
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
		"Ilargi/dependencies/optick/src",
		"Ilargi/dependencies/glm",
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