workspace "Halo"
	architecture "x64"
	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}
outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

IncludeDir = {}
IncludeDir["GLFW"] = "Halo/Thirdparty/GLFW/include"
IncludeDir["Glad"] = "Halo/Thirdparty/Glad/include"
IncludeDir["ImGui"] = "Halo/Thirdparty/imgui"
IncludeDir["glm"] = "Halo/Thirdparty/glm"

include "Halo/Thirdparty/GLFW"
include "Halo/Thirdparty/Glad"
include "Halo/Thirdparty/imgui"

project "Halo"
	location "Halo"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("Intermediate/" .. outputdir .. "/%{prj.name}")
	pchheader "hlpch.h"
	pchsource "Halo/src/hlpch.cpp"
	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/Thirdparty/glm/glm/**.hpp",
		"%{prj.name}/Thirdparty/glm/glm/**.inl",
	}

	defines
	{
		"_CRT_SECURE_NO_WARNINGS"
	}

	includedirs
	{
		"%{prj.name}/src",
		"%{prj.name}/Thirdparty/spdlog/include",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.glm}",
		"%{prj.name}/Thirdparty/assimp/include",
		"%{prj.name}/Thirdparty/stb/include"
	}
	links 
	{ 
		"GLFW",
		"Glad",
		"ImGui",
		"opengl32.lib"
	}
	filter "system:windows"
		systemversion "latest"
		defines
		{
			"HL_PLATFORM_WINDOWS",
			"HL_BUILD_DLL",
			"GLFW_INCLUDE_NONE"
		}

	filter "configurations:Debug"
		defines "HL_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "HL_RELEASE"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines "HL_DIST"
		runtime "Release"
		optimize "on"

project "HaloEditor"
	location "HaloEditor"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("Intermediate/" .. outputdir .. "/%{prj.name}")
	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}
	includedirs
	{
		"Halo/Thirdparty/spdlog/include",
		"Halo/src",
		"Halo/Thirdparty",
		"%{IncludeDir.glm}",
		"%{IncludeDir.Glad}"
	}
	links
	{
		"Halo",
		"Halo/Thirdparty/assimp/win64/assimp.lib"
	}
	filter "system:windows"
		cppdialect "C++17"
		systemversion "latest"
		defines
		{
			"HL_PLATFORM_WINDOWS"
		}

	filter "configurations:Debug"
		defines "HL_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "HL_RELEASE"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines "HL_DIST"
		runtime "Release"
		optimize "on"