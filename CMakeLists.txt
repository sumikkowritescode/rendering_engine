cmake_minimum_required(VERSION 3.6)
set(project_name "rendering_engine")
project(${project_name})

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

IF(NOT WIN32 OR NOT WIN64)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
ENDIF(NOT WIN32 OR NOT WIN64)

find_package(OpenGL REQUIRED)
find_package(SDL2 REQUIRED)
find_package(GLEW REQUIRED)
find_package(GLM REQUIRED)
find_package(ASSIMP REQUIRED)

include_directories(${OPENGL_INCLUDE_DIR})
include_directories(${SDL2_INCLUDE_DIR})
include_directories(${GLEW_INCLUDE_PATH})
include_directories(${GLM_INCLUDE_DIR})
include_directories(${ASSIMP_INCLUDE_DIR})

file(GLOB SOURCE_FILES
    "src/main.cpp"
    "src/framework/3d/*.cpp"
    "src/framework/3d/*.h"
    "src/framework/3rdparty/imgui/*.cpp"
    "src/framework/3rdparty/imgui/*.h"
    "src/framework/base/*.cpp"
    "src/framework/base/*.h"
    "src/framework/gpu/*.cpp"
    "src/framework/gpu/*.h"
    "src/framework/gpu/*.hpp"
    "src/framework/light/*.cpp"
    "src/framework/light/*.h"
    "src/framework/postprocess/*.cpp"
    "src/framework/postprocess/*.h"
    "src/framework/renderer/*.cpp"
    "src/framework/renderer/*.h"
    "src/framework/scene/*.cpp"
    "src/framework/scene/*.h"
)

add_executable(${project_name} ${SOURCE_FILES})
target_link_libraries(${project_name} ${OPENGL_LIBRARIES} ${SDL2_LIBRARY} ${GLEW_LIBRARY} ${ASSIMP_LIBRARIES})
set_target_properties(${project_name} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin/")

if ( MSVC )
    set_target_properties(${project_name} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin/)
    set_target_properties(${project_name} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG ${PROJECT_SOURCE_DIR}/bin/)
    set_target_properties(${project_name} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE ${PROJECT_SOURCE_DIR}/bin/)

    set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT ${project_name})
endif ( MSVC )

IF(WIN32)
    add_custom_command(TARGET ${project_name} PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/dlls/x86/assimp.dll $<TARGET_FILE_DIR:${project_name}>)
    add_custom_command(TARGET ${project_name} PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/dlls/x86/SDL2.dll $<TARGET_FILE_DIR:${project_name}>)
ENDIF(WIN32)

IF(WIN64)
    add_custom_command(TARGET ${project_name} PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/dlls/x64/assimp.dll $<TARGET_FILE_DIR:${project_name}>)
    add_custom_command(TARGET ${project_name} PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/dlls/x64/SDL2.dll $<TARGET_FILE_DIR:${project_name}>)
ENDIF(WIN64)
