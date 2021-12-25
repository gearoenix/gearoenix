#include "gx-dxr-model.hpp"
#ifdef GX_RENDER_DXR_ENABLED
#include "../core/ecs/gx-cr-ecs-entity.hpp"
#include "../core/gx-cr-string.hpp"
#include "../render/material/gx-rnd-mat-pbr.hpp"
#include "../render/mesh/gx-rnd-msh-mesh.hpp"
#include "gx-dxr-buffer.hpp"
#include "gx-dxr-device.hpp"
#include "gx-dxr-engine.hpp"
#include "gx-dxr-mesh.hpp"
#include "gx-dxr-texture.hpp"
#include "gx-dxr-uploader.hpp"
#include "shaders/gx-dxr-shd-common.hpp"
#include <map>

gearoenix::dxr::Model::Model(
    Engine& e,
    std::shared_ptr<Mesh>&& bound_mesh,
    const UINT buffer_size,
    const std::string& name) noexcept
    : core::ecs::Component(this)
    , uniforms {
        UniformBuffer(e, buffer_size, core::String::to_wchar_ptr(name + "-DXR-Uniform-0")),
        UniformBuffer(e, buffer_size, core::String::to_wchar_ptr(name + "-DXR-Uniform-1")),
        UniformBuffer(e, buffer_size, core::String::to_wchar_ptr(name + "-DXR-Uniform-2")),
    }
    , bound_mesh(std::move(bound_mesh))
{
}

gearoenix::dxr::Model::~Model() noexcept = default;

gearoenix::dxr::Model::Model(Model&&) noexcept = default;

gearoenix::dxr::ModelBuilder::ModelBuilder(
    Engine& e,
    const std::string& name,
    std::shared_ptr<render::mesh::Mesh>&& bound_mesh,
    bool is_transformable) noexcept
    : render::model::Builder(e, name, std::move(bound_mesh), is_transformable)
    , e(e)
{
}

void gearoenix::dxr::ModelBuilder::set_material(const render::material::Pbr& mat) noexcept
{
    render::model::Builder::set_material(mat);
    auto& b = entity_builder->get_builder();
    Model m(
        e,
        std::dynamic_pointer_cast<Mesh>(b.get_component<render::model::Model>()->bound_mesh),
        sizeof(ModelUniform),
        *b.get_name());
    for (auto& uniform : m.uniforms) {
        auto& u = *reinterpret_cast<ModelUniform*>(uniform.get_buffer().get_pointer());
        u.colour_factor = mat.get_albedo_factor();
        u.emission_factor__alpha_cutoff = math::Vec4(mat.get_emission_factor(), mat.get_alpha_cutoff());
        u.normal_scale__occlusion_strength = math::Vec4(mat.get_normal_scale(), mat.get_occlusion_strength());
        u.metallic_factor__roughness_factor__radiance_lod_coefficient = math::Vec4(mat.get_metallic_factor(), mat.get_roughness_factor(), mat.get_radiance_lod_coefficient(), 0.0f);
        const auto& at = *dynamic_cast<const Texture2D*>(mat.get_albedo().get());
        u.sampler_albedo_normal_emission.x = at.get_sampler_index();
        u.sampler_albedo_normal_emission.y = at.get_descriptor().resource_index;
        u.sampler_albedo_normal_emission.z = dynamic_cast<const Texture2D*>(mat.get_normal__metallic().get())->get_descriptor().resource_index;
        u.sampler_albedo_normal_emission.w = dynamic_cast<const Texture2D*>(mat.get_emission__roughness().get())->get_descriptor().resource_index;
    }
    b.add_component(std::move(m));
}

gearoenix::dxr::ModelBuilder::~ModelBuilder() noexcept = default;

std::shared_ptr<gearoenix::render::model::Builder> gearoenix::dxr::ModelManager::build(
    std::string&& name,
    std::shared_ptr<render::mesh::Mesh>&& mesh,
    core::sync::EndCallerIgnored&&,
    const bool is_transformable) noexcept
{
    return std::shared_ptr<render::model::Builder>(new ModelBuilder(
        dynamic_cast<Engine&>(e), name, std::move(mesh), is_transformable));
}

gearoenix::dxr::ModelManager::ModelManager(Engine& e) noexcept
    : render::model::Manager(e)
{
}

gearoenix::dxr::ModelManager::~ModelManager() noexcept = default;

#endif
