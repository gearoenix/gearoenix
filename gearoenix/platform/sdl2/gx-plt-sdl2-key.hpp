#pragma once
#include "../gx-plt-build-configuration.hpp"
#ifdef GX_PLATFORM_INTERFACE_SDL2
#include "../gx-plt-key.hpp"
#include <SDL2/SDL_keycode.h>

namespace gearoenix::platform {
[[nodiscard]] key::Id convert_sdl_key(SDL_Keycode key);
[[nodiscard]] key::Id convert_sdl_mouse_key(Uint8 key);
}

#endif