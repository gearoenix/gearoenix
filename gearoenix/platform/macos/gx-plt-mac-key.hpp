#ifndef GEAROENIX_PLATFORM_MACOS_KEY_HPP
#define GEAROENIX_PLATFORM_MACOS_KEY_HPP
#include "../gx-plt-build-configuration.hpp"
#ifdef GX_PLATFORM_MACOS
#import "../gx-plt-key.hpp"

namespace gearoenix::platform {

[[nodiscard]] key::Id convert_key(unsigned short key_code) noexcept;

}

#endif
#endif
