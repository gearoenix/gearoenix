#include "gx-mtl-engine.hpp"
#ifdef GX_RENDER_METAL_ENABLED

gearoenix::metal::Engine::Engine(platform::Application& platform_application) noexcept
    : render::engine::Engine(render::engine::Type::Metal, platform_application)
{
}

gearoenix::metal::Engine::~Engine() noexcept = default;

void gearoenix::metal::Engine::start_frame() noexcept {
    render::engine::Engine::start_frame();
}

void gearoenix::metal::Engine::end_frame() noexcept {
    render::engine::Engine::end_frame();
}

void gearoenix::metal::Engine::window_resized() noexcept {
}

void gearoenix::metal::Engine::upload_imgui_fonts() noexcept {
}

bool gearoenix::metal::Engine::is_supported() noexcept {
    return true;
}

std::unique_ptr<gearoenix::metal::Engine> gearoenix::metal::Engine::construct(platform::Application& platform_application) noexcept {
    return std::make_unique<gearoenix::metal::Engine>(platform_application);
}

#endif
