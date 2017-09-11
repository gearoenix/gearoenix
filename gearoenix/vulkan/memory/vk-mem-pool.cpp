#include "vk-mem-pool.hpp"
gearoenix::render::memory::Pool::Pool(
    const std::shared_ptr<device::Logical>& logical_device)
    : logical_device(logical_device)
{
}

const std::shared_ptr<gearoenix::render::device::Logical>& gearoenix::render::memory::Pool::get_logical_device() const
{
    return logical_device;
}
