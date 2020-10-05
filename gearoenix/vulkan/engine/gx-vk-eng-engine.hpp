#ifndef GEAROENIX_VULKAN_ENGINE_HPP
#define GEAROENIX_VULKAN_ENGINE_HPP
#include "../../core/gx-cr-build-configuration.hpp"
#ifdef GX_USE_VULKAN
#include "../../core/gx-cr-static.hpp"
#include "../../render/engine/gx-rnd-eng-engine.hpp"
#include <functional>
#include <mutex>

namespace gearoenix::system {
class Application;
class File;
}

namespace gearoenix::vulkan::command {
class Buffer;
class Pool;
}

namespace gearoenix::vulkan::device {
class Physical;
class Logical;
}

namespace gearoenix::vulkan::image {
class View;
}

namespace gearoenix::vulkan::memory {
class Manager;
}

namespace gearoenix::vulkan::scene {
class Scene;
}

namespace gearoenix::vulkan::sync {
class Fence;
class Semaphore;
}

namespace gearoenix::vulkan::sampler {
class Manager;
}

namespace gearoenix::vulkan {
class Framebuffer;
class Instance;
class RenderPass;
class Surface;
class Swapchain;
}

namespace gearoenix::vulkan::engine {
class Engine final : public render::engine::Engine {
    GX_GET_CREF_PRV(std::shared_ptr<Instance>, instance)
    GX_GET_CREF_PRV(std::shared_ptr<Surface>, surface)
    GX_GET_CREF_PRV(std::shared_ptr<device::Physical>, physical_device)
    GX_GET_CREF_PRV(std::shared_ptr<device::Logical>, logical_device)
    GX_GET_CREF_PRV(std::shared_ptr<memory::Manager>, memory_manager)
    GX_GET_CREF_PRV(std::shared_ptr<sampler::Manager>, sampler_manager)

    explicit Engine(system::Application* sys_app) noexcept;

public:
    [[nodiscard]] static Engine* construct(system::Application* sys_app) noexcept;
    ~Engine() noexcept final;
    void update() noexcept final;
    void terminate() noexcept final;
    [[nodiscard]] std::shared_ptr<render::sync::Semaphore> create_semaphore() const noexcept final;
    [[nodiscard]] std::shared_ptr<render::texture::Texture2D> create_texture_2d(
        core::Id id,
        std::string name,
        std::vector<std::vector<std::uint8_t>> data,
        const render::texture::TextureInfo& info,
        std::size_t img_width,
        std::size_t img_height,
        const core::sync::EndCaller<core::sync::EndCallerIgnore>& call) noexcept final;
    [[nodiscard]] std::shared_ptr<render::texture::TextureCube> create_texture_cube(
        core::Id id,
        std::string name,
        std::vector<std::vector<std::vector<std::uint8_t>>> data,
        const render::texture::TextureInfo& info,
        std::size_t aspect,
        const core::sync::EndCaller<core::sync::EndCallerIgnore>& call) noexcept final;
    [[nodiscard]] std::shared_ptr<render::texture::Target> create_render_target(
        core::Id id,
        const std::vector<render::texture::AttachmentInfo>& infos,
        const core::sync::EndCaller<core::sync::EndCallerIgnore>& call) noexcept final;
    void submit(
        std::size_t pres_count,
        const render::sync::Semaphore* const* pres,
        std::size_t cmds_count,
        const render::command::Buffer* const* cmds,
        std::size_t nxts_count,
        const render::sync::Semaphore* const* nxts) noexcept final;
};
}
#endif
#endif
