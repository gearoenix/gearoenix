#include "gx-vk-txt-target.hpp"
#ifdef GX_USE_VULKAN
#include "../../core/asset/gx-cr-asset-manager.hpp"
#include "../engine/gx-vk-eng-engine.hpp"
#include "gx-vk-txt-2d.hpp"
#include "gx-vk-txt-cube.hpp"

// MSVC does not support trivial copy constructor
gearoenix::vulkan::texture::Target::Target(const Target& o) noexcept
    : render::texture::Target(o)
    , render_pass(o.render_pass)
    , start_semaphore(o.start_semaphore)
    , end_semaphore(o.end_semaphore)
    , framebuffer(o.framebuffer)
{
}

gearoenix::vulkan::texture::Target::Target(
    const core::Id id,
    const std::vector<render::texture::AttachmentInfo>& infos,
    engine::Engine* const e,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& call) noexcept
    : render::texture::Target(id, e)
{
    GX_CHECK_NOT_EQUAL_D(infos.size(), 0)
    clipping_width = infos[0].img_width;
    clipping_height = infos[0].img_height;
    attachments.reserve(infos.size());
    for (const auto& info : infos) {
        decltype(std::declval<render::texture::Attachment>().txt) txt;
        decltype(std::declval<render::texture::Attachment>().var) var;

        if (info.txt.has_value()) {
            txt = info.txt.value();
            switch (info.texture_info.texture_type) {
            case render::texture::Type::Texture2D: {
                var = render::texture::Attachment2D {
                    .txt = std::dynamic_pointer_cast<Texture2D>(txt),
                };
                break;
            }
            case render::texture::Type::TextureCube: {
                GX_CHECK_EQUAL_D(info.img_width, info.img_height)
                var = render::texture::AttachmentCube {
                    .txt = std::dynamic_pointer_cast<TextureCube>(txt),
                    .face = info.face.value(),
                };
                break;
            }
            default:
                GX_UNIMPLEMENTED
            }
        } else {
            const auto txt_id = core::asset::Manager::create_id();
            switch (info.texture_info.texture_type) {
            case render::texture::Type::Texture2D: {
                auto t = std::make_shared<Texture2D>(
                    txt_id, std::string("target-") + std::to_string(target_id) + "-2d-" + std::to_string(txt_id),
                    e, std::vector<std::vector<std::uint8_t>> {}, info.texture_info,
                    static_cast<std::size_t>(info.img_width), static_cast<std::size_t>(info.img_height), call);
                txt = t;
                var = render::texture::Attachment2D {
                    .txt = std::move(t),
                };
                break;
            }
            case render::texture::Type::TextureCube: {
                GX_CHECK_EQUAL_D(info.img_width, info.img_height)
                auto t = std::make_shared<TextureCube>(
                    txt_id, std::string("target-") + std::to_string(target_id) + "-cube-" + std::to_string(txt_id),
                    e, std::vector<std::vector<std::vector<std::uint8_t>>> {},
                    info.texture_info, static_cast<std::size_t>(info.img_width), call);
                txt = t;
                var = render::texture::AttachmentCube {
                    .txt = std::move(t),
                    .face = info.face.value(),
                };
                break;
            }
            default:
                GX_UNIMPLEMENTED
            }
        }

        attachments.push_back(render::texture::Attachment {
            .txt = std::move(txt),
            .img_width = info.img_width,
            .img_height = info.img_height,
            .img_depth = 0,
            .mipmap_level = info.mipmap_level,
            .usage = info.usage,
            .var = std::move(var),
        });
    }
}

gearoenix::vulkan::texture::Target::~Target() noexcept = default;

gearoenix::vulkan::texture::Target* gearoenix::vulkan::texture::Target::clone() const noexcept
{
    return new Target(*this);
}

#endif
