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

option(PTSD_ENABLE_PCH "Turn on PCH to increase compilation speed" OFF)

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/Dependencies.cmake)

set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(SRC_FILES
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
    ${SRC_DIR}/Util/Root.cpp

    ${SRC_DIR}/Game/Util/Util.cpp
    ${SRC_DIR}/Game/Util/Timer.cpp
    ${SRC_DIR}/Game/Util/Physical.cpp
    ${SRC_DIR}/Game/Util/QuadTree.cpp
    ${SRC_DIR}/Game/Util/Animation.cpp
    ${SRC_DIR}/Game/Util/Animated.cpp
    ${SRC_DIR}/Game/Util/Mirrored.cpp
    ${SRC_DIR}/Game/Util/Offseted.cpp
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

set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(INCLUDE_FILES
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

    ${INCLUDE_DIR}/json.hpp

    ${INCLUDE_DIR}/Game/Util/Util.hpp
    ${INCLUDE_DIR}/Game/Util/Timer.hpp
    ${INCLUDE_DIR}/Game/Util/Physical.hpp
    ${INCLUDE_DIR}/Game/Util/QuadTree.hpp
    ${INCLUDE_DIR}/Game/Util/ObjectPool.hpp
    ${INCLUDE_DIR}/Game/Util/ObjectPool.inl
    ${INCLUDE_DIR}/Game/Util/Animation.hpp
    ${INCLUDE_DIR}/Game/Util/Animated.hpp
    ${INCLUDE_DIR}/Game/Util/Mirrored.hpp
    ${INCLUDE_DIR}/Game/Util/Offseted.hpp
    
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
    
    ${INCLUDE_DIR}/Util/Root.hpp
    ${INCLUDE_DIR}/Util/Color.hpp
    ${INCLUDE_DIR}/Util/Animation.hpp
    ${INCLUDE_DIR}/config.hpp
    ${INCLUDE_DIR}/App.hpp
)

add_executable(${PROJECT_NAME}
    ${SRC_FILES}
)

if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        PTSD_DIR="${CMAKE_CURRENT_SOURCE_DIR}"
    )
else()
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        PTSD_DIR=".."
    )
endif()

target_include_directories(${PROJECT_NAME} SYSTEM PRIVATE
    ${DEPENDENCY_INCLUDE_DIRS}
)
target_link_libraries(${PROJECT_NAME}
    SDL2::SDL2main # IDK what happens if we don't link SDL2main
    ${DEPENDENCY_LINK_LIBRARIES}
)
target_include_directories(${PROJECT_NAME} SYSTEM PRIVATE
    ${DEPENDENCY_INCLUDE_DIRS}
)
target_include_directories(${PROJECT_NAME} PRIVATE
    ${INCLUDE_DIR}
)
if(${PTSD_ENABLE_PCH})
    target_precompile_headers(${PROJECT_NAME} PRIVATE
        include/pch.hpp
    )
endif()
target_compile_options(${PROJECT_NAME} PRIVATE
    ${TARGET_COMPILE_OPTIONS}
)
