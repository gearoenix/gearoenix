#ifndef GEAROENIX_CORE_ALLOCATOR_HPP
#define GEAROENIX_CORE_ALLOCATOR_HPP
#include "../platform/macro/gx-plt-mcr-lock.hpp"
#include "macro/gx-cr-mcr-getter-setter.hpp"
#include <map>
#include <memory>
#include <optional>

namespace gearoenix::core {
struct Allocator final {
    /// size, offset (within itself)
    typedef std::pair<std::size_t, std::size_t> SizeOffset;
    /// allocator that is before free space, allocator that is after space
    typedef std::pair<Allocator*, Allocator*> Range;

    GX_GET_CVAL_PRV(std::size_t, size)
    /// It is the offset from the direct parent not the origin parent
    GX_GET_CVAL_PRV(std::size_t, offset)
    /// It is the offset from the root
    GX_GET_CVAL_PRV(std::size_t, root_offset)
private:
    GX_CREATE_GUARD(this)
    std::map<SizeOffset, Range> ranges;
    std::weak_ptr<Allocator> self;
    std::shared_ptr<Allocator> parent;
    std::optional<SizeOffset> previous_key = std::nullopt;
    std::optional<SizeOffset> next_key = std::nullopt;
    Allocator* previous = nullptr;
    Allocator* next = nullptr;
    Allocator* first_child = nullptr;

    Allocator(std::size_t size, std::size_t offset, std::size_t root_offset) noexcept;
    void deallocate(const Allocator* child) noexcept;

public:
    [[nodiscard]] static std::shared_ptr<Allocator> construct(std::size_t size, std::size_t offset, std::size_t root_offset) noexcept;
    ~Allocator() noexcept;
    [[nodiscard]] std::shared_ptr<Allocator> allocate(std::size_t size) noexcept;
};
}
#endif
