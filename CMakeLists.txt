cmake_minimum_required(VERSION 3.7)

project(snowy-january)
include(cmake/StandardProjectSettings.cmake)

# Link this 'library' to set the c++ standard / compile-time options requested
add_library(project_options INTERFACE)
target_compile_features(project_options INTERFACE cxx_std_17)

set(CONAN_EXTRA_REQUIRES "")
set(CONAN_EXTRA_OPTIONS "")

set(CONAN_EXTRA_REQUIRES ${CONAN_EXTRA_REQUIRES}
                           sdl2/2.0.12@bincrafters/stable)

set(CONAN_EXTRA_REQUIRES ${CONAN_EXTRA_REQUIRES}
                           glm/0.9.9.5@g-truc/stable)

set(CONAN_EXTRA_REQUIRES ${CONAN_EXTRA_REQUIRES}
                          bullet3/2.88@bincrafters/stable)

include(cmake/Conan.cmake)
run_conan()

### Find OpenGL
find_package(OpenGL REQUIRED)

add_executable(snowy-january
    README.md
    PROGRESS.md
    include/glad/glad.h
    include/KHR/khrplatform.h
    include/game.h
    include/gl-color-position-vertex.h
    include/gl-color-normal-position-vertex.h
    include/gl-masked-textures.h
    include/gl-obj-renderer.h
    include/tiny_obj_loader.h
    include/capabilityguard.h
    lib/imgui/imgui.cpp
    lib/imgui/imgui.h
    lib/imgui/imgui_draw.cpp
    src/game.cpp
    src/program.cpp
    src/glad.c
    src/snowyjanuary.cpp
    src/snowyjanuary.h
    src/imgui_impl_sdl_gl3.cpp
    src/imgui_impl_sdl_gl3.h
    src/physics.cpp
    src/physics_debug.cpp
    src/physics.h
    src/physicsobject.cpp
    src/physicsobject.h
    src/gameobject.cpp
    src/gameobject.h
    src/stb_image.h
    src/updatingtexture.cpp
    src/updatingtexture.h
    )

target_include_directories(snowy-january
    PRIVATE ${BULLET_INCLUDE_DIR}
    PRIVATE ${GLM_INCLUDE_DIRS}
    PRIVATE include
    PRIVATE lib/imgui
    PRIVATE lib/system.io/include
    )

target_link_libraries(snowy-january
    ${OPENGL_LIBRARIES}
    CONAN_PKG::sdl2
    CONAN_PKG::glm
    CONAN_PKG::bullet3)

target_compile_features(snowy-january
    PRIVATE cxx_auto_type
    PRIVATE cxx_nullptr
    PRIVATE cxx_range_for
    )
