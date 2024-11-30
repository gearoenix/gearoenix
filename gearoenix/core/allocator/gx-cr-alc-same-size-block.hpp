#pragma once
#include <mutex>
#include <stack>
#include <vector>

namespace gearoenix::core::allocator {
struct SameSizeBlock final {
    const std::int64_t object_size;
    const std::int64_t objects_count_in_each_block;
    const std::int64_t block_bytes_count;

private:
    std::mutex this_lock;
    std::vector<std::vector<unsigned char>> blocks;
    std::stack<unsigned char*> free_pointers;

public:
    SameSizeBlock(std::int64_t object_size, std::int64_t objects_count_in_each_block);
    ~SameSizeBlock();
    [[nodiscard]] unsigned char* alloc();
    void free(unsigned char*);
};
}