#pragma once
#include "../gx-plt-build-configuration.hpp"
#if GX_PLATFORM_INTERFACE_SDL
#include "../gx-plt-key.hpp"
#include <SDL3/SDL.h>

namespace gearoenix::platform {
[[nodiscard]] key::Id convert_sdl_key(SDL_Scancode key);
[[nodiscard]] key::Id convert_sdl_mouse_key(Uint8 key);
}

#endif