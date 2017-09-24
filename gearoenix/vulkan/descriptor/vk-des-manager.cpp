#include "vk-des-manager.hpp"
#include "../../core/cache/cr-cache-cacher.hpp"
#include "../../core/cr-static.hpp"
#include "../buffer/vk-buf-buffer.hpp"
#include "../buffer/vk-buf-manager.hpp"
#include "vk-des-pool.hpp"
#include "vk-des-set.hpp"

gearoenix::render::descriptor::Manager::Manager(buffer::Manager* bufmgr)
    : dev(bufmgr->get_buffer()->get_logical_device())
    , bufmgr(bufmgr)
{
    pool = new Pool(dev);
    cacher = new core::cache::Cacher();
}

gearoenix::render::descriptor::Manager::~Manager()
{
    delete cacher;
    delete pool;
}

const gearoenix::render::device::Logical* gearoenix::render::descriptor::Manager::get_logical_device() const
{
    return dev;
}

const gearoenix::render::buffer::Manager* gearoenix::render::descriptor::Manager::get_buffer_manager() const
{
    return bufmgr;
}

const gearoenix::render::descriptor::Pool* gearoenix::render::descriptor::Manager::get_pool() const
{
    return pool;
}

std::shared_ptr<gearoenix::render::descriptor::Set> gearoenix::render::descriptor::Manager::get_set(shader::Id sid)
{
    std::function<std::shared_ptr<Set>()> fn_new = [this, sid] {
        VkDescriptorBufferInfo buff_info;
        setz(buff_info);
        buff_info.buffer = bufmgr->get_buffer()->get_vulkan_data();
        buff_info.range = shader::Shader::get_uniform_size(sid);
        return std::shared_ptr<Set>(new Set(sid, pool, buff_info));
    };
    return cacher->get(sid, fn_new);
}

std::shared_ptr<gearoenix::render::descriptor::Set> gearoenix::render::descriptor::Manager::get_cached_set(shader::Id sid) const
{
    return cacher->get<Set>(static_cast<core::Id>(sid));
}
