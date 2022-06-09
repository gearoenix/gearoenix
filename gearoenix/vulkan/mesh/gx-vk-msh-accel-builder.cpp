#include "gx-vk-msh-accel-builder.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../engine/gx-vk-eng-engine.hpp"
#include "gx-vk-msh-accel.hpp"

gearoenix::vulkan::mesh::AccelBuilder::AccelBuilder(
    engine::Engine& e,
    const std::string& name,
    const std::vector<render::PbrVertex>& vertices,
    const std::vector<std::uint32_t>& indices,
    math::Aabb3&& occlusion_box,
    std::shared_ptr<Accel>&& accel) noexcept
    : render::mesh::Builder(e, name, vertices, indices, std::move(occlusion_box))
    , accel(std::move(accel))
{
}

std::shared_ptr<gearoenix::vulkan::mesh::AccelBuilder> gearoenix::vulkan::mesh::AccelBuilder::construct(
    engine::Engine& e,
    const std::string& name,
    const std::vector<render::PbrVertex>& vertices,
    const std::vector<std::uint32_t>& indices,
    math::Aabb3&& occlusion_box,
    core::sync::EndCaller<AccelBuilder>& c) noexcept
{
    core::sync::EndCaller<Accel> end([c](const std::shared_ptr<Accel>&) {});
    auto accel = Accel::construct(e, name, vertices, indices, end);
    std::shared_ptr<AccelBuilder> result(new AccelBuilder(e, name, vertices, indices, std::move(occlusion_box), std::move(accel)));
    result->self = result;
    c.set_data(result);
    return result;
}

void gearoenix::vulkan::mesh::AccelBuilder::set_material_type_index(const std::type_index& material_type) noexcept {
    GX_TODO
}

gearoenix::vulkan::mesh::AccelBuilder::~AccelBuilder() noexcept = default;

#endif
