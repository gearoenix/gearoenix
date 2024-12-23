#include "gx-cr-alc-range.hpp"
#include "../macro/gx-cr-mcr-assert.hpp"
#include <algorithm>

gearoenix::core::allocator::Range::Range(
    const std::int64_t size, const std::int64_t offset, std::shared_ptr<Range> parent)
    : size(size)
    , offset(offset)
    , parent(std::move(parent))
    , ranges { { { size, offset }, { nullptr, nullptr } } }
{
}

void gearoenix::core::allocator::Range::deallocate(const Range* const child)
{
    std::lock_guard _lg(this_lock);
    auto* const child_previous = child->previous;
    auto* const child_next = child->next;
    const auto new_range = std::make_pair(child_previous, child_next);
    SizeOffset new_key { child->size, child->offset };
    const auto child_previous_key = child->previous_key;
    const auto child_next_key = child->next_key;
    if (child_previous_key.has_value()) {
        new_key.first += child_previous_key->first;
        new_key.second = child_previous_key->second;
        ranges.erase(*child_previous_key);
    }
    if (child_next_key.has_value()) {
        new_key.first += child_next_key->first;
        ranges.erase(*child_next_key);
    }
    if (nullptr != child_previous) {
        child_previous->next = child_next;
        child_previous->next_key = new_key;
    }
    if (nullptr != child_next) {
        child_next->previous = child_previous;
        child_next->previous_key = new_key;
    }
    ranges.emplace(new_key, new_range);
}

std::shared_ptr<gearoenix::core::allocator::Range> gearoenix::core::allocator::Range::construct(const std::int64_t size)
{
    GX_CHECK_NOT_EQUAL_D(size, 0);
    std::shared_ptr<Range> result(new Range(size, 0));
    result->self = result;
    return result;
}

gearoenix::core::allocator::Range::~Range()
{
    if (nullptr != parent)
        parent->deallocate(this);
}

std::shared_ptr<gearoenix::core::allocator::Range> gearoenix::core::allocator::Range::allocate(const std::int64_t sz)
{
    std::lock_guard<std::mutex> _lg(this_lock);
    auto search = std::upper_bound(
        ranges.begin(), ranges.end(), sz,
        [](const std::int64_t a, const decltype(ranges)::value_type& b) {
            return a <= b.first.first;
        });
    if (ranges.end() == search) {
        GX_LOG_D("Not enough space left to allocate: " << sz);
        return nullptr;
    }
    const auto found_size = search->first.first;
    const auto found_offset = search->first.second;
    std::shared_ptr<Range> result(new Range(sz, found_offset, self.lock()));
    const auto del_range = search->second;
    ranges.erase(search);
    auto* const result_previous = del_range.first;
    if (nullptr != result_previous) {
        result_previous->next_key = std::nullopt;
        result_previous->next = result.get();
        result->previous = result_previous;
    }
    auto* const result_next = del_range.second;
    const auto new_size = found_size - sz;
    const auto new_key = new_size == 0 ? std::nullopt : std::make_optional(std::make_pair(new_size, found_offset + sz));
    if (new_key.has_value()) {
        result->next_key = new_key;
        ranges.emplace(*new_key, std::make_pair(result.get(), result_next));
    }
    if (nullptr != result_next) {
        result_next->previous_key = new_key;
        result_next->previous = result.get();
        result->next = result_next;
    }
    return result;
}
