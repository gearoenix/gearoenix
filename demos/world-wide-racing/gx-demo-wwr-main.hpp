#ifndef GX_DEMO_WWR_MAIN_HPP
#define GX_DEMO_WWR_MAIN_HPP

#include <gearoenix/core/cr-application.hpp>

namespace gearoenix::demo::wwr {
class Main final: public core::Application {
public:
    explicit Main(system::Application* sys_app) noexcept;
};
}

#endif