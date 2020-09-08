include "../premakeDefines.lua"

project "VulkanRenderer"
	kind "SharedLib"
	language "C++"
	staticruntime "off"

	targetdir ("../bin/" .. outputdir .. "/%{prj.name}")
	objdir ("../obj/" .. outputdir .. "/%{prj.name}")
	
	pchheader "VkRendererStdafx.h"
	pchsource "VkRendererStdafx.cpp"

	files
	{
		"VulkanRenderer/**.h",
		"VulkanRenderer/**.cpp",
		"VulkanRenderer/**.inl",
		"VkRendererStdafx.cpp",
		"VkRendererStdafx.h",
		"premake5.lua",
	}
	
	Link("Renderer")
	Link("Utils")
	Link("GLFWWindowManager")
	Link("Core")
	Link("Entity")

	includedirs
	{
		".",
		"../Physics",
		"%{VulkanSDKBase}Include",
		"../%{IncludeDir.pugixml}",
		"../%{IncludeDir.fmt}",
		"../%{IncludeDir.ImGui}",
		"../%{IncludeDir.GLM}",
		"../%{IncludeDir.GLFW}",
	}

	libdirs { "%{VulkanSDKBase}/Lib" }

	links 
	{ 
		"pugixml",
		"ImGui",
		"vulkan-1.lib",
	}

	filter "system:windows"
		defines
		{
			"BUILD_VULKAN_RENDERER_DLL",
		}

		postbuildcommands
		{
			("{COPY} %{cfg.buildtarget.relpath} \"../bin/" .. outputdir .. "/Sandbox/\""),
		}