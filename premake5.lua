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
IncludeDir["GLFW"] = "Halo/vendor/GLFW/include"
IncludeDir["Glad"] = "Halo/vendor/Glad/include"
IncludeDir["ImGui"] = "Halo/vendor/imgui"
IncludeDir["glm"] = "Halo/vendor/glm"

include "Halo/vendor/GLFW"
include "Halo/vendor/Glad"
include "Halo/vendor/imgui"

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
		"%{prj.name}/vendor/glm/glm/**.hpp",
		"%{prj.name}/vendor/glm/glm/**.inl",
	}

	defines
	{
		"_CRT_SECURE_NO_WARNINGS"
	}

	includedirs
	{
		"%{prj.name}/src",
		"%{prj.name}/vendor/spdlog/include",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.glm}"
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

project "Sandbox"
	location "Sandbox"
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
		"Halo/vendor/spdlog/include",
		"Halo/src",
		"Halo/vendor",
		"%{IncludeDir.glm}"
	}
	links
	{
		"Halo"
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