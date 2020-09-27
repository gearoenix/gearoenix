#ifndef GEAROENIX_CORE_ALLOCATOR_HPP
#define GEAROENIX_CORE_ALLOCATOR_HPP
#include "gx-cr-static.hpp"
#include <map>
#include <memory>
#include <optional>

namespace gearoenix::core {
struct Allocator final {
    typedef std::pair<std::size_t, std::size_t> SizeOffset;
    typedef std::pair<Allocator*, Allocator*> Range;

    GX_GET_CVAL_PRV(std::size_t, size)
    GX_GET_CVAL_PRV(std::size_t, offset)
private:
    GX_CREATE_GUARD(whole)
    std::map<SizeOffset, Range> ranges;
    std::weak_ptr<Allocator> self;
    std::shared_ptr<Allocator> parent;
    std::optional<SizeOffset> previous_key = std::nullopt;
    std::optional<SizeOffset> next_key = std::nullopt;
    Allocator* previous = nullptr;
    Allocator* next = nullptr;

    Allocator(std::size_t size, std::size_t offset) noexcept;
    void deallocat(const Allocator* o) noexcept;

public:
    [[nodiscard]] static std::shared_ptr<Allocator> construct(std::size_t size, std::size_t offset = 0) noexcept;
    ~Allocator() noexcept;
    [[nodiscard]] std::shared_ptr<Allocator> allocate(std::size_t size) noexcept;
};
}
#endif
