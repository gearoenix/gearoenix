#ifndef GEAROENIX_PLATFORM_ANDROID_KEY_HPP
#define GEAROENIX_PLATFORM_ANDROID_KEY_HPP
#include "../gx-plt-build-configuration.hpp"
#ifdef GX_PLATFORM_INTERFACE_ANDROID
#include "../gx-plt-key.hpp"
#include <cstdint>

namespace gearoenix::platform {
key::Id convert_android_key(std::int32_t key_code);
}

#endif
#endif