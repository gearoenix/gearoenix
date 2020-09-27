#include "gx-cr-allocator.hpp"
#include "../system/gx-sys-log.hpp"
#include <algorithm>

gearoenix::core::Allocator::Allocator(
    const std::size_t size,
    const std::size_t offset) noexcept
    : size(size)
    , offset(offset)
    , ranges { { { size, 0 }, { nullptr, nullptr } } }
{
}

std::shared_ptr<gearoenix::core::Allocator> gearoenix::core::Allocator::construct(
    const std::size_t size, const std::size_t offset) noexcept
{
    std::shared_ptr<Allocator> r(new Allocator(size, offset));
    r->self = r;
    return r;
}

gearoenix::core::Allocator::~Allocator() noexcept = default;

std::shared_ptr<gearoenix::core::Allocator> gearoenix::core::Allocator::allocate(
    const std::size_t sz) noexcept
{
    GX_GUARD_LOCK(whole)
    auto search = std::upper_bound(
        ranges.begin(), ranges.end(), sz,
        [](const std::size_t a, const decltype(ranges)::value_type& b) {
            return a <= b.first.first;
        });
    if (ranges.end() == search) {
        GXLOGD("No space left to allocate: " << sz)
        return nullptr;
    }
    const auto found_size = search->first.first;
    const auto found_offset = search->first.second;
    auto result = construct(sz, found_offset);
    result->parent = self.lock();
    const Range del_range = search->second;
    ranges.erase(search);
    auto* const result_previous = del_range.first;
    if (nullptr != result_previous) {
        result_previous->next_key = std::nullopt;
        result_previous->next = result.get();
        result->previous = result_previous;
    }
    auto* const result_next = del_range.second;
    if (sz != found_size) {
        const auto new_key = std::make_pair(found_size - sz, found_offset + sz);
        if (nullptr != result_next)
            result_next->previous_key = new_key;
        result->next_key = new_key;
        ranges.emplace(new_key, std::make_pair(result.get(), result_next));
    }
    if (nullptr != result_next) {
        result_next->previous = result.get();
        result->next = result_next;
    }
    return result;
}
