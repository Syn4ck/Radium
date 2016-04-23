set(library_version "0.1.0")

set( CMAKE_SKIP_BUILD_RPATH FALSE )
set( CMAKE_BUILD_WITH_INSTALL_RPATH FALSE )
set( CMAKE_INSTALL_RPATH ${CMAKE_INSTALL_PREFIX}/lib )
set( CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE )

list( FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "${CMAKE_INSTALL_PREFIX}/lib" isSysDir )
if ( "${isSysDir}" STREQUAL "-1" )
    set( CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib" )
endif( "${isSysDir}" STREQUAL "-1" )

# Find packages
find_package(OpenGL     REQUIRED)
find_package(Qt5Core    REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5OpenGL  REQUIRED)
find_package(GLEW       REQUIRED) #only used on windows builds.

include_directories(
    .
    ${EIGEN_INC}
    ${ASSIMP_INC}
)

# Build Core
set(core_target radiumCore)

file(GLOB_RECURSE core_sources Core/*.cpp)
file(GLOB_RECURSE core_headers Core/*.h Core/*.hpp)
file(GLOB_RECURSE core_inlines Core/*.inl)

set(core_libs ${OPENGL_LIBRARIES})

add_library(
    ${core_target} SHARED
    ${core_sources}
    ${core_headers}
    ${core_inlines}
    )

target_compile_definitions(${core_target} PRIVATE "-DRA_CORE_EXPORTS")

target_link_libraries(
    ${core_target}
    ${core_libs}
    )

# Build Engine
set(engine_target radiumEngine)

file(GLOB_RECURSE engine_sources Engine/*.cpp)
file(GLOB_RECURSE engine_headers Engine/*.h Engine/*.hpp)
file(GLOB_RECURSE engine_inlines Engine/*.inl)

file(GLOB_RECURSE engine_glsl ../Shaders/*.glsl)

set( engine_shaders ${engine_glsl})

set(engine_libs
    radiumCore
    ${GLEW_LIBRARIES}
    ${ASSIMP_LIB}
    ${OPENGL_LIBRARIES})

add_library(
    ${engine_target} SHARED
    ${engine_sources}
    ${engine_headers}
    ${engine_inlines}

    ${engine_shaders}
    )

add_dependencies( ${engine_target} ${core_target} assimp )

target_compile_definitions(${engine_target} PRIVATE "-DRA_ENGINE_EXPORTS")

target_link_libraries(
    ${engine_target}
    ${engine_libs}
    )

set(Qt5_LIBRARIES ${Qt5Core_LIBRARIES} ${Qt5Widgets_LIBRARIES} ${Qt5OpenGL_LIBRARIES})

set(guibase_target radiumGuiBase)

file(GLOB_RECURSE guibase_sources Gui/*.cpp)
file(GLOB_RECURSE guibase_headers Gui/*.h Gui/*.hpp)
file(GLOB_RECURSE guibase_inlines Gui/*.inl)

set(guibase_libs
    radiumCore
    radiumEngine
    ${Qt5_LIBRARIES} # FIXME(charly): This should go away, this is just for plugins
)

add_library(${guibase_target} SHARED ${guibase_sources} ${guibase_headers} ${guibase_inlines})

add_dependencies( ${guibase_target} ${engine_target} ${core_target})
target_compile_definitions(${engine_target} PRIVATE "-DRA_GUIBASE_EXPORTS")
target_link_libraries(${guibase_target} ${guibase_libs})

# Build MainApplication
set(qapp_target qradium)

include(CMakeGenerateVersion.txt)

set(qapp_libs radiumGuiBase radiumEngine radiumCore ${Qt5_LIBRARIES} ${OPENGL_LIBRARIES})

file(GLOB_RECURSE qapp_sources MainApplication/*.cpp)
file(GLOB_RECURSE qapp_headers MainApplication/*.h MainApplication/*.hpp)
file(GLOB_RECURSE qapp_inlines MainApplication/*.inl)

file(GLOB_RECURSE qapp_uis     MainApplication/*.ui)
qt5_wrap_ui(qapp_uis_moc ${qapp_uis})

file(GLOB resources ${CMAKE_SOURCE_DIR}/*.qrc)

# Qt5
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# Fixme : Really usefull ?
add_definitions(${Qt5Core_DEFINITIONS})
add_definitions(${Qt5Widgets_DEFINITIONS})
add_definitions(${Qt5OpenGL_DEFINITIONS})

include_directories(. ${CMAKE_CURRENT_BINARY_DIR}) # MOC

add_executable(${qapp_target} ${qapp_sources} ${qapp_headers} ${qapp_inlines} ${qapp_uis_moc} ${resources})

add_dependencies(qradium radiumCore radiumEngine radiumGuiBase)
target_link_libraries(${qapp_target} ${qapp_libs})

install( DIRECTORY . DESTINATION include FILES_MATCHING PATTERN "*.hpp" PATTERN "*.inl" PATTERN "*.h" )
install( FILES ${engine_shaders} DESTINATION Shaders )
install( DIRECTORY ${CMAKE_SOURCE_DIR}/Assets/. DESTINATION Assets )
install( TARGETS ${qapp_target} ${guibase_target} ${core_target} ${engine_target}
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib )
