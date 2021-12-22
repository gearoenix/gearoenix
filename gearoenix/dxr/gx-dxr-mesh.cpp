#include "gx-dxr-mesh.hpp"
#ifdef GX_RENDER_DXR_ENABLED
#include "../core/gx-cr-string.hpp"
#include "../render/material/gx-rnd-mat-pbr.hpp"
#include "../render/mesh/gx-rnd-msh-mesh.hpp"
#include "gx-dxr-buffer.hpp"
#include "gx-dxr-device.hpp"
#include "gx-dxr-engine.hpp"
#include "gx-dxr-texture.hpp"
#include "gx-dxr-uploader.hpp"
#include "shaders/gx-dxr-shd-common.hpp"
#include <map>

gearoenix::dxr::Mesh::Mesh(
    std::shared_ptr<GpuBuffer>&& _vb, std::shared_ptr<GpuBuffer>&& _ib,
    const UINT vertex_size, const UINT vertices_size, const UINT indices_count) noexcept
    : core::ecs::Component(this)
    , vb(std::move(_vb))
    , ib(std::move(_ib))
    , indices_count(indices_count)
    , vv {
        .BufferLocation = vb->get_resource()->GetGPUVirtualAddress(), // vertices
        .SizeInBytes = vertices_size,
        .StrideInBytes = vertex_size
    }
    , iv { .BufferLocation = ib->get_resource()->GetGPUVirtualAddress(), // indices
        .SizeInBytes = indices_count * 4,
        .Format = DXGI_FORMAT_R32_UINT }
{
}

gearoenix::dxr::Mesh::~Mesh() noexcept = default;

gearoenix::dxr::Mesh::Mesh(Mesh&&) noexcept = default;

gearoenix::dxr::MeshBuffer::MeshBuffer(Engine& e, UINT buffer_size, LPCWSTR entity_name) noexcept
    : core::ecs::Component(this)
    , uniform(e, buffer_size, entity_name)
{
}

gearoenix::dxr::MeshBuffer::~MeshBuffer() noexcept = default;

gearoenix::dxr::MeshBuffer::MeshBuffer(MeshBuffer&&) noexcept = default;

gearoenix::dxr::MeshBuilder::MeshBuilder(
    Engine& e,
    const std::string& name,
    math::Aabb3&& occlusion_box) noexcept
    : render::mesh::Builder(e, name, std::move(occlusion_box))
    , e(e)
{
}

void gearoenix::dxr::MeshBuilder::set_material(const render::material::Pbr& mat) noexcept
{
    render::mesh::Builder::set_material(mat);
    const auto material_name = core::String::to_wstring(name + "-PBR-Material");
    MeshBuffer mb(e, sizeof(MeshUniform), material_name.c_str());
    auto& b = *reinterpret_cast<MeshUniform*>(mb.uniform.get_buffer().get_pointer());
    b.colour_factor = mat.get_albedo_factor();
    b.emission_factor__alpha_cutoff = math::Vec4(mat.get_emission_factor(), mat.get_alpha_cutoff());
    b.normal_scale__occlusion_strength = math::Vec4(mat.get_normal_scale(), mat.get_occlusion_strength());
    b.metallic_factor__roughness_factor__radiance_lod_coefficient = math::Vec4(mat.get_metallic_factor(), mat.get_roughness_factor(), mat.get_radiance_lod_coefficient(), 0.0f);
    const auto& at = *dynamic_cast<const Texture2D*>(mat.get_albedo().get());
    b.sampler_albedo_normal_emission.x = at.get_sampler_index();
    b.sampler_albedo_normal_emission.y = at.get_descriptor().resource_index;
    entity_builder->get_builder().add_component(std::move(mb));
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

    std::shared_ptr<render::mesh::Builder> r(new MeshBuilder(eng, name, std::move(occlusion_box)));

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

    const auto vertex_size = static_cast<UINT>(sizeof(render::PbrVertex));
    const auto vertices_size = static_cast<UINT>(vsz);
    const auto indices_count = static_cast<UINT>(indices.size());

    core::sync::EndCallerIgnored end([c, r]() noexcept -> void {
        (void)c;
        (void)r;
    });

    r->get_entity_builder()->get_builder().add_component(Mesh(
        std::shared_ptr<GpuBuffer>(vb),
        std::shared_ptr<GpuBuffer>(ib),
        vertex_size, vertices_size, indices_count));

    u->upload(std::move(vbd), std::move(vb), core::sync::EndCallerIgnored(end));
    u->upload(std::move(ibd), std::move(ib), std::move(end));

    return r;
}

#endif
