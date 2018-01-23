cmake_minimum_required(VERSION 3.7)

project(snowy-january)

list(APPEND CMAKE_PREFIX_PATH "$ENV{userprofile}/.cpppm/install")

list(APPEND CMAKE_MODULE_PATH ${CMAKE_PREFIX_PATH}/cmake)

find_package(SDL2 REQUIRED)
find_package(BULLET REQUIRED)
find_package(OPENGL REQUIRED)
find_package(GLM REQUIRED)

add_executable(snowy-january
    include/glad/glad.h
    include/KHR/khrplatform.h
    include/game.h
    include/gl-color-position-vertex.h
    include/gl-color-normal-position-vertex.h
    include/gl-masked-textures.h
    include/tiny_obj_loader.h
    lib/imgui/imgui.cpp
    lib/imgui/imgui.h
    lib/imgui/imgui_draw.cpp
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
    )

target_include_directories(snowy-january
    PRIVATE ${BULLET_INCLUDE_DIR}
    PRIVATE ${GLM_INCLUDE_DIRS}
    PRIVATE include
    PRIVATE lib/imgui
    )

target_link_libraries(snowy-january
    SDL2::SDL2-static
    ${OPENGL_LIBRARIES}
    ${BULLET_LIBRARIES}
    )

target_compile_features(snowy-january
    PRIVATE cxx_auto_type
    PRIVATE cxx_nullptr
    PRIVATE cxx_range_for
    )
