#ifndef GEAROENIX_CORE_ALLOCATOR_HPP
#define GEAROENIX_CORE_ALLOCATOR_HPP
#include "../platform/macro/gx-plt-mcr-lock.hpp"
#include "macro/gx-cr-mcr-getter-setter.hpp"
#include <map>
#include <optional>
#include <set>

namespace gearoenix::core {
/// Allocator struct for implementing GC or other memory management mechanisms
///
/// \note Be careful about the movement operations, it may cause the parent pointer be incorrect.
struct Allocator final {
    GX_GET_VAL_PRV(std::size_t, size, 0)
    /// It is the offset from the direct parent not the origin parent
    GX_GET_VAL_PRV(std::size_t, offset, 0)
private:
    GX_CREATE_GUARD(this)
    std::set<std::pair<std::size_t, std::size_t>> size_start;
    std::map<std::size_t, std::size_t> start_to_end;
    std::map<std::size_t, std::size_t> end_to_start;
    Allocator* parent = nullptr;

    void deallocate(std::size_t size, std::size_t offset) noexcept;

public:
    explicit Allocator(std::size_t size, std::size_t offset = 0, Allocator* parent = nullptr) noexcept;
    Allocator(Allocator&&) noexcept;
    Allocator(const Allocator&) = delete;
    ~Allocator() noexcept;
    Allocator& operator=(Allocator&&) noexcept;
    Allocator& operator=(const Allocator&) = delete;
    [[nodiscard]] std::optional<Allocator> allocate(std::size_t size) noexcept;
};
}
#endif
