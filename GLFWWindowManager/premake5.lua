include "../Tools/Premake5/premakeDefines.lua"

project "GLFWWindowManager"
	kind "StaticLib"
	language "C++"
	staticruntime "off"

	SetupProject("GLFWWindowManager")
	
	PrecompiledHeaders("GLFWWindowManager")
	
	Link("Utils")
	Link("Core")

	includedirs
	{
		"../Renderer",
		"../%{IncludeDir.GLFW}",
		"../%{IncludeDir.GLM}",
		"../%{IncludeDir.fmt}",
	}

	links 
	{ 
		"GLFW",
		"pugixml",
		"ImGui",
	}

	defines
	{
		"IMGUI_IMPL_OPENGL_LOADER_GLAD"
	}

	filter "system:windows"
		postbuildcommands
		{
			("{COPY} %{cfg.buildtarget.relpath} \"%{wks.location}/bin/" .. outputdir .. "/Sandbox/\""),
		}

	filter "system:linux"
		pic "On"
