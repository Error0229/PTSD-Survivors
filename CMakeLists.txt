cmake_minimum_required(VERSION 3.16)
set(PROJECT_NAME PTSD-Survivors)
project(${PROJECT_NAME})

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
if (MSVC)
    set(TARGET_COMPILE_OPTIONS
        /W4
    )
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/build)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_SOURCE_DIR}/build)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_SOURCE_DIR}/build)
else()
    set(TARGET_COMPILE_OPTIONS
        -Wall -Wextra -pedantic -O0 -g -fno-omit-frame-pointer
    )
endif()

if (WIN32 AND CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    # see https://github.com/ntut-open-source-club/practical-tools-for-simple-design/issues/22
    set(CMAKE_RC_FLAGS="-C 1252")
endif()

option(PTSD_ENABLE_PCH "Turn on PCH to increase compilation speed" OFF)

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/Dependencies.cmake)

set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(SRC_FILES
    ${SRC_DIR}/config.cpp

    ${SRC_DIR}/Core/Context.cpp
    ${SRC_DIR}/Core/DebugMessageCallback.cpp
    ${SRC_DIR}/Core/VertexArray.cpp
    ${SRC_DIR}/Core/VertexBuffer.cpp
    ${SRC_DIR}/Core/IndexBuffer.cpp
    ${SRC_DIR}/Core/Shader.cpp
    ${SRC_DIR}/Core/Program.cpp
    ${SRC_DIR}/Core/Texture.cpp
    ${SRC_DIR}/Core/TextureUtils.cpp

    ${SRC_DIR}/Util/LoadTextFile.cpp
    ${SRC_DIR}/Util/Logger.cpp
    ${SRC_DIR}/Util/Time.cpp
    ${SRC_DIR}/Util/Input.cpp
    ${SRC_DIR}/Util/SFX.cpp
    ${SRC_DIR}/Util/BGM.cpp
    ${SRC_DIR}/Util/Image.cpp
    ${SRC_DIR}/Util/Text.cpp
    ${SRC_DIR}/Util/TransformUtils.cpp
    ${SRC_DIR}/Util/GameObject.cpp
    ${SRC_DIR}/Util/Renderer.cpp
    ${SRC_DIR}/Util/Color.cpp
    ${SRC_DIR}/Util/Animation.cpp
    ${SRC_DIR}/Util/MissingTexture.cpp
    ${SRC_DIR}/Util/Position.cpp
)
set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(INCLUDE_FILES
    ${INCLUDE_DIR}/pch.hpp
    ${INCLUDE_DIR}/config.hpp

    ${INCLUDE_DIR}/Core/Context.hpp
    ${INCLUDE_DIR}/Core/DebugMessageCallback.hpp
    ${INCLUDE_DIR}/Core/VertexArray.hpp
    ${INCLUDE_DIR}/Core/VertexBuffer.hpp
    ${INCLUDE_DIR}/Core/UniformBuffer.hpp
    ${INCLUDE_DIR}/Core/UniformBuffer.inl
    ${INCLUDE_DIR}/Core/IndexBuffer.hpp
    ${INCLUDE_DIR}/Core/Shader.hpp
    ${INCLUDE_DIR}/Core/Program.hpp
    ${INCLUDE_DIR}/Core/Texture.hpp
    ${INCLUDE_DIR}/Core/TextureUtils.hpp
    ${INCLUDE_DIR}/Core/Drawable.hpp
    ${INCLUDE_DIR}/Core/MissingFontTextureBase64.hpp
    ${INCLUDE_DIR}/Core/MissingImageTextureBase64.hpp

    ${INCLUDE_DIR}/Util/LoadTextFile.hpp
    ${INCLUDE_DIR}/Util/Logger.hpp
    ${INCLUDE_DIR}/Util/Time.hpp
    ${INCLUDE_DIR}/Util/Input.hpp
    ${INCLUDE_DIR}/Util/Keycode.hpp
    ${INCLUDE_DIR}/Util/SFX.hpp
    ${INCLUDE_DIR}/Util/BGM.hpp
    ${INCLUDE_DIR}/Util/Image.hpp
    ${INCLUDE_DIR}/Util/Text.hpp
    ${INCLUDE_DIR}/Util/Transform.hpp
    ${INCLUDE_DIR}/Util/TransformUtils.hpp
    ${INCLUDE_DIR}/Util/GameObject.hpp
    ${INCLUDE_DIR}/Util/Renderer.hpp
    ${INCLUDE_DIR}/Util/Color.hpp
    ${INCLUDE_DIR}/Util/MissingTexture.hpp
    ${INCLUDE_DIR}/Util/Base64.hpp
    ${INCLUDE_DIR}/Util/Animation.hpp
    ${INCLUDE_DIR}/Util/Position.hpp
)
set(EXAMPLE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/example)
set(EXAMPLE_FILES
    ${EXAMPLE_DIR}/src/App.cpp
    ${EXAMPLE_DIR}/src/Giraffe.cpp
    ${EXAMPLE_DIR}/src/GiraffeText.cpp
    ${EXAMPLE_DIR}/src/Cat.cpp
    ${EXAMPLE_DIR}/src/main.cpp

    ${EXAMPLE_DIR}/include/App.hpp
    ${EXAMPLE_DIR}/include/Giraffe.hpp
    ${EXAMPLE_DIR}/include/GiraffeText.hpp
    ${EXAMPLE_DIR}/include/Cat.hpp
)
set(TEST_DIR ${CMAKE_CURRENT_SOURCE_DIR}/test)
set(TEST_FILES
    ${TEST_DIR}/SimpleTest.cpp
    ${TEST_DIR}/NotSimpleTest.cpp
    ${TEST_DIR}/TransformTest.cpp
)

