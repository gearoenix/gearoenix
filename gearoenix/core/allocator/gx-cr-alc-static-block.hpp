#ifndef GEAROENIX_CORE_ALLOCATOR_RANGE_HPP
#define GEAROENIX_CORE_ALLOCATOR_RANGE_HPP
#include <boost/container/static_vector.hpp>
#include <map>
#include <memory>
#include <mutex>
#include <utility>

namespace gearoenix::core::allocator {
template <std::size_t Size>
struct StaticBlock final {
    template <typename T>
    struct std_deleter {
        StaticBlock<Size>& ref;

        void operator()(T const* p) noexcept
        {
            ref.template free(p);
        }
    };

private:
    boost::container::static_vector<std::size_t, (Size + sizeof(std::size_t) - 1) / sizeof(std::size_t)> memory;
    std::mutex released_offsets_lock;
    std::multimap<std::size_t /*size*/, void* /*ptr*/> released_offsets;

public:
    StaticBlock() noexcept = default;

    template <typename T, typename... Args>
    [[nodiscard]] T* alloc(Args&&... args) noexcept
    {
        constexpr auto sz = (sizeof(T) + sizeof(std::size_t) - 1) / sizeof(std::size_t);
        std::lock_guard<std::mutex> _lg(released_offsets_lock);
        if (auto search = released_offsets.find(sizeof(T)); released_offsets.end() != search) {
            auto* const t = new (search->second) T(std::forward<Args>(args)...);
            released_offsets.erase(search);
            return t;
        }
        memory.push_back(sz);
        const auto ptr = memory.size();
        for (std::size_t i = 0; i < sz; ++i)
            memory.push_back(0);
        return new (static_cast<void*>(&memory[ptr])) T(std::forward<Args>(args)...);
    }

    template <typename T, typename... Args>
    [[nodiscard]] auto make_shared(Args&&... args) noexcept
    {
        return std::shared_ptr<T>(alloc<T>(std::forward<Args>(args)...), std_deleter<T> { *this });
    }

    template <typename T>
    void free(T* const t) noexcept
    {
        auto* const v = const_cast<void*>(static_cast<const void*>(t));
        std::lock_guard<std::mutex> _lg(released_offsets_lock);
        const auto sz = *(reinterpret_cast<const std::size_t*>(t) - 1);
        std::memset(v, 0, sz);
        released_offsets.template emplace(sz, v);
    }
};
}

#endif