#pragma once
#include "../../render/gx-rnd-build-configuration.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../core/gx-cr-singleton.hpp"
#include "../../core/macro/gx-cr-mcr-assert.hpp"
#include "../buffer/gx-vk-buf-manager.hpp"
#include "../buffer/gx-vk-buf-uniform.hpp"
#include "../shader/glsl/gx-vk-shd-common.glslh"

#include <vector>

namespace gearoenix::vulkan::descriptor {
/// It is good for per frame indexing
template <typename GlslStruct>
struct UniformIndexer : core::Singleton<UniformIndexer<GlslStruct>> {
    struct DataAccess final {
        GlslStruct* const ptr;
        const std::uint32_t index;
    };

private:
    std::vector<GlslStruct> shader_datas;
    const std::shared_ptr<buffer::Uniform> uniform_buffer;
    std::uint32_t last_index = 0;

public:
    explicit UniformIndexer(const std::uint32_t count)
        : core::Singleton<UniformIndexer>(this)
        , shader_datas(count)
        , uniform_buffer(buffer::Manager::get().create_uniform(count * sizeof(GlslStruct)))
    {
    }

    ~UniformIndexer() override = default;

    [[nodiscard]] DataAccess get_next()
    {
        auto i = last_index;
        ++last_index;
        GX_ASSERT_D(i < shader_datas.size());
        return {&shader_datas[i], i};
    }

    void reset()
    {
        last_index = 0;
    }

    void update()
    {
        uniform_buffer->update(shader_datas.data());
    }
};
}

#endif