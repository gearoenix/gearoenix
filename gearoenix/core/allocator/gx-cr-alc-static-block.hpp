#pragma once
#include "../macro/gx-cr-mcr-assert.hpp"
#include <array>
#include <cstring>
#include <memory>
#include <mutex>
#include <set>
#include <utility>

namespace gearoenix::core::allocator {
template <std::intptr_t Size>
struct StaticBlock final {
    template <typename T>
    struct std_deleter {
        StaticBlock& ref;

        void operator()(T const* p)
        {
            ref.free(p);
        }
    };

private:
    std::mutex this_lock;
    std::array<std::uint8_t, Size> memory;
    std::set<std::pair<std::intptr_t /*start-offset*/, std::intptr_t /*size*/>> start_map { { 0, Size } };
    std::set<std::pair<std::intptr_t /*size*/, std::intptr_t /*start-offset*/>> size_map { { Size, 0 } };
    std::set<std::pair<std::intptr_t /*end-offset*/, std::intptr_t /*size*/>> end_map { { Size, Size } };

public:
#ifdef GX_DEBUG_MODE
    StaticBlock()
    {
        std::memset(memory.data(), 0, memory.size());
    }

    ~StaticBlock()
    {
        for (auto c : memory) {
            GX_ASSERT(c == 0);
        }
    }
#endif

    template <typename T, typename... Args>
    [[nodiscard]] T* alloc(Args&&... args)
    {
        std::lock_guard _lg(this_lock);
        auto search = size_map.lower_bound({ sizeof(T), 0 });
        GX_ASSERT_D(size_map.end() != search);
        const auto old_size = search->first;
        const auto old_start = search->second;
        const auto old_end = old_size + old_start;
        end_map.erase({ old_start, old_size });
        size_map.erase(search);
        end_map.erase({ old_end, old_size });
        auto* const result = new (&memory[old_start]) T(std::forward<Args>(args)...);
        GX_ASSERT_D(sizeof(T) <= old_size);
        if (sizeof(T) == old_size) {
            return result;
        }
        const auto new_size = old_size - sizeof(T);
        const auto new_start = old_start + sizeof(T);
        start_map.emplace(new_start, new_size);
        size_map.emplace(new_size, new_start);
        end_map.emplace(old_end, new_size);
        return result;
    }

    template <typename T, typename... Args>
    [[nodiscard]] auto make_shared(Args&&... args)
    {
        return std::shared_ptr<T>(alloc<T>(std::forward<Args>(args)...), std_deleter<T> { *this });
    }

    template <typename T>
    void free(T* const t)
    {
        t->~T();
        auto start = reinterpret_cast<std::intptr_t>(t) - reinterpret_cast<std::intptr_t>(memory.data());
        auto end = static_cast<std::intptr_t>(start + sizeof(T));
        std::memset(&memory[start], 0, sizeof(T));
        std::lock_guard _lg(this_lock);
        if (const auto search = end_map.lower_bound({ start, 0 }); end_map.end() != search && search->first == start) {
            start -= search->second;
            start_map.erase({ start, search->second });
            size_map.erase({ search->second, start });
            end_map.erase(search);
        }
        if (const auto search = start_map.lower_bound({ end, 0 }); start_map.end() != search && search->first == end) {
            end += search->second;
            size_map.erase({ search->second, search->first });
            end_map.erase({ end, search->second });
            start_map.erase(search);
        }
        const auto size = end - start;
        start_map.emplace(start, size);
        size_map.emplace(size, start);
        end_map.emplace(end, size);
    }
};
}