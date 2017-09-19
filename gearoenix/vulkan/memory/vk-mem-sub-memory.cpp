#include "vk-mem-sub-memory.hpp"

gearoenix::render::memory::SubMemory::SubMemory(unsigned int size)
    : core::gc::Object(size)
{
}

gearoenix::render::memory::SubMemory::~SubMemory()
{
}
