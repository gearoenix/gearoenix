#ifndef GEAROENIX_PLATFORM_WINDOWS_KEY_HPP
#define GEAROENIX_PLATFORM_WINDOWS_KEY_HPP
#include "../gx-plt-build-configuration.hpp"
#ifdef GX_PLATFORM_WINDOWS
#include "../gx-plt-key.hpp"
#include <Windows.h>

namespace gearoenix::platform {
[[nodiscard]] key::Id convert_to_keyboard_key(WPARAM, LPARAM) noexcept;
}

#endif
#endif