#ifndef GEAROENIX_PLATFORM_SDL2_KEY_HPP
#define GEAROENIX_PLATFORM_SDL2_KEY_HPP
#include "../gx-plt-build-configuration.hpp"
#ifdef GX_PLATFORM_INTERFACE_SDL2
#include "../gx-plt-key.hpp"
#ifdef GX_PLATFORM_LINUX
#include <SDL2/SDL_keycode.h>
#else
#include <SDL_keycode.h>
#endif
namespace gearoenix::platform {
[[nodiscard]] key::Id convert_sdl_key(SDL_Keycode key) noexcept;
[[nodiscard]] key::Id convert_sdl_mouse_key(Uint8 key) noexcept;
}

#endif
#endif