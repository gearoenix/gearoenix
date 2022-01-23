#ifndef GEAROENIX_METAL_ENGINE_HPP
#define GEAROENIX_METAL_ENGINE_HPP
#include "../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_METAL_ENABLED
#include "../render/engine/gx-rnd-eng-engine.hpp"

namespace gearoenix::metal {
    struct Engine final: public render::engine::Engine {
        Engine(platform::Application& platform_application) noexcept;
        ~Engine() noexcept final;
        void start_frame() noexcept final;
        void end_frame() noexcept final;
        void window_resized() noexcept final;
        void upload_imgui_fonts() noexcept final;
        [[nodiscard]] static bool is_supported() noexcept;
        [[nodiscard]] static std::unique_ptr<Engine> construct(platform::Application& platform_application) noexcept;
    };
}

#endif
#endif
