//#include "gx-vk-buf-manager.hpp"
//#ifdef GX_USE_VULKAN
//#include "../engine/gx-vk-eng-engine.hpp"
//#include "../memory/gx-vk-mem-manager.hpp"
//#include "gx-vk-buf-static.hpp"
//#include "gx-vk-buf-uniform.hpp"
//
//gearoenix::vulkan::buffer::Manager::Manager(
//    std::shared_ptr<memory::Manager> mm,
//    engine::Engine* eng) noexcept
//    : render::buffer::Manager(eng)
//    , memory_manager(std::move(mm))
//{
//}
//
//gearoenix::vulkan::buffer::Manager::~Manager() noexcept = default;
//
//std::shared_ptr<gearoenix::render::buffer::Uniform> gearoenix::vulkan::buffer::Manager::create_uniform(const std::size_t size) noexcept
//{
//    VkBufferCreateInfo info;
//    GX_SET_ZERO(info)
//    info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
//    info.size = static_cast<VkDeviceSize>(size);
//    info.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
//    auto [buff, mem, data] = memory_manager->create(info, memory::Usage::CpuToGpu);
//    return std::make_shared<Uniform>(size, buff, std::move(mem), data, dynamic_cast<engine::Engine*>(e));
//}
//
//std::shared_ptr<gearoenix::render::buffer::Static> gearoenix::vulkan::buffer::Manager::create_static(
//    const std::vector<math::BasicVertex>& vertices,
//    const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept
//{
//    const std::size_t size = vertices.size() * sizeof(math::BasicVertex);
//    VkBufferCreateInfo info;
//    GX_SET_ZERO(info)
//    info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
//    info.size = static_cast<VkDeviceSize>(size);
//    info.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
//    auto [buff, mem, data] = memory_manager->create(info, memory::Usage::Cpu);
//    auto src_buff = std::make_shared<Uniform>(size, buff, std::move(mem), data, dynamic_cast<engine::Engine*>(e));
//    std::tie(buff, mem, data) = memory_manager->create(info, memory::Usage::Gpu);
//    auto dst_buff = std::make_shared<Static>(size, buff, std::move(mem), dynamic_cast<engine::Engine*>(e));
//    copy_buffers.emplace_back(std::move(src_buff), dst_buff);
//}
//
//#endif
