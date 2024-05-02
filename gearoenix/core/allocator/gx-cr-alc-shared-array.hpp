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

public:
    struct StdDeleter final {
        SharedArray* const allocator;

        explicit StdDeleter(SharedArray* const allocator)
            : allocator(allocator)
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

    SharedArray()
    {
#if GX_DEBUG_MODE
        for (auto& b : gx_core_shared_array_objects)
            b = 0;
#endif
        for (std::size_t i = S * gx_core_shared_array_element_size; i > 0;) {
            i -= gx_core_shared_array_element_size;
            gx_core_shared_array_free_pointers.push_back(reinterpret_cast<T*>(&gx_core_shared_array_objects[i]));
        }
    }

    template <typename... Args>
    [[nodiscard]] std::shared_ptr<T> make_shared(Args&&... args)
    {
        auto* const ptr = [this]() {
            const std::lock_guard<std::mutex> _lg(gx_core_shared_array_lock);

            if (gx_core_shared_array_free_pointers.empty()) {
                GX_LOG_F("Allocation failed, out of memory for type: " << typeid(T).name());
            }
            auto* const ptr = gx_core_shared_array_free_pointers.back();
            gx_core_shared_array_free_pointers.pop_back();
            return ptr;
        }();
#if GX_DEBUG_MODE
        auto* const bs = reinterpret_cast<std::uint8_t*>(ptr);
        for (std::size_t i = 0; i < gx_core_shared_array_element_size; ++i) {
            GX_ASSERT(bs[i] == 0);
        }
#endif
        return std::shared_ptr<T>(new (ptr) T(std::forward<Args>(args)...), StdDeleter(this));
    }
};
}

#endif