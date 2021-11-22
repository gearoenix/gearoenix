#include "gx-dxr-mesh.hpp"
#ifdef GX_RENDER_DXR_ENABLED
#include "../core/gx-cr-string.hpp"
#include "gx-dxr-buffer.hpp"
#include "gx-dxr-device.hpp"
#include "gx-dxr-engine.hpp"
#include "gx-dxr-uploader.hpp"

gearoenix::dxr::Mesh::Mesh(std::shared_ptr<GpuBuffer>&& vb, std::shared_ptr<GpuBuffer>&& ib) noexcept
    : core::ecs::Component(this)
    , vb(std::move(vb))
    , ib(std::move(ib))
{
}

gearoenix::dxr::Mesh::~Mesh() noexcept = default;

gearoenix::dxr::Mesh::Mesh(Mesh&&) noexcept = default;

gearoenix::dxr::MeshBuilder::MeshBuilder(
    Engine& e,
    const std::string& name,
    const std::vector<render::PbrVertex>& vertices,
    const std::vector<std::uint32_t>& indices,
    math::Aabb3&& occlusion_box) noexcept
    : render::mesh::Builder(e, name, vertices, indices, std::move(occlusion_box))
{
}

void gearoenix::dxr::MeshBuilder::set_material_type_index(const std::type_index& material_type) noexcept {
    GX_TODO
}

gearoenix::dxr::MeshBuilder::~MeshBuilder() noexcept = default;

gearoenix::dxr::MeshManager::MeshManager(Engine& e) noexcept
    : render::mesh::Manager(e)
{
}

gearoenix::dxr::MeshManager::~MeshManager() noexcept = default;

std::shared_ptr<gearoenix::render::mesh::Builder> gearoenix::dxr::MeshManager::build(
    const std::string& name,
    const std::vector<render::PbrVertex>& vertices,
    const std::vector<std::uint32_t>& indices,
    math::Aabb3&& occlusion_box,
    const core::sync::EndCallerIgnored& c) noexcept
{
    auto& eng = dynamic_cast<Engine&>(e);
    auto* const d = eng.get_device()->get_device().Get();
    const auto& u = eng.get_uploader();

    std::shared_ptr<render::mesh::Builder> r(new MeshBuilder(eng, name, vertices, indices, std::move(occlusion_box)));

    const auto vbn = core::String::to_wstring(name + "-DXR-Vertex-Buffer");
    const auto vsz = vertices.size() * sizeof(render::PbrVertex);
    auto vb = std::make_shared<GpuBuffer>(d, static_cast<UINT>(vsz), vbn.c_str());
    std::vector<std::uint8_t> vbd(vsz);
    std::memcpy(vbd.data(), vertices.data(), vsz);

    const auto ibn = core::String::to_wstring(name + "-DXR-Index-Buffer");
    const auto isz = indices.size() * sizeof(std::uint32_t);
    auto ib = std::make_shared<GpuBuffer>(d, static_cast<UINT>(isz), ibn.c_str());
    std::vector<std::uint8_t> ibd(isz);
    std::memcpy(ibd.data(), indices.data(), isz);

    core::sync::EndCallerIgnored end([r, vb, ib, c]() mutable noexcept -> void {
        r->get_entity_builder()->get_builder().add_component(Mesh(std::move(vb), std::move(ib)));
        (void)c;
    });

    u->upload(std::move(vbd), std::move(vb), core::sync::EndCallerIgnored(end));
    u->upload(std::move(ibd), std::move(ib), std::move(end));

    return r;
}

#endif