#pragma once
#include "../macro/gx-cr-mcr-getter-setter.hpp"
#include <map>
#include <memory>
#include <mutex>
#include <optional>

namespace gearoenix::core::allocator {
/// Allocator struct for memory management mechanisms
struct Range final {
    GX_GET_CVAL_PRV(std::int64_t, size);
    /// It is the offset from the origin parent
    GX_GET_VAL_PRV(std::int64_t, offset, 0);

private:
    /// size, offset (within itself)
    typedef std::pair<std::int64_t, std::int64_t> SizeOffset;
    /// allocator that is before free space, allocator that is after space
    typedef std::pair<Range*, Range*> PtrRange;

    std::mutex this_lock;
    std::map<SizeOffset, PtrRange> ranges;
    std::weak_ptr<Range> self;
    const std::shared_ptr<Range> parent;
    std::optional<SizeOffset> previous_key = std::nullopt;
    std::optional<SizeOffset> next_key = std::nullopt;
    Range* previous = nullptr;
    Range* next = nullptr;

    Range(std::int64_t, std::int64_t, std::shared_ptr<Range> parent = nullptr);
    void deallocate(const Range* child);

public:
    [[nodiscard]] static std::shared_ptr<Range> construct(std::int64_t size);
    Range(Range&&) = delete;
    Range(const Range&) = delete;
    Range& operator=(Range&&) = delete;
    Range& operator=(const Range&) = delete;
    ~Range();
    [[nodiscard]] std::shared_ptr<Range> allocate(std::int64_t size);
};
}