#include "../gx-plt-build-configuration.hpp"

/// This header does not have include guards because it should be used several times in a file

#ifdef GX_PLATFORM_WINDOWS
#pragma warning(push, 0)
#else
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wall"
#pragma clang diagnostic ignored "-Wextra"
#pragma clang diagnostic ignored "-Wmissing-field-initializers"
#pragma clang diagnostic ignored "-Wunused-parameter"
#endif