project "XornApp"
  kind "ConsoleApp"
  targetdir ("%{wks.location}/build/%{prj.name}-%{cfg.buildcfg}")
  objdir ("%{wks.location}/build/intermediate/%{prj.name}-%{cfg.buildcfg}")
  systemversion "latest"
  language "C++"
  cppdialect "C++17"
    
  files 
  {
    "src/**.h",
    "src/**.cpp",
	"3rdParty/ImGui/**.h",
	"3rdParty/ImGui/**.cpp"
  }
    
  includedirs
  {
    "src",
    "%{wks.location}/XornCore/src",
    "%{wks.location}/DgLib/src",
	"3rdParty/glad/include/glad",
	"3rdParty/GLFW/include",
	"3rdParty/ImGui"
  }
  
  links
  {
    "DgLib",
	"XornCore",
	"3rdParty/GLFW/bin/glfw3.lib",
	"opengl32.lib"
  }
  
  filter "configurations:Debug"
    runtime "Debug"
    symbols "on"

  filter "configurations:Release"
    runtime "Release"
    optimize "on"