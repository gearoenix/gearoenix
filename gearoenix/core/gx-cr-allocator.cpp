#include "gx-cr-allocator.hpp"

void gearoenix::core::Allocator::deallocate(const std::size_t sz, const std::size_t off) noexcept
{
    const auto end = off + sz;
    auto final_end = end;
    auto final_start = off;
    GX_GUARD_LOCK(this)
    {
        auto end_search = start_to_end.find(end);
        if (start_to_end.end() != end_search) {
            final_end = end_search->second;
            end_to_start.erase(final_end);
            size_start.erase(std::make_pair(final_end - end_search->first, end_search->first));
            start_to_end.erase(end_search);
        }
    }
    {
        auto start_search = end_to_start.find(off);
        if (end_to_start.end() != start_search) {
            final_start = start_search->second;
            start_to_end.erase(final_start);
            size_start.erase(std::make_pair(start_search->first - final_start, final_start));
            end_to_start.erase(start_search);
        }
    }
    const auto final_size = final_end - final_start;
    size_start.emplace(final_size, final_start);
    start_to_end.emplace(final_start, final_end);
    end_to_start.emplace(final_end, final_start);
}

gearoenix::core::Allocator::Allocator(
    const std::size_t size,
    const std::size_t offset,
    Allocator* const parent) noexcept
    : size(size)
    , offset(offset)
    , parent(parent)
{
    const auto end = offset + size;
    size_start.emplace(size, offset);
    start_to_end.emplace(offset, end);
    end_to_start.emplace(end, offset);
}

gearoenix::core::Allocator::Allocator(Allocator&& o) noexcept
    : size(o.size)
    , offset(o.offset)
    , size_start(std::move(o.size_start))
    , start_to_end(std::move(o.start_to_end))
    , end_to_start(std::move(o.end_to_start))
    , parent(o.parent)
{
}

gearoenix::core::Allocator::~Allocator() noexcept
{
    if (nullptr != parent)
        parent->deallocate(size, offset);
}

gearoenix::core::Allocator& gearoenix::core::Allocator::operator=(Allocator&& o) noexcept
{
    size = o.size;
    offset = o.offset;
    size_start = std::move(o.size_start);
    start_to_end = std::move(o.start_to_end);
    end_to_start = std::move(o.end_to_start);
    parent = o.parent;
    return *this;
}

std::optional<gearoenix::core::Allocator> gearoenix::core::Allocator::allocate(const std::size_t sz) noexcept
{
    GX_GUARD_LOCK(this)
    const auto size_search = size_start.lower_bound(std::make_pair(sz, 0));
    if (size_start.end() == size_search)
        return std::nullopt;
    const auto result_offset = size_search->second;
    const auto final_size = size_search->first - sz;
    size_start.erase(size_search);
    if (final_size > 0) {
        const auto start = result_offset + sz;
        const auto end = start + final_size;
        size_start.emplace(final_size, start);
        start_to_end.emplace(start, end);
        end_to_start.emplace(end, start);
    }
    return Allocator(sz, result_offset, this);
}
