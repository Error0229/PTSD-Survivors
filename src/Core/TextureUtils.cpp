#include "Core/TextureUtils.hpp"

namespace Core {
GLint SdlFormatToGlFormat(Uint32 format) {
    switch (format) {
    case SDL_PIXELFORMAT_RGB24:
        return GL_RGB;
    case SDL_PIXELFORMAT_BGR24:
#ifdef __EMSCRIPTEN__
        return GL_RGB;
#else
        return GL_BGR;
#endif
    case SDL_PIXELFORMAT_XRGB8888:
    case SDL_PIXELFORMAT_ARGB8888:
#ifdef __EMSCRIPTEN__
        return GL_RGBA;
#else
        return GL_BGRA;
#endif
    case SDL_PIXELFORMAT_XBGR8888:
    case SDL_PIXELFORMAT_ABGR8888:
        return GL_RGBA;
    default:
        LOG_ERROR("Format currently unsupported: {}",
                  SDL_GetPixelFormatName(format));
        return -1;
    }
}

GLint GlFormatToGlInternalFormat(GLint format) {
    switch (format) {
    case GL_RGB:
#ifndef __EMSCRIPTEN__
    case GL_BGR:
#endif
#ifdef __EMSCRIPTEN__
        return GL_RGB8;
#else
        return GL_RGB16;
#endif
    case GL_RGBA:
#ifndef __EMSCRIPTEN__
    case GL_BGRA:
#endif
#ifdef __EMSCRIPTEN__
        return GL_RGBA8;
#else
        return GL_RGBA16;
#endif
    default:
        LOG_ERROR("Format currently unsupported");
        return -1;
    }
}
} // namespace Core
