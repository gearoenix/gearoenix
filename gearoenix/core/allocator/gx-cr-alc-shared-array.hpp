#ifndef GEAROENIX_CORE_ALLOCATOR_SHARED_ARRAY_HPP
#define GEAROENIX_CORE_ALLOCATOR_SHARED_ARRAY_HPP
#include "../../math/gx-math-numeric.hpp"
#include "../macro/gx-cr-mcr-assert.hpp"
#include <array>
#include <boost/container/static_vector.hpp>
#include <cstring>
#include <memory>
#include <mutex>

namespace gearoenix::core::allocator {
template <typename T, std::size_t S>
struct SharedArray final {
private:
    static constexpr std::size_t gx_core_shared_array_align_size = 256;
    static constexpr auto gx_core_shared_array_element_size = math::Numeric::align<std::size_t>(sizeof(T), gx_core_shared_array_align_size);
    static constexpr std::size_t gx_core_shared_array_size = gx_core_shared_array_element_size * S;

    std::mutex gx_core_shared_array_lock;
    std::array<std::uint8_t, gx_core_shared_array_size> gx_core_shared_array_objects;
    boost::container::static_vector<T*, S> gx_core_shared_array_free_pointers;
    std::weak_ptr<SharedArray> weak_self;

    SharedArray()
    {
#if GX_DEBUG_MODE
        std::memset(gx_core_shared_array_objects.data(), 0, gx_core_shared_array_objects.size());
#endif
        for (std::size_t i = S * gx_core_shared_array_element_size; i > 0;) {
            i -= gx_core_shared_array_element_size;
            gx_core_shared_array_free_pointers.push_back(reinterpret_cast<T*>(&gx_core_shared_array_objects[i]));
        }
    }

public:
    struct StdDeleter final {
        const std::shared_ptr<SharedArray> allocator;

        explicit StdDeleter(std::shared_ptr<SharedArray>&& allocator)
            : allocator(std::move(allocator))
        {
        }

        void operator()(T* const o)
        {
            o->~T();
#if GX_DEBUG_MODE
            std::memset(reinterpret_cast<std::uint8_t*>(o), 0, gx_core_shared_array_element_size);
#endif
            const std::lock_guard<std::mutex> _lg(allocator->gx_core_shared_array_lock);
            allocator->gx_core_shared_array_free_pointers.push_back(o);
        }
    };

    [[nodiscard]] static std::shared_ptr<SharedArray> construct()
    {
        std::shared_ptr<SharedArray> result(new SharedArray());
        result->weak_self = result;
        return result;
    }

    template <typename... Args>
    [[nodiscard]] std::shared_ptr<T> make_shared(Args&&... args)
    {
        auto* const ptr = [this] {
            const std::lock_guard _lg(gx_core_shared_array_lock);

            if (gx_core_shared_array_free_pointers.empty()) {
                GX_LOG_F("Allocation failed, out of memory for type: " << typeid(T).name());
            }
            auto* const result = gx_core_shared_array_free_pointers.back();
            gx_core_shared_array_free_pointers.pop_back();
            return result;
        }();
#if GX_DEBUG_MODE
        const auto* const bs = reinterpret_cast<std::uint8_t*>(ptr);
        for (std::size_t i = 0; i < gx_core_shared_array_element_size; ++i) {
            GX_ASSERT(bs[i] == 0);
        }
#endif
        return std::shared_ptr<T>(new (ptr) T(std::forward<Args>(args)...), StdDeleter(weak_self.lock()));
    }
};
}

#endif