#pragma once
#include "../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED
#include "../render/engine/gx-rnd-eng-engine.hpp"

namespace gearoenix::gl::shader {
struct Manager;
}

namespace gearoenix::gl::submission {
struct Manager;
}

namespace gearoenix::gl {
struct Engine final : render::engine::Engine, core::Singleton<Engine> {
    GX_GET_UPTR_PRV(submission::Manager, submission_manager);
    GX_GET_UPTR_PRV(shader::Manager, shader_manager);

public:
    Engine();
    ~Engine() override;
    void start_frame() override;
    void end_frame() override;
    void window_resized() override;
    void upload_imgui_fonts() override;
    [[nodiscard]] static bool is_supported();
    [[nodiscard]] static std::unique_ptr<Engine> construct();
};
}

#endif