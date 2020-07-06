-- premake5.lua

workspace "CppUtilities"
  language "C++"
  architecture "x64"   
  location "_local" -- where to place sln-files etc
  targetdir "_local/%{cfg.buildcfg}"
  configurations { "Debug", "Release", "Final" }
  cppdialect "C++17"
  platforms { "Static", "DLL" }

  -- setup the different build configurations
  filter { "platforms:Static" }
    kind "StaticLib"
    defines { "BUILD_COMPILE_STATIC" }

  filter { "platforms:DLL" }
    kind "SharedLib"
    defines { "BUILD_COMPILE_DLL" }   

   filter { "configurations:Debug" }
      defines { "DD_DEBUG", "DEBUG" }
      symbols "On"

   filter {"configurations:Release"}
      defines { "DD_RELEASE", "RELEASE", "NDEBUG" }
      optimize "On"
      symbols "On"

   filter {"configurations:Final"}
      defines { "DD_FINAL", "FINAL", "NDEBUG" }
      optimize "On"
      symbols "Off"

      -- Done with global project settings
group "External"

  project "GoogleTest"
    kind "StaticLib"
    files { "_external/googletest/src/gtest-all.cc" }
    includedirs { "_external/googletest/include", "_external/googletest" }

group ""
      -- Done with external projects

-- Tests

group "Tests"

  project "Core.Test"
    defines { "BUILD_INTERNAL_ACCESS_CORE_MODULE"}
    kind "ConsoleApp"
    files { "Core/_Test/**" }

    links { "Core", "GoogleTest" }
    includedirs { "Core/_Public", "Core", "_external/googletest/include" }

  project "Library.Test"
    defines { "BUILD_INTERNAL_ACCESS_LIBRARY_MODULE"}
    kind "ConsoleApp"
    files { "Library/_Test/**" }

    links { "Library", "Core", "GoogleTest" }
    includedirs { "Library/_Public", "Library", "Core/_Public", "_external/googletest/include" }

group ""

-- Implementations
project "Core"
  defines { "BUILD_EXPORT_CORE_MODULE"}
  files { "Core/**.h", "Core/**.cpp" }
  removefiles { "Core/_Test/**" }

  includedirs { "Core/_Public", "Core" }

project "Library"
  defines { "BUILD_EXPORT_LIBRARY_MODULE"}
  files { "Library/**.h", "Library/**.cpp" }
  removefiles { "Library/_Test/**" }

  links { "Core" }
  includedirs { "Library/_Public", "Library", "Core/_Public" }
