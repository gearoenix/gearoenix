#ifndef GEAROENIX_GLC3_ENGINE_HPP
#define GEAROENIX_GLC3_ENGINE_HPP
#include "../../core/cr-build-configuration.hpp"
#ifdef GX_USE_OPENGL_CLASS_3
#include "../../render/engine/rnd-eng-engine.hpp"
#include "../glc3.hpp"
#include <memory>
namespace gearoenix::glc3 {
namespace texture {
    class Texture2D;
}
namespace engine {
    class Engine : public render::engine::Engine {
    private:
        explicit Engine(system::Application* sys_app, render::engine::Type::Id engine_type) noexcept;

    public:
        /// TODO: this is temporary and must change in future
        static std::shared_ptr<Engine> construct(system::Application* sys_app, render::engine::Type::Id engine_type) noexcept;
        ~Engine() noexcept final;
        void update() noexcept final;
        void terminate() noexcept final;
        render::sync::Semaphore* create_semaphore() const noexcept final;
        render::texture::Texture2D* create_texture_2d(
            core::Id id,
            const void* data,
            render::texture::TextureFormat::Id f,
            render::texture::SampleInfo s,
            unsigned int width,
            unsigned int heigt,
            const core::sync::EndCaller<core::sync::EndCallerIgnore>& call) noexcept final;
        render::texture::Cube* create_texture_cube(
            core::Id id,
            const void* data,
            render::texture::TextureFormat::Id f,
            render::texture::SampleInfo s,
            unsigned int aspect,
            const core::sync::EndCaller<core::sync::EndCallerIgnore>& call) noexcept final;
        render::texture::Target* create_render_target(
            core::Id id,
            const std::vector<render::texture::Info>& infos,
            unsigned int width,
            unsigned int height,
            const core::sync::EndCaller<core::sync::EndCallerIgnore>& call) noexcept final;
        void submit(
            std::size_t pres_count,
            const render::sync::Semaphore* const* pres,
            std::size_t cmds_count,
            const render::command::Buffer* const* cmds,
            std::size_t nxts_count,
            const render::sync::Semaphore* const* nxts) noexcept final;
    };
} // namespace engine
} // namespace gearoenix::glc3
#endif
#endif
