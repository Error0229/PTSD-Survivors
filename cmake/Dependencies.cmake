cmake_policy(SET CMP0135 NEW)
include(FetchContent)
set(FETCH_CONTENT_QUIET FALSE)

# --- Dependencies that are always fetched ---

FetchContent_Declare(
    spdlog
    URL         https://github.com/gabime/spdlog/archive/refs/tags/v1.10.0.zip
    URL_HASH    MD5=031565384b28f29e44c6e7fb247ad48a
    SOURCE_DIR  ${CMAKE_CURRENT_SOURCE_DIR}/lib/spdlog
)

FetchContent_Declare(
    glm
    URL         https://github.com/g-truc/glm/releases/download/1.0.1/glm-1.0.1-light.7z
    SOURCE_DIR  ${CMAKE_CURRENT_SOURCE_DIR}/lib/glm
)

FetchContent_Declare(
    googletest
    URL         https://github.com/google/googletest/archive/refs/tags/v1.13.0.zip
    URL_HASH    MD5=a1279c6fb5bf7d4a5e0d0b2a4adb39ac
    SOURCE_DIR  ${CMAKE_CURRENT_SOURCE_DIR}/lib/googletest
)

FetchContent_Declare(
    nlohmann_json
    URL         https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz
    URL_HASH    MD5=c23a33f04786d85c29fda8d16b5f0efd
    SOURCE_DIR  ${CMAKE_CURRENT_SOURCE_DIR}/lib/nlohmann_json
)

set(JSON_ImplicitConversions OFF)
set(BUILD_SHARED_LIBS FALSE)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

if(EMSCRIPTEN)
    # --- Emscripten: SDL2/GL provided by ports, no GLEW/ImGui ---
    # Workaround: spdlog 1.10.0 bundled fmt uses consteval which fails on clang 19+
    add_compile_definitions(FMT_CONSTEVAL=constexpr)
    FetchContent_MakeAvailable(spdlog glm googletest nlohmann_json)

    set(DEPENDENCY_LINK_LIBRARIES
        spdlog::spdlog
        nlohmann_json
    )

    set(DEPENDENCY_INCLUDE_DIRS
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/spdlog/include/
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/nlohmann_json/include/
    )
else()
    # --- Desktop: fetch all dependencies ---
    find_package(OpenGL REQUIRED)

    FetchContent_Declare(
        glew
        GIT_REPOSITORY  https://github.com/Perlmint/glew-cmake.git
        GIT_TAG         918ece3da858c2e28e10f6507378af01647cb139
        SOURCE_DIR      ${CMAKE_CURRENT_SOURCE_DIR}/lib/glew
    )

    FetchContent_Declare(
        sdl2
        URL         https://github.com/libsdl-org/SDL/releases/download/release-2.32.4/SDL2-2.32.4.zip
        SOURCE_DIR  ${CMAKE_CURRENT_SOURCE_DIR}/lib/sdl2
    )

    FetchContent_Declare(
        sdl2_image
        URL         https://github.com/libsdl-org/SDL_image/releases/download/release-2.6.3/SDL2_image-2.6.3.zip
        URL_HASH    MD5=ecedb5078bbd31e7d1552e2b1443d2f6
        SOURCE_DIR  ${CMAKE_CURRENT_SOURCE_DIR}/lib/sdl2_image
    )

    FetchContent_Declare(
        sdl2_ttf
        GIT_REPOSITORY  https://github.com/libsdl-org/SDL_ttf.git
        GIT_TAG         db1e6d14494bfffe464c50667a26941cc3a36324
        SOURCE_DIR  ${CMAKE_CURRENT_SOURCE_DIR}/lib/sdl2_ttf
    )

    FetchContent_Declare(
        sdl2_mixer
        URL         https://github.com/libsdl-org/SDL_mixer/releases/download/release-2.6.3/SDL2_mixer-2.6.3.zip
        URL_HASH    MD5=fb3e71ef072ff8dd793cec3ed384f9a0
        SOURCE_DIR  ${CMAKE_CURRENT_SOURCE_DIR}/lib/sdl2_mixer
    )

    FetchContent_Declare(
        imgui
        URL         https://github.com/ocornut/imgui/archive/refs/tags/v1.90.4-docking.zip
        URL_HASH    MD5=384084df566474aec3729df4ea30b937
        SOURCE_DIR  ${CMAKE_CURRENT_SOURCE_DIR}/lib/imgui
    )

    set(SDL2IMAGE_INSTALL OFF)
    set(SDL2IMAGE_VENDORED ON)
    set(SDL2TTF_INSTALL OFF)
    set(SDL2TTF_VENDORED ON)
    set(SDL2MIXER_INSTALL OFF)
    set(SDL2MIXER_VENDORED ON)
    set(SDL2MIXER_FLAC OFF)
    set(SDL2MIXER_MIDI OFF)
    set(SDL2MIXER_MOD OFF)
    set(SDL2MIXER_OPUS OFF)
    set(SDL2MIXER_OGG OFF)
    set(SDL2MIXER_VORBIS OFF)
    set(SDL2MIXER_VOC OFF)

    add_compile_definitions(GLEW_NO_GLU)

    FetchContent_MakeAvailable(glew sdl2 sdl2_image sdl2_ttf sdl2_mixer spdlog glm googletest imgui nlohmann_json)

    set(IMGUI_SOURCE
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/imgui/backends/imgui_impl_sdl2.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/imgui/backends/imgui_impl_opengl3.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/imgui/imgui.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/imgui/imgui_demo.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/imgui/imgui_draw.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/imgui/imgui_tables.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/imgui/imgui_widgets.cpp
    )

    set(IMGUI_INCLUDE_DIR
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/imgui/
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/imgui/backends/
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/sdl2/include/
    )

    add_library(ImGui STATIC ${IMGUI_SOURCE})
    target_include_directories(ImGui PUBLIC ${IMGUI_INCLUDE_DIR})

    set(DEPENDENCY_LINK_LIBRARIES
        ${OPENGL_LIBRARY}
        libglew_static
        SDL2::SDL2-static
        SDL2_image::SDL2_image-static
        SDL2_ttf::SDL2_ttf-static
        SDL2_mixer::SDL2_mixer-static
        spdlog::spdlog
        ImGui
        nlohmann_json
    )

    set(DEPENDENCY_INCLUDE_DIRS
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/sdl2/include/
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/glew/include/
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/spdlog/include/
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/
        ${IMGUI_INCLUDE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/nlohmann_json/include/
    )
endif()