add_library(PTSD STATIC
    ${SRC_FILES}
    ${INCLUDE_FILES}
)
target_link_libraries(PTSD
    ${DEPENDENCY_LINK_LIBRARIES}
)
target_include_directories(PTSD SYSTEM PRIVATE
    ${DEPENDENCY_INCLUDE_DIRS}
)
target_include_directories(PTSD PRIVATE
    ${INCLUDE_DIR}
)
if(EMSCRIPTEN)
    target_compile_definitions(PTSD PRIVATE PTSD_ASSETS_DIR="/assets" PTSD_DISABLE_IMGUI FMT_CONSTEVAL=constexpr)
    target_compile_options(PTSD PRIVATE
        -sUSE_SDL=2
        -sUSE_SDL_IMAGE=2
        -sUSE_SDL_TTF=2
        -sUSE_SDL_MIXER=2
    )
elseif(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(PTSD PRIVATE PTSD_ASSETS_DIR="${CMAKE_CURRENT_SOURCE_DIR}/assets")
else()
    message(AUTHOR_WARNING "relative PTSD_ASSETS_DIR is WIP, Please use `-DCMAKE_BUILD_TYPE=Debug` build for now.")
    target_compile_definitions(PTSD PRIVATE PTSD_ASSETS_DIR="${CMAKE_CURRENT_SOURCE_DIR}/assets")
endif()
if (${PTSD_ENABLE_PCH})
    target_precompile_headers(PTSD PRIVATE
        include/pch.hpp
    )
endif()

target_compile_options(PTSD PRIVATE
    ${TARGET_COMPILE_OPTIONS}
)

if(NOT PROJECT_IS_TOP_LEVEL)
    set(DEPENDENCY_INCLUDE_DIRS ${DEPENDENCY_INCLUDE_DIRS} PARENT_SCOPE)
    set(DEPENDENCY_LINK_LIBRARIES ${DEPENDENCY_LINK_LIBRARIES} PARENT_SCOPE)
endif()

if(NOT EMSCRIPTEN)
    add_executable(Example EXCLUDE_FROM_ALL
        ${EXAMPLE_FILES}
    )
    target_link_libraries(Example
        SDL2::SDL2main
        PTSD
    )
    target_include_directories(Example SYSTEM PRIVATE
        ${INCLUDE_DIR}
        ${DEPENDENCY_INCLUDE_DIRS}
    )
    target_include_directories(Example PRIVATE
        ${EXAMPLE_DIR}/include
    )
    target_compile_definitions(Example PRIVATE ASSETS_DIR="${CMAKE_CURRENT_SOURCE_DIR}/assets")
endif()

set(GAME_SRC_FILES
    ${SRC_DIR}/Game/Util/Util.cpp
    ${SRC_DIR}/Game/Util/Timer.cpp
    ${SRC_DIR}/Game/Util/Physical.cpp
    ${SRC_DIR}/Game/Util/QuadTree.cpp
    ${SRC_DIR}/Game/Util/Animation.cpp
    ${SRC_DIR}/Game/Util/Animated.cpp
    ${SRC_DIR}/Game/Util/Mirrored.cpp
    ${SRC_DIR}/Game/Util/Offsetted.cpp
    ${SRC_DIR}/Game/Passive/Passive.cpp
    ${SRC_DIR}/Game/Projectile/Projectile.cpp
    ${SRC_DIR}/Game/Projectile/Whip.cpp
    ${SRC_DIR}/Game/Projectile/Vampirica.cpp
    ${SRC_DIR}/Game/Weapon/Weapon.cpp
    ${SRC_DIR}/Game/Weapon/Whip.cpp
    ${SRC_DIR}/Game/Weapon/Vampirica.cpp
    ${SRC_DIR}/Game/Enemy/Enemy.cpp
    ${SRC_DIR}/Game/Camera.cpp
    ${SRC_DIR}/Game/Map.cpp
    ${SRC_DIR}/Game/Character.cpp
    ${SRC_DIR}/Game/Resource.cpp
    ${SRC_DIR}/Game/Manager.cpp

    ${SRC_DIR}/App.cpp
    ${SRC_DIR}/main.cpp
)
set(GAME_INCLUDE_FILES
    ${INCLUDE_DIR}/Game/Util/Util.hpp
    ${INCLUDE_DIR}/Game/Util/Timer.hpp
    ${INCLUDE_DIR}/Game/Util/Physical.hpp
    ${INCLUDE_DIR}/Game/Util/QuadTree.hpp
    ${INCLUDE_DIR}/Game/Util/ObjectPool.hpp
    ${INCLUDE_DIR}/Game/Util/ObjectPool.inl
    ${INCLUDE_DIR}/Game/Util/Animation.hpp
    ${INCLUDE_DIR}/Game/Util/Animated.hpp
    ${INCLUDE_DIR}/Game/Util/Mirrored.hpp
    ${INCLUDE_DIR}/Game/Util/Offsetted.hpp

    ${INCLUDE_DIR}/Game/Passive/Passive.hpp
    ${INCLUDE_DIR}/Game/Projectile/Projectile.hpp
    ${INCLUDE_DIR}/Game/Projectile/Whip.hpp
    ${INCLUDE_DIR}/Game/Projectile/Vampirica.hpp
    ${INCLUDE_DIR}/Game/Weapon/Weapon.hpp
    ${INCLUDE_DIR}/Game/Weapon/Whip.hpp
    ${INCLUDE_DIR}/Game/Weapon/Vampirica.hpp
    ${INCLUDE_DIR}/Game/Enemy/Enemy.hpp
    ${INCLUDE_DIR}/Game/Camera.hpp
    ${INCLUDE_DIR}/Game/Map.hpp
    ${INCLUDE_DIR}/Game/Character.hpp
    ${INCLUDE_DIR}/Game/Resource.hpp
    ${INCLUDE_DIR}/Game/Manager.hpp

    ${INCLUDE_DIR}/App.hpp
)

add_executable(Sample
    ${GAME_SRC_FILES}
    ${GAME_INCLUDE_FILES}
)
if(EMSCRIPTEN)
    target_link_libraries(Sample PTSD)
    target_compile_definitions(Sample PRIVATE PTSD_DISABLE_IMGUI)
    target_compile_options(Sample PRIVATE
        -sUSE_SDL=2
        -sUSE_SDL_IMAGE=2
        -sUSE_SDL_TTF=2
        -sUSE_SDL_MIXER=2
    )
    set_target_properties(Sample PROPERTIES SUFFIX ".html")
    target_link_options(Sample PRIVATE
        -sUSE_SDL=2
        -sUSE_SDL_IMAGE=2
        -sSDL2_IMAGE_FORMATS=["png"]
        -sUSE_SDL_TTF=2
        -sUSE_SDL_MIXER=2
        -sMIN_WEBGL_VERSION=2
        -sMAX_WEBGL_VERSION=2
        -sALLOW_MEMORY_GROWTH=1
        -sFULL_ES3=1
        "SHELL:--preload-file ${CMAKE_CURRENT_SOURCE_DIR}/assets@/assets"
        "SHELL:--preload-file ${CMAKE_CURRENT_SOURCE_DIR}/resources@/resources"
    )
else()
    target_link_libraries(Sample SDL2::SDL2main PTSD)
endif()
target_include_directories(Sample SYSTEM PRIVATE
    ${INCLUDE_DIR}
    ${DEPENDENCY_INCLUDE_DIRS}
)
target_include_directories(Sample PRIVATE
    ${INCLUDE_DIR}
)
target_compile_definitions(Sample PRIVATE ASSETS_DIR="${CMAKE_CURRENT_SOURCE_DIR}/assets")
target_compile_options(Sample PRIVATE
    ${TARGET_COMPILE_OPTIONS}
)

enable_testing()

add_executable(Tests EXCLUDE_FROM_ALL
    ${TEST_FILES}
)
target_link_libraries(Tests
    PTSD
    GTest::gtest_main
)
target_include_directories(Tests SYSTEM PRIVATE
    ${INCLUDE_DIR}
    ${DEPENDENCY_INCLUDE_DIRS}
    lib/googletest/googletest/include
    lib/googletest/googlemock/include
)
target_compile_options(Tests PRIVATE
    ${TARGET_COMPILE_OPTIONS}
)

add_executable(AudioTest EXCLUDE_FROM_ALL
    ${TEST_DIR}/Interactive/Audio.cpp
)
target_link_libraries(AudioTest
    PTSD
    GTest::gtest_main
)
target_include_directories(AudioTest SYSTEM PRIVATE
    ${INCLUDE_DIR}
    ${DEPENDENCY_INCLUDE_DIRS}
    lib/googletest/googletest/include
    lib/googletest/googlemock/include
)
target_compile_options(AudioTest PRIVATE
    ${TARGET_COMPILE_OPTIONS}
)

include(GoogleTest)
gtest_discover_tests(Tests)
