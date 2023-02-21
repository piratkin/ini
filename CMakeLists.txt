cmake_minimum_required(VERSION 3.16)

project(ini CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON
    CACHE INTERNAL "")

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_VERBOSE_MAKEFILE 1)
    add_compile_definitions(_DEBUG)
endif()

if (MSVC)
    add_definitions(/MP)
    set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})
    set_property(GLOBAL PROPERTY USE_FOLDERS ON)	
endif()

add_executable(
    ${PROJECT_NAME}
    main.cpp
)

set_target_properties(
    ${PROJECT_NAME}
    PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS ON
)
