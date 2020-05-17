#ifndef GX_DEMO_WWR_MAIN_HPP
#define GX_DEMO_WWR_MAIN_HPP

#include <gearoenix/core/cr-application.hpp>

namespace gearoenix::render::scene {
    class Logo;
}

namespace gearoenix::render::graph::tree {
    class Pbr;
}

namespace gearoenix::demo::wwr {
class Main final: public core::Application {
private:
    std::unique_ptr<render::graph::tree::Pbr> tree;
    std::shared_ptr<render::scene::Logo> logo_scene;
public:
    explicit Main(system::Application* sys_app) noexcept;
};
}

#endif