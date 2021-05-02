#ifndef GEAROENIX_BGFX_ENGINE_HPP
#define GEAROENIX_BGFX_ENGINE_HPP
#include "../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_BGFX_ENABLED
#include "../render/engine/gx-rnd-eng-engine.hpp"

namespace gearoenix::bgfx {
struct Frame;
struct Engine final : public render::engine::Engine {
private:
public:
    Engine(Engine&&) = delete;
    Engine(const Engine&) = delete;
    Engine& operator=(Engine&&) = delete;
    Engine& operator=(const Engine&) = delete;

    explicit Engine(platform::Application& platform_application) noexcept;
    ~Engine() noexcept final;
    void start_frame() noexcept final;
    void end_frame() noexcept final;
    void upload_imgui_fonts() noexcept final;
    void create_mesh(
        const std::string& name,
        const std::vector<math::BasicVertex>& vertices,
        const std::vector<std::uint32_t>& indices,
        core::sync::EndCaller<render::mesh::Mesh>& c) noexcept final;
};
}
#endif
#endif