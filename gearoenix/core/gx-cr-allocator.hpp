#ifndef GEAROENIX_CORE_ALLOCATOR_HPP
#define GEAROENIX_CORE_ALLOCATOR_HPP
#include "../platform/macro/gx-plt-mcr-lock.hpp"
#include "macro/gx-cr-mcr-getter-setter.hpp"
#include <map>
#include <memory>
#include <optional>
#include <set>

namespace gearoenix::core {
/// Allocator struct for memory management mechanisms
struct Allocator final {
    GX_GET_CVAL_PRV(std::size_t, size)
    /// It is the offset from the origin parent
    GX_GET_VAL_PRV(std::size_t, offset, 0)
private:
    /// size, offset (within itself)
    typedef std::pair<std::size_t, std::size_t> SizeOffset;
    /// allocator that is before free space, allocator that is after space
    typedef std::pair<Allocator*, Allocator*> Range;
    GX_CREATE_GUARD(this);
    std::map<SizeOffset, Range> ranges;
    std::weak_ptr<Allocator> self;
    const std::shared_ptr<Allocator> parent;
    std::optional<SizeOffset> previous_key = std::nullopt;
    std::optional<SizeOffset> next_key = std::nullopt;
    Allocator* previous = nullptr;
    Allocator* next = nullptr;

    Allocator(std::size_t, std::size_t, std::shared_ptr<Allocator> parent = nullptr) noexcept;
    void deallocate(const Allocator* child) noexcept;

public:
    [[nodiscard]] static std::shared_ptr<Allocator> construct(std::size_t size) noexcept;
    Allocator(Allocator&&) = delete;
    Allocator(const Allocator&) = delete;
    Allocator& operator=(Allocator&&) = delete;
    Allocator& operator=(const Allocator&) = delete;
    ~Allocator() noexcept;
    [[nodiscard]] std::shared_ptr<Allocator> allocate(std::size_t size) noexcept;
};
}
#endif
