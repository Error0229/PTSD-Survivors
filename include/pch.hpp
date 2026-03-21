#ifndef PCH_HPP
#define PCH_HPP

#include <algorithm>
#include <array>
#include <map>
#include <memory>
#include <random>
#include <set>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

#include <SDL.h>
#include <SDL_events.h>
#include <SDL_image.h>

#include <SDL_mixer.h>
#include <SDL_ttf.h>

#ifdef __EMSCRIPTEN__
#include <GLES3/gl3.h>
#else
#include <GL/glew.h>
#endif

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/matrix_transform_2d.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/string_cast.hpp>

#ifndef PTSD_DISABLE_IMGUI
#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_sdl2.h>
#endif

#include <nlohmann/json.hpp>

#endif
