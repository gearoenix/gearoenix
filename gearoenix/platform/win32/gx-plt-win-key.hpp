#pragma once
#include "../gx-plt-build-configuration.hpp"
#ifdef GX_PLATFORM_INTERFACE_WIN32
#include "../gx-plt-key.hpp"
#include <Windows.h>

namespace gearoenix::platform {
[[nodiscard]] key::Id convert_to_keyboard_key(WPARAM, LPARAM);
}

#endif