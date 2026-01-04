#pragma once
#include "../../render/gx-rnd-build-configuration.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../core/macro/gx-cr-mcr-getter-setter.hpp"
#include <memory>
#include <vector>

namespace gearoenix::vulkan::buffer {
struct Buffer;
struct Uniform final {
    GX_GET_REFC_PRV(std::vector<std::shared_ptr<Buffer>>, cpu);
    GX_GET_REFC_PRV(std::shared_ptr<Buffer>, gpu);

public:
    Uniform(std::vector<std::shared_ptr<Buffer>> cpu, std::shared_ptr<Buffer> gpu);
    ~Uniform();
    void update(const void*);
};
}
#endif