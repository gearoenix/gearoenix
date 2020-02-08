#include "glc3-eng-engine.hpp"
#ifdef GX_USE_OPENGL_CLASS_3
#include "../../core/asset/cr-asset-manager.hpp"
#include "../../core/event/cr-ev-event.hpp"
#include "../../gl/gl-constants.hpp"
#include "../../gl/gl-loader.hpp"
#include "../../physics/phs-engine.hpp"
#include "../../render/camera/rnd-cmr-camera.hpp"
#include "../../render/pipeline/rnd-pip-manager.hpp"
#include "../../render/scene/rnd-scn-scene.hpp"
#include "../../system/sys-app.hpp"
#include "../buffer/glc3-buf-manager.hpp"
#include "../buffer/glc3-buf-uniform.hpp"
#include "../command/glc3-cmd-buffer.hpp"
#include "../command/glc3-cmd-manager.hpp"
#include "../pipeline/glc3-pip-manager.hpp"
#include "../sync/glc3-sy-semaphore.hpp"
#include "../texture/glc3-txt-2d.hpp"
#include "../texture/glc3-txt-cube.hpp"
#include "../texture/glc3-txt-target.hpp"

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
    gl::Loader::clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
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
    const void* data,
    const render::texture::TextureFormat f,
    const render::texture::SampleInfo s,
    const unsigned int img_width,
    const unsigned int img_height,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& call) noexcept
{
    return texture::Texture2D::construct(id, this, data, f, s, img_width, img_height, call);
}

std::shared_ptr<gearoenix::render::texture::TextureCube> gearoenix::glc3::engine::Engine::create_texture_cube(
    const core::Id id,
    const void* data,
    const render::texture::TextureFormat f,
    const render::texture::SampleInfo s,
    const unsigned int aspect,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& call) noexcept
{
    return texture::TextureCube::construct(id, this, data, f, s, aspect, call);
}

std::shared_ptr<gearoenix::render::texture::Target> gearoenix::glc3::engine::Engine::create_render_target(
    core::Id id,
    const std::vector<render::texture::Info>& infos,
    unsigned int width,
    unsigned int height,
    const gearoenix::core::sync::EndCaller<gearoenix::core::sync::EndCallerIgnore>& call) noexcept
{
    return texture::Target::construct(id, this, infos, width, height, call);
}

void gearoenix::glc3::engine::Engine::submit(
    const std::size_t,
    const render::sync::Semaphore* const* const,
    const std::size_t cmds_count,
    const render::command::Buffer* const* const cmds,
    const std::size_t,
    const render::sync::Semaphore* const* const) noexcept
{
    for (std::size_t i = 0; i < cmds_count; ++i)
        reinterpret_cast<const command::Buffer*>(cmds[i])->play();
}

#endif
