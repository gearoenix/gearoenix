#ifndef GEAROENIX_PLATFORM_LINUX_APPLICATION_HPP
#define GEAROENIX_PLATFORM_LINUX_APPLICATION_HPP
#include "../gx-plt-build-configuration.hpp"

#ifdef GX_PLATFORM_LINUX

#include "../../core/macro/gx-cr-mcr-getter-setter.hpp"
#include "../gx-plt-application.hpp"
#include "../gx-plt-runtime-configuration.hpp"
#include <memory>
#include <xcb/xcb.h>

namespace gearoenix::core {
struct Application;
}

namespace gearoenix::platform {
struct Application final {
    GX_GET_CREF_PRV(BaseApplication, base)
    GX_GET_CREF_PRV(std::weak_ptr<Application>, self)
    GX_GET_PTR_PRV(xcb_connection_t, connection)
    GX_GET_VAL_PRV(xcb_window_t, window, 0)

private:
    explicit Application(const RuntimeConfiguration& config) noexcept;

public:
    [[nodiscard]] static std::shared_ptr<Application> construct(const RuntimeConfiguration& config = RuntimeConfiguration()) noexcept;
    ~Application() noexcept;
    void run() noexcept;
};
}
#endif
#endif