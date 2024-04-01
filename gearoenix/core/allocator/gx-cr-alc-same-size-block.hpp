#ifndef GEAROENIX_CORE_ALLOCATOR_SAME_SIZE_BLOCK_HPP
#define GEAROENIX_CORE_ALLOCATOR_SAME_SIZE_BLOCK_HPP
#include <mutex>
#include <stack>
#include <vector>

namespace gearoenix::core::allocator {
struct SameSizeBlock final {
    const std::size_t object_size;
    const std::size_t objects_count_in_each_block;
    const std::size_t block_bytes_count;

private:
    std::mutex this_lock;
    std::vector<std::vector<unsigned char>> blocks;
    std::stack<unsigned char*> free_pointers;

public:
    SameSizeBlock(std::size_t object_size, std::size_t objects_count_in_each_block);
    ~SameSizeBlock();
    [[nodiscard]] unsigned char* alloc();
    void free(unsigned char*);
};
}

#endif