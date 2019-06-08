#include "rnd-gr-nd-shadow-mapper.hpp"
#include "../../../core/asset/cr-asset-manager.hpp"
#include "../../../core/sync/cr-sync-kernel-workers.hpp"
#include "../../command/rnd-cmd-buffer.hpp"
#include "../../command/rnd-cmd-manager.hpp"
#include "../../engine/rnd-eng-engine.hpp"
#include "../../sync/rnd-sy-semaphore.hpp"
#include "../../texture/rnd-txt-target.hpp"

gearoenix::render::graph::node::ShadowMapperKernel::ShadowMapperKernel(
    engine::Engine* const e,
    const unsigned int kernel_index) noexcept
    : secondary_cmd(e->get_command_manager()->create_secondary_command_buffer(kernel_index))
{
}

gearoenix::render::graph::node::ShadowMapperFrame::ShadowMapperFrame(engine::Engine* const e) noexcept
    : primary_cmd(e->get_command_manager()->create_primary_command_buffer())
    , semaphore(e->create_semaphore())
{
    const unsigned int kernels_count = e->get_kernels()->get_threads_count();
    kernels.resize(kernels_count);
    for (unsigned int i = 0; i < kernels_count; ++i) {
        kernels[i] = std::make_shared<ShadowMapperKernel>(e, i);
    }
}

gearoenix::render::graph::node::ShadowMapper::ShadowMapper(
    engine::Engine* e,
    const core::sync::EndCaller<gearoenix::core::sync::EndCallerIgnore>& call) noexcept
    : Node(
        e,
        pipeline::Type::ShadowMapper,
        0,
        1,
        {},
        { "depth" },
        call)
{

    frames.resize(e->get_frames_count());
    for (unsigned int i = 0; i < e->get_frames_count(); ++i) {
        frames[i] = std::make_shared<ShadowMapperFrame>(e);
    }
    const bool week_hwr = e->get_engine_type_id() == engine::Type::OPENGL_ES2;
    texture::SampleInfo s;
    render_target = e->create_render_target(
        core::asset::Manager::create_id(),
        week_hwr ? texture::TextureFormat::R_FLOAT16 : texture::TextureFormat::R_FLOAT32,
        s,
        week_hwr ? 512 : 1024,
        week_hwr ? 512 : 1024,
        call);
    output_textures[0] = std::shared_ptr<texture::Texture>(static_cast<texture::Texture*>(render_target));
}
