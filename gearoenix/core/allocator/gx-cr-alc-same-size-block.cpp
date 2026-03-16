#include "gx-cr-alc-same-size-block.hpp"
#include "../gx-cr-build-configuration.hpp"
#include "../macro/gx-cr-mcr-assert.hpp"
#include <cstring>

gearoenix::core::allocator::SameSizeBlock::SameSizeBlock(const std::int64_t object_size, const std::int64_t objects_count_in_each_block)
    : object_size(object_size)
    , objects_count_in_each_block(objects_count_in_each_block)
    , block_bytes_count(object_size * objects_count_in_each_block)
{
}

gearoenix::core::allocator::SameSizeBlock::~SameSizeBlock()
{
    if constexpr (GX_DEBUG_MODE) {
        for (const auto& block : blocks) {
            for (const auto byte : block) {
                GX_ASSERT_D(0 == byte); // You have undeleted objects
            }
        }
    }
}

unsigned char* gearoenix::core::allocator::SameSizeBlock::alloc()
{
    const std::lock_guard _lg(this_lock);
    if (free_pointers.empty()) {
        blocks.emplace_back(block_bytes_count);
        auto& new_block = blocks.back();
        if constexpr (GX_DEBUG_MODE) {
            std::memset(new_block.data(), 0, block_bytes_count);
        }
        for (std::int64_t i = 0, ptr = 0; i < objects_count_in_each_block; ++i, ptr += object_size) {
            free_pointers.push(&new_block[ptr]);
        }
    }
    const auto result = free_pointers.top();
    free_pointers.pop();
    return result;
}

void gearoenix::core::allocator::SameSizeBlock::free(unsigned char* const p)
{
    const std::lock_guard _lg(this_lock);
    if constexpr (GX_DEBUG_MODE) {
        std::memset(p, 0, object_size);
    }
    free_pointers.push(p);
}
