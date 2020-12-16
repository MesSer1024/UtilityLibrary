-- UtilityLibrary/premake5.lua

-- <Workspace Settings>
workspace "CppUtilities"
	language "C++"
	architecture "x64"   
	cppdialect "C++17"
	warnings "Extra"
	disablewarnings { "4100" } -- unused parameter value (input to function)
	
	location "local" -- where to place obj + sln-files etc
	targetdir "bin/%{cfg.buildcfg}" -- output directory
	
	configurations { "Debug", "Final" }
	platforms { "Static", "DLL" }
	
	filter { "platforms:Static" }
		kind "StaticLib"
		defines { "BUILD_COMPILE_STATIC" }

	filter { "platforms:DLL" }
		kind "SharedLib"
		defines { "BUILD_COMPILE_DLL" }   

	filter { "configurations:Debug" }
		defines { "DD_DEBUG", "DEBUG" }
		symbols "On"

	filter {"configurations:Final"}
		defines { "DD_FINAL", "FINAL", "NDEBUG" }
		optimize "On"
		symbols "Off"
-- </Workspace Settings>

-- <UtilityFunctions>
function DeclareProject(identifier, projectType)
	project (identifier)
	if projectType ~= nil then kind (projectType) end
	
	files { "source/" .. identifier .. "/**" }
	includedirs { "source/" .. identifier, "source/" .. identifier .. "/Public"  }
end

function DeclareTestProject(identifier)
	project (identifier)
	kind "ConsoleApp"
	
	files { "source/" .. identifier .. "/**" }
	includedirs { "source/" .. identifier, "ExternalLibs/googletest/include" }		
	links { "GoogleTest" }
end

function AddOneDependency(name)
	links { name }
	includedirs { "source/" .. name .. "/Public" }
end

function AddDependency(...)
   local arg = {...}
   for i,v in ipairs(arg) do
      AddOneDependency(v)
   end
end
-- </UtilityFunctions>

-- <External>
group "External"
	project "GoogleTest"
		kind "StaticLib"
		files { "ExternalLibs/googletest/src/gtest-all.cc" }
		includedirs { "ExternalLibs/googletest/include", "ExternalLibs/googletest" }

group ""
-- </External>

-- <Tests>
group "Tests"
	DeclareTestProject("Core.Test")
		AddDependency("Core")
		defines { "BUILD_INTERNAL_ACCESS_CORE_MODULE"}

	DeclareTestProject("Library.Test")
		AddDependency("Library", "Core")
		defines { "BUILD_INTERNAL_ACCESS_LIBRARY_MODULE"}
group ""
-- </Tests>

-- <Implementations>
DeclareProject("Core")
  defines { "BUILD_EXPORT_CORE_MODULE"}

DeclareProject("Library")
	AddDependency("Core")
	defines { "BUILD_EXPORT_LIBRARY_MODULE"}
-- </Implementations>