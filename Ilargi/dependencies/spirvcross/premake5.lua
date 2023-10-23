project "SpirvCrossCore"
	kind "StaticLib"
	language "C++"
	staticruntime "off"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"include/GLSL.std.450.h",
		"include/spirv.hpp",
		"include/spirv_cfg.hpp",
		"include/spirv_common.hpp",
		"include/spirv_cross.hpp",
		"include/spirv_cross_containers.hpp",
		"include/spirv_cross_error_handling.hpp",
		"include/spirv_cross_parsed_ir.hpp",
		"include/spirv_parser.hpp",
		"src/spirv_cfg.cpp",
		"src/spirv_cross.cpp",
		"src/spirv_cross_parsed_ir.cpp",
		"src/spirv_parser.cpp"
	}

	includedirs
	{
		"include"
	}

	filter "system:windows"
		systemversion "latest"

		defines 
		{ 
			"_CRT_SECURE_NO_WARNINGS"
		}

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		runtime "Release"
		optimize "on"

project "SpirvCrossGLSL"
	kind "StaticLib"
	language "C++"
	staticruntime "off"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"include/spirv_glsl.h",
		"src/spirv_glsl.cpp",
	}

	includedirs
	{
		"include"
	}

	links
	{
		"SpirvCrossCore"
	}

	filter "system:windows"
		systemversion "latest"

		defines 
		{ 
			"_CRT_SECURE_NO_WARNINGS"
		}

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		runtime "Release"
		optimize "on"