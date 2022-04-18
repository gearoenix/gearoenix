#include "gx-rnd-sky-manager.hpp"
#include "../../platform/gx-plt-log.hpp"
#include "../../platform/stream/gx-plt-stm-path.hpp"
#include "../engine/gx-rnd-eng-engine.hpp"
#include "../texture/gx-rnd-txt-manager.hpp"
#include "../texture/gx-rnd-txt-texture-2d.hpp"

gearoenix::render::skybox::Manager::Manager(engine::Engine& e) noexcept
    : e(e)
{
}

gearoenix::render::skybox::Manager::~Manager() noexcept = default;

std::shared_ptr<gearoenix::render::skybox::Builder> gearoenix::render::skybox::Manager::build(
    std::string&& name, platform::stream::Path&& texture_path, const core::sync::EndCallerIgnored& c) noexcept
{
    if (texture_path.get_raw_data().ends_with(".hdr")) {
        auto txt = e.get_texture_manager()->create_2d_from_file(
            name + "-texture",
            texture_path,
            gearoenix::render::texture::TextureInfo {
                .sampler_info = gearoenix::render::texture::SamplerInfo {
                    .min_filter = gearoenix::render::texture::Filter::Linear,
                    .mag_filter = gearoenix::render::texture::Filter::Linear,
                    .wrap_s = gearoenix::render::texture::Wrap::ClampToEdge,
                    .wrap_t = gearoenix::render::texture::Wrap::ClampToEdge,
                    .wrap_r = gearoenix::render::texture::Wrap::ClampToEdge,
                },
                .has_mipmap = false,
            },
            c);
        return build(std::move(name), std::move(txt), c);
    } else if (texture_path.get_raw_data().ends_with(".gx-sky")) {
        GX_UNIMPLEMENTED;
    }
    GX_UNEXPECTED;
}
