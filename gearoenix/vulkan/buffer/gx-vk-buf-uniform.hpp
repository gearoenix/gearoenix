#pragma once
#include "../../render/gx-rnd-build-configuration.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include <memory>
#include <vector>

namespace gearoenix::vulkan::buffer {
struct Buffer;
struct Uniform final {
private:
    std::vector<std::shared_ptr<Buffer>> cpu;
    std::shared_ptr<Buffer> gpu;

public:
    [[nodiscard]] const std::vector<std::shared_ptr<Buffer>>& get_cpu() const { return cpu; }
    [[nodiscard]] const std::shared_ptr<Buffer>& get_gpu() const { return gpu; }

    Uniform(std::vector<std::shared_ptr<Buffer>>&& cpu, std::shared_ptr<Buffer>&& gpu);
    ~Uniform();
    void update(const void*);
};
}
#endif