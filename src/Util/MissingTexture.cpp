#include <SDL.h>
#include <SDL_image.h>
#include <Util/Logger.hpp>

#include "Util/MissingTexture.hpp"

namespace Util {
SDL_Surface *GetMissingImageTextureSDLSurface() {
    SDL_RWops *rwop =
        SDL_RWFromConstMem(MISSING_IMAGE_TEXTURE_BASE64_DECODE.data(),
                           MISSING_IMAGE_TEXTURE_BASE64_DECODE.size());
    SDL_Surface *aSurface = IMG_LoadTyped_RW(rwop, 1, "PNG");

    if (aSurface == nullptr) {
        LOG_ERROR("base64ToSurface");
        return nullptr;
    }

    // Convert to RGBA32 to avoid INDEX8/palette formats that GL can't handle
    SDL_Surface *converted =
        SDL_ConvertSurfaceFormat(aSurface, SDL_PIXELFORMAT_ABGR8888, 0);
    SDL_FreeSurface(aSurface);
    if (converted == nullptr) {
        LOG_ERROR("Failed to convert missing texture to RGBA: {}",
                  SDL_GetError());
    }
    return converted;
}

SDL_Surface *GetMissingFontTextureSDLSurface() {
    SDL_RWops *rwop =
        SDL_RWFromConstMem(MISSING_FONT_TEXTURE_BASE64_DECODE.data(),
                           MISSING_FONT_TEXTURE_BASE64_DECODE.size());
    SDL_Surface *aSurface = IMG_LoadTyped_RW(rwop, 1, "JPG");

    if (aSurface == nullptr) {
        LOG_ERROR("base64ToSurface");
        return nullptr;
    }

    // Convert to RGBA32 to avoid unsupported formats
    SDL_Surface *converted =
        SDL_ConvertSurfaceFormat(aSurface, SDL_PIXELFORMAT_ABGR8888, 0);
    SDL_FreeSurface(aSurface);
    if (converted == nullptr) {
        LOG_ERROR("Failed to convert missing font texture to RGBA: {}",
                  SDL_GetError());
    }
    return converted;
}
} // namespace Util
