cmake_minimum_required(VERSION 3.21)

project(jmc
  VERSION 3.7.0
  DESCRIPTION "Jaba Mud Client"
  LANGUAGES C CXX)

# Top-level-only defaults: keep these from overriding a parent project if jmc is
# ever embedded, and respect toolchain/user choices.
if(PROJECT_IS_TOP_LEVEL)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
  set(CMAKE_CXX_EXTENSIONS OFF)
  set(CMAKE_EXPORT_COMPILE_COMMANDS ON)   # for clangd / IDE tooling

  # Single-config generators (Ninja, Makefiles) have no default build type;
  # an empty type means an unoptimized, no-debug-info build. Pick a sane one.
  if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING "Build type" FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
      Debug Release RelWithDebInfo MinSizeRel)
  endif()
endif()

add_subdirectory(sources)
