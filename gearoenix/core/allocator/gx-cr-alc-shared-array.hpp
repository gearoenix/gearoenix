#pragma once
#include "../../math/gx-math-numeric.hpp"
#include "../gx-cr-build-configuration.hpp"
#include "../macro/gx-cr-mcr-assert.hpp"

#include <array>
#include <cstring>
#include <memory>
#include <mutex>

#include <boost/container/static_vector.hpp>

namespace gearoenix::core::allocator {
template <typename T, std::size_t S>
struct SharedArray final {
private:
    constexpr static std::size_t element_size = math::Numeric::align<std::size_t>(sizeof(T), alignof(T));

    alignas(alignof(T)) std::array<std::uint8_t, element_size * S> heap;
    boost::container::static_vector<T*, S> free_pointers;
    std::weak_ptr<SharedArray> weak_self;
    std::mutex lock;

    SharedArray()
    {
        if constexpr (GX_DEBUG_MODE) {
            std::memset(heap.data(), 0, heap.size());
        }
        for (std::size_t i = S * element_size; i > 0;) {
            i -= element_size;
            free_pointers.push_back(reinterpret_cast<T*>(&heap[i]));
        }
    }

public:
    ~SharedArray()
    {
        GX_ASSERT_D(free_pointers.size() == S);
        GX_ASSERT_D(weak_self.expired());
        if constexpr (GX_DEBUG_MODE) {
            for (const auto b : heap) {
                GX_ASSERT(0 == b);
            }
        }
    }

    struct StdDeleter final {
        const std::shared_ptr<SharedArray> allocator;

        explicit StdDeleter(std::shared_ptr<SharedArray>&& allocator)
            : allocator(std::move(allocator))
        {
        }

        void operator()(T* const o)
        {
            o->~T();
            if constexpr (GX_DEBUG_MODE) {
                std::memset(reinterpret_cast<std::uint8_t*>(o), 0, element_size);
            }
            const std::lock_guard<std::mutex> _lg(allocator->lock);
            allocator->free_pointers.push_back(o);
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
            const std::lock_guard _lg(lock);

            if (free_pointers.empty()) {
                GX_LOG_F("Allocation failed, out of memory for type: " << typeid(T).name());
            }
            auto* const result = free_pointers.back();
            free_pointers.pop_back();
            return result;
        }();
        if constexpr (GX_DEBUG_MODE) {
            const auto* const bs = reinterpret_cast<std::uint8_t*>(ptr);
            for (std::size_t i = 0; i < element_size; ++i) {
                GX_ASSERT(bs[i] == 0);
            }
        }
        try {
            new (ptr) T(std::forward<Args>(args)...);
        } catch (...) {
            if constexpr (GX_DEBUG_MODE) {
                std::memset(reinterpret_cast<std::uint8_t*>(ptr), 0, element_size);
            }
            const std::lock_guard _lg(lock);
            free_pointers.push_back(ptr);
            throw;
        }
        return std::shared_ptr<T>(ptr, StdDeleter(weak_self.lock()));
    }
};
}