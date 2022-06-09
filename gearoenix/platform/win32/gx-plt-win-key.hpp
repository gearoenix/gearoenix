#ifndef GEAROENIX_PLATFORM_WIN32_KEY_HPP
#define GEAROENIX_PLATFORM_WIN32_KEY_HPP
#include "../gx-plt-build-configuration.hpp"
#ifdef GX_PLATFORM_INTERFACE_WIN32
#include "../gx-plt-key.hpp"
#include <Windows.h>

namespace gearoenix::platform {
[[nodiscard]] key::Id convert_to_keyboard_key(WPARAM, LPARAM) noexcept;
}

#endif
#endif