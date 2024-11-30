#pragma once
#include "../gx-plt-build-configuration.hpp"
#ifdef GX_PLATFORM_INTERFACE_X11
#include "../gx-plt-key.hpp"

namespace gearoenix::platform {
[[nodiscard]] key::Id convert_mouse_to_key(unsigned int);
}

#endif