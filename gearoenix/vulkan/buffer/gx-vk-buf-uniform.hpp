#ifndef GEAROENIX_VULKAN_BUFFER_UNIFORM_HPP
#define GEAROENIX_VULKAN_BUFFER_UNIFORM_HPP
#include "../../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../../core/macro/gx-cr-mcr-getter-setter.hpp"
#include <memory>
#include <vector>

namespace gearoenix::vulkan::engine {
struct Engine;
}

namespace gearoenix::vulkan::buffer {
struct Buffer;
struct Uniform final {
    GX_GET_CRRF_PRV(engine::Engine, e);
    GX_GET_REFC_PRV(std::vector<std::shared_ptr<Buffer>>, cpu);
    GX_GET_REFC_PRV(std::shared_ptr<Buffer>, gpu);

public:
    Uniform(const engine::Engine& e, std::vector<std::shared_ptr<Buffer>> cpu, std::shared_ptr<Buffer> gpu) noexcept;
    ~Uniform() noexcept;
    void update(const void*) noexcept;
};
}
#endif
#endif
