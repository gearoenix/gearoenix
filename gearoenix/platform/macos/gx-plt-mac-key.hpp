#pragma once
#include "../gx-plt-build-configuration.hpp"
#if GX_PLATFORM_INTERFACE_APPKIT
#import "../gx-plt-key.hpp"

namespace gearoenix::platform {

[[nodiscard]] key::Id convert_key(unsigned short key_code);

}

#endif