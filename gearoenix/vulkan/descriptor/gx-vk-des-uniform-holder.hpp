#pragma once
#include "../../render/gx-rnd-build-configuration.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../core/gx-cr-singleton.hpp"
#include "../../core/macro/gx-cr-mcr-assert.hpp"
#include "../buffer/gx-vk-buf-manager.hpp"
#include "../buffer/gx-vk-buf-uniform.hpp"

#include <vector>
#include <mutex>

namespace gearoenix::vulkan::descriptor {
template <typename GlslStruct>
struct UniformHolder : core::Singleton<UniformHolder<GlslStruct>> {
    struct PtrHolder final {
        friend struct UniformHolder;

    private:
        GlslStruct* p;
        std::uint32_t i;

        PtrHolder(GlslStruct* const p, const std::uint32_t i): p(p), i(i) {}

    public:
        PtrHolder(const PtrHolder&) = delete;
        PtrHolder& operator=(const PtrHolder&) = delete;

        PtrHolder(PtrHolder&& o) noexcept
            : p(o.p), i(o.i)
        {
            o.p = nullptr;
        }

        PtrHolder& operator=(PtrHolder&& o) noexcept
        {
            if (this == &o) {
                return *this;
            }
            if (p) {
                core::Singleton<UniformHolder>::get().remove(i);
            }
            p = o.p;
            i = o.i;
            o.p = nullptr;
            return *this;
        }

        ~PtrHolder()
        {
            if (p) {
                core::Singleton<UniformHolder>::get().remove(i);
            }
        }

        [[nodiscard]] GlslStruct* ptr() const { return p; }
        [[nodiscard]] std::uint32_t index() const { return i; }
    };

private:
    std::vector<GlslStruct> shader_datas;
    const std::shared_ptr<buffer::Uniform> uniform_buffer;
    std::mutex free_shader_data_indices_lock;
    std::vector<std::uint32_t> free_shader_data_indices;

public:
    explicit UniformHolder(const std::uint32_t count)
        : core::Singleton<UniformHolder>(this)
        , shader_datas(count)
        , uniform_buffer(buffer::Manager::get().create_uniform(count * sizeof(GlslStruct)))
        , free_shader_data_indices(count)
    {
        for (auto i = count; auto& fi: free_shader_data_indices) {
            --i;
            fi = i;
        }
    }

    ~UniformHolder() override = default;

    void remove(const std::uint32_t i)
    {
        const std::lock_guard _(free_shader_data_indices_lock);
        free_shader_data_indices.push_back(i);
    }

    [[nodiscard]] PtrHolder get_uniform_holder()
    {
        const std::lock_guard _(free_shader_data_indices_lock);
        GX_ASSERT_D(!free_shader_data_indices.empty());
        const auto result = free_shader_data_indices.back();
        free_shader_data_indices.pop_back();
        return PtrHolder(&shader_datas[result], result);
    }

    void update()
    {
        uniform_buffer->update(shader_datas.data());
    }

};
}

#endif