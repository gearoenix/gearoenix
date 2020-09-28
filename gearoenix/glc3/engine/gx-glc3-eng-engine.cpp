#include "gx-glc3-eng-engine.hpp"
#ifdef GX_USE_OPENGL_CLASS_3
#include "../../core/asset/gx-cr-asset-manager.hpp"
#include "../../core/event/gx-cr-ev-event.hpp"
#include "../../gl/gx-gl-loader.hpp"
#include "../../physics/gx-phs-engine.hpp"
#include "../../render/camera/gx-rnd-cmr-camera.hpp"
#include "../../render/pipeline/gx-rnd-pip-manager.hpp"
#include "../../render/scene/gx-rnd-scn-scene.hpp"
#include "../../system/gx-sys-application.hpp"
#include "../buffer/gx-glc3-buf-manager.hpp"
#include "../buffer/gx-glc3-buf-uniform.hpp"
#include "../command/gx-glc3-cmd-buffer.hpp"
#include "../command/gx-glc3-cmd-manager.hpp"
#include "../pipeline/gx-glc3-pip-manager.hpp"
#include "../sync/gx-glc3-sy-semaphore.hpp"
#include "../texture/gx-glc3-txt-2d.hpp"
#include "../texture/gx-glc3-txt-cube.hpp"
#include "../texture/gx-glc3-txt-target.hpp"

gearoenix::glc3::engine::Engine::Engine(system::Application* const sys_app, const render::engine::Type engine_type) noexcept
    : render::engine::Engine(sys_app, engine_type)
{
    gl::Loader::fill_limitations(limitations);
}

gearoenix::glc3::engine::Engine* gearoenix::glc3::engine::Engine::construct(system::Application* const sys_app, const render::engine::Type engine_type) noexcept
{
    auto e = new Engine(sys_app, engine_type);
    e->pipeline_manager = std::make_unique<pipeline::Manager>(e);
    e->buffer_manager = std::make_unique<buffer::Manager>(e);
    e->command_manager = std::make_unique<command::Manager>();
    e->main_render_target = std::make_unique<texture::Target>(e);
    return e;
}

gearoenix::glc3::engine::Engine::~Engine() noexcept
{
    terminate();
}

void gearoenix::glc3::engine::Engine::update() noexcept
{
    bound_target = nullptr;
    bound_shader = gl::uint(-1);

#ifdef GX_DEBUG_GL_CLASS_3
    gl::Loader::check_for_error();
#endif
    render::engine::Engine::update();
#ifdef GX_DEBUG_GL_CLASS_3
    gl::Loader::check_for_error();
#endif
}

void gearoenix::glc3::engine::Engine::terminate() noexcept
{
    render::engine::Engine::terminate();
}

std::shared_ptr<gearoenix::render::sync::Semaphore> gearoenix::glc3::engine::Engine::create_semaphore() const noexcept
{
    return std::shared_ptr<render::sync::Semaphore>(new sync::Semaphore());
}

std::shared_ptr<gearoenix::render::texture::Texture2D> gearoenix::glc3::engine::Engine::create_texture_2d(
    const core::Id id,
    std::string name,
    std::vector<std::vector<std::uint8_t>> data,
    const render::texture::TextureInfo& info,
    const std::size_t img_width,
    const std::size_t img_height,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& call) noexcept
{
    return texture::Texture2D::construct(id, std::move(name), this, std::move(data), info, img_width, img_height, call);
}

std::shared_ptr<gearoenix::render::texture::TextureCube> gearoenix::glc3::engine::Engine::create_texture_cube(
    const core::Id id,
    std::string name,
    std::vector<std::vector<std::vector<std::uint8_t>>> data,
    const render::texture::TextureInfo& info,
    const std::size_t aspect,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& call) noexcept
{
    return texture::TextureCube::construct(id, std::move(name), this, std::move(data), info, aspect, call);
}

std::shared_ptr<gearoenix::render::texture::Target> gearoenix::glc3::engine::Engine::create_render_target(
    const core::Id id,
    const std::vector<render::texture::AttachmentInfo>& infos,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& call) noexcept
{
    return texture::Target::construct(id, this, infos, call);
}

void gearoenix::glc3::engine::Engine::submit(
    const std::size_t,
    const render::sync::Semaphore* const* const,
    const std::size_t cmds_count,
    const render::command::Buffer* const* const cmds,
    const std::size_t,
    const render::sync::Semaphore* const* const) noexcept
{
#ifdef GX_DEBUG_GL_CLASS_3
    gl::Loader::check_for_error();
#endif
    for (std::size_t i = 0; i < cmds_count; ++i) {
        const auto res = static_cast<const command::Buffer*>(cmds[i])->play(bound_target, bound_shader);
        bound_shader = res.first;
        bound_target = res.second;
    }
#ifdef GX_DEBUG_GL_CLASS_3
    gl::Loader::check_for_error();
#endif
}

#endif
