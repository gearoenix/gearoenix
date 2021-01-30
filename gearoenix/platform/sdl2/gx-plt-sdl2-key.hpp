#ifndef GEAROENIX_PLATFORM_SDL2_KEY_HPP
#define GEAROENIX_PLATFORM_SDL2_KEY_HPP
#include "../gx-plt-build-configuration.hpp"
#ifdef GX_PLATFORM_INTERFACE_SDL2
#include "../gx-plt-key.hpp"

namespace gearoenix::platform {
[[nodiscard]] key::Id convert_mouse_to_key(unsigned int) noexcept;
}

#endif
#endif