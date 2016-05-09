project(ImGuiPlugin)

cmake_minimum_required(VERSION 2.8)
if ( NOT PLUGIN_OUTPUT_PATH )
  set ( PLUGIN_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/Plugins/bin )
else()
  message( "Output path : ${PLUGIN_OUTPUT_PATH}" )
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PLUGIN_OUTPUT_PATH} )
set(EXECUTABLE_OUTPUT_PATH         ${PLUGIN_OUTPUT_PATH} )
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PLUGIN_OUTPUT_PATH} )
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PLUGIN_OUTPUT_PATH} )

if ( NOT CMAKE_PREFIX_PATH )
  set ( CMAKE_PREFIX_PATH ${CMAKE_SOURCE_DIR} )
endif()

# WIN32

file( GLOB_RECURSE sources src/*.cpp )
file( GLOB_RECURSE headers src/*.hpp )

include_directories( src/ )

add_library( ImGui SHARED ${sources} ${headers} )

