#include "vk-mem-sub-memory.hpp"
#ifdef USE_VULKAN

gearoenix::render::memory::SubMemory::SubMemory(unsigned int size, Memory* mem)
    : core::gc::Object(size)
    , mem(mem)
{
}

gearoenix::render::memory::SubMemory::~SubMemory()
{
}

const gearoenix::render::memory::Memory* gearoenix::render::memory::SubMemory::get_memory() const
{
    return mem;
}
#endif
