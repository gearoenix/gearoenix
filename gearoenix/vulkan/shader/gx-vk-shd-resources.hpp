#pragma once
#include "../../render/gx-rnd-build-configuration.hpp"
#if GX_RENDER_VULKAN_ENABLED

#include <memory>
#include <vector>

namespace gearoenix::vulkan::buffer {
struct Uniform;
}

namespace gearoenix::vulkan::descriptor {
struct Set;
}

namespace gearoenix::vulkan::pipeline {
struct Pipeline;
}

namespace gearoenix::vulkan::texture {
struct Texture2D;
}

namespace gearoenix::vulkan::shader {
struct Resources {
private:
    std::vector<std::shared_ptr<descriptor::Set>> descriptors;
    unsigned int descriptors_count;

public:
    Resources(pipeline::Pipeline* pip, buffer::Uniform* u);
    Resources(pipeline::Pipeline* pip, buffer::Uniform* u, texture::Texture2D* t);
    ~Resources();
    void bind(pipeline::Pipeline& pip);
};
}
#endif