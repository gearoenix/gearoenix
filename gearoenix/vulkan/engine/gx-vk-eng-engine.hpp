#ifndef GEAROENIX_VULKAN_ENGINE_HPP
#define GEAROENIX_VULKAN_ENGINE_HPP
#include "../../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../../render/engine/gx-rnd-eng-engine.hpp"
#include <functional>
#include <mutex>

namespace gearoenix::vulkan::buffer {
struct Manager;
}

namespace gearoenix::vulkan::command {
struct Buffer;
}

namespace gearoenix::vulkan::device {
struct Physical;
struct Logical;
}

namespace gearoenix::vulkan::image {
struct View;
struct Manager;
}

namespace gearoenix::vulkan::memory {
struct Manager;
}

namespace gearoenix::vulkan::sync {
struct Semaphore;
}

namespace gearoenix::vulkan::sampler {
struct Manager;
}

namespace gearoenix::vulkan::texture {
struct MainTarget;
}

namespace gearoenix::vulkan {
struct Instance;
struct Surface;
struct Swapchain;
}

namespace gearoenix::vulkan::engine {
struct Engine final : public render::engine::Engine {
    GX_GET_CREF_PRV(std::shared_ptr<Instance>, instance)
    GX_GET_CREF_PRV(std::shared_ptr<Surface>, surface)
    GX_GET_CREF_PRV(std::shared_ptr<device::Physical>, physical_device)
    GX_GET_CREF_PRV(std::shared_ptr<device::Logical>, logical_device)
    //    GX_GET_CREF_PRV(std::shared_ptr<memory::Manager>, memory_manager)
    //    GX_GET_CREF_PRV(std::shared_ptr<sampler::Manager>, sampler_manager)
    //    GX_GET_CREF_PRV(std::shared_ptr<image::Manager>, image_manager)
    //    GX_GET_CREF_PRV(std::shared_ptr<buffer::Manager>, vulkan_buffer_manager)
    //    GX_GET_CREF_PRV(std::shared_ptr<texture::MainTarget>, vulkan_main_render_target)
    //    GX_GET_CREF_PRV(std::vector<std::shared_ptr<command::Buffer>>, upload_command_buffers)
    //    GX_GET_CREF_PRV(std::vector<std::shared_ptr<sync::Semaphore>>, upload_semaphore)
private:
    Engine(
        const render::RuntimeConfiguration& configuration,
        std::shared_ptr<platform::Application> platform_application) noexcept;

public:
    [[nodiscard]] static std::shared_ptr<Engine> construct(
        const render::RuntimeConfiguration& configuration,
        std::shared_ptr<platform::Application> platform_application) noexcept;
    ~Engine() noexcept final;
    void update() noexcept final;
    //    [[nodiscard]] std::shared_ptr<render::sync::Semaphore> create_semaphore() const noexcept final;
    //    [[nodiscard]] std::shared_ptr<render::texture::Texture2D> create_texture_2d(
    //        core::Id id,
    //        std::string name,
    //        std::vector<std::vector<std::uint8_t>> data,
    //        const render::texture::TextureInfo& info,
    //        std::size_t img_width,
    //        std::size_t img_height,
    //        const core::sync::EndCaller<core::sync::EndCallerIgnore>& call) noexcept final;
    //    [[nodiscard]] std::shared_ptr<render::texture::TextureCube> create_texture_cube(
    //        core::Id id,
    //        std::string name,
    //        std::vector<std::vector<std::vector<std::uint8_t>>> data,
    //        const render::texture::TextureInfo& info,
    //        std::size_t aspect,
    //        const core::sync::EndCaller<core::sync::EndCallerIgnore>& call) noexcept final;
    //    [[nodiscard]] std::shared_ptr<render::texture::Target> create_render_target(
    //        core::Id id,
    //        const std::vector<render::texture::AttachmentInfo>& infos,
    //        const core::sync::EndCaller<core::sync::EndCallerIgnore>& call) noexcept final;
    //    void submit(
    //        std::size_t pres_count,
    //        const render::sync::Semaphore* const* pres,
    //        std::size_t cmds_count,
    //        const render::command::Buffer* const* cmds,
    //        std::size_t nxts_count,
    //        const render::sync::Semaphore* const* nxts) noexcept final;
    [[nodiscard]] static bool is_supported() noexcept;
};
}
#endif
#endif
