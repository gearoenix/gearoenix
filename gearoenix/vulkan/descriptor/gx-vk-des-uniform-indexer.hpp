#pragma once
#include "../../render/gx-rnd-build-configuration.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../core/gx-cr-singleton.hpp"
#include "../../core/macro/gx-cr-mcr-assert.hpp"
#include "../../core/macro/gx-cr-mcr-getter-setter.hpp"
#include "../buffer/gx-vk-buf-manager.hpp"
#include "../buffer/gx-vk-buf-uniform.hpp"
#include "../shader/glsl/gx-vk-shd-common.glslh"

#include <atomic>
#include <mutex>
#include <type_traits>
#include <vector>

namespace gearoenix::vulkan::descriptor {
enum struct IndexingPolicy {
    DefaultCounter,
    AtomicCounter,
    Allocator,
};

template <typename GlslStruct, IndexingPolicy Policy = IndexingPolicy::DefaultCounter>
struct UniformIndexer : core::Singleton<UniformIndexer<GlslStruct, Policy>> {
    static constexpr bool IsAtomic = Policy == IndexingPolicy::AtomicCounter;
    static constexpr bool IsAllocator = Policy == IndexingPolicy::Allocator;
    static constexpr bool IsDefault = Policy == IndexingPolicy::DefaultCounter;

    using counter_t = std::conditional_t<IsAtomic, std::atomic<std::uint32_t>, std::uint32_t>;
    using allocator_t = std::pair<std::mutex, std::vector<std::uint32_t>>;
    using policy_holder_t = std::conditional_t<IsAllocator, allocator_t, counter_t>;

    struct DataAccess final {
        GX_GET_PTR_PRV(GlslStruct, ptr);
        GX_GET_VAL_PRV(std::uint32_t, index, static_cast<std::uint32_t>(-1));

    public:
        explicit DataAccess(GlslStruct* const ptr, const std::uint32_t index)
            : ptr(ptr)
            , index(index)
        {
        }

        DataAccess(DataAccess&& o) noexcept
            : ptr(o.ptr)
            , index(o.index)
        {
            o.ptr = nullptr;
            o.index = static_cast<std::uint32_t>(-1);
        }

        ~DataAccess()
        {
            if constexpr (IsAllocator) {
                if (ptr != nullptr) {
                    UniformIndexer::get().free(index);
                }
            }
        }
    };

private:
    std::vector<GlslStruct> shader_datas;
    const std::shared_ptr<buffer::Uniform> uniform_buffer;
    policy_holder_t policy_holder;

public:
    explicit UniformIndexer(const std::uint32_t count)
        : core::Singleton<UniformIndexer>(this)
        , shader_datas(count)
        , uniform_buffer(buffer::Manager::get().create_uniform(count * sizeof(GlslStruct)))
    {
        if constexpr (IsAtomic) {
            policy_holder.store(0, std::memory_order_relaxed);
        } else if constexpr (IsDefault) {
            policy_holder = 0;
        } else if constexpr (IsAllocator) {
            policy_holder.second.reserve(count);
            for (auto i = count; i > 0;) {
                policy_holder.second.push_back(--i);
            }
        }
        static_assert(IsAllocator || IsAtomic || IsDefault);
    }

    ~UniformIndexer() override = default;

    [[nodiscard]] DataAccess get_next()
    {
        const auto i = [&] {
            if constexpr (IsAtomic) {
                return policy_holder.fetch_add(1, std::memory_order_relaxed);
            } else if constexpr (IsAllocator) {
                const std::lock_guard _(policy_holder.first);
                GX_ASSERT(!policy_holder.second.empty());
                const std::uint32_t idx = policy_holder.second.back();
                policy_holder.second.pop_back();
                return idx;
            } else if constexpr (IsDefault) {
                return policy_holder++;
            }
            static_assert(IsAllocator || IsAtomic || IsDefault);
        }();
        GX_ASSERT_D(i < shader_datas.size());
        return DataAccess(&shader_datas[i], i);
    }

    void reset()
        requires(!IsAllocator)
    {
        if constexpr (IsAtomic) {
            policy_holder.store(0, std::memory_order_relaxed);
        } else if constexpr (IsDefault) {
            policy_holder = 0;
        }
        static_assert(IsAtomic || IsDefault);
    }

    void free(const std::uint32_t index)
        requires IsAllocator
    {
        static_assert(IsAllocator);
        const std::lock_guard _(policy_holder.first);
        policy_holder.second.push_back(index);
    }

    void update()
    {
        uniform_buffer->update(shader_datas.data());
    }

    [[nodiscard]] std::uint32_t get_current_index() const
        requires(!IsAllocator)
    {
        if constexpr (IsAtomic) {
            return policy_holder.load(std::memory_order_relaxed);
        } else if constexpr (IsDefault) {
            return policy_holder;
        }
        static_assert(IsAtomic || IsDefault);
        return 0;
    }

    [[nodiscard]] buffer::Buffer& get_gpu_buffer() const
    {
        return *uniform_buffer->get_gpu();
    }
};
}

#endif