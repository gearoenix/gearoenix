#include "gx-rnd-sky-skybox.hpp"
#include "../buffer/gx-rnd-buf-manager.hpp"
#include "../material/gx-rnd-mat-uniform.hpp"
#include "../texture/gx-rnd-txt-texture.hpp"

namespace {
using Mgr = gearoenix::render::buffer::Manager;
constexpr auto albedo_factor_off = offsetof(GxShaderDataMaterial, albedo_factor);
constexpr auto albedo_idx_off = offsetof(GxShaderDataMaterial, albedo_normal_texture_sampler_index);
}

gearoenix::render::skybox::Skybox::Skybox(
    core::ecs::Entity* const entity,
    const core::object_type_index_t final_component_type_index,
    std::shared_ptr<mesh::Mesh>&& bound_mesh,
    std::shared_ptr<texture::Texture>&& bound_texture,
    std::string&& name)
    : Component(entity, final_component_type_index, std::move(name))
    , is_equirectangular(texture::Type::Texture2D == bound_texture->get_info().get_type())
    , is_cube(texture::Type::TextureCube == bound_texture->get_info().get_type())
    , strength(1.0f)
    , uniform(buffer::Manager::get_range(buffer::UniformRegionIndex::materials))
    , bound_mesh(std::move(bound_mesh))
    , bound_texture(std::move(bound_texture))
{
    const auto base = uniform.offset_from_frame_pointer;
    Mgr::upload_to_all_frames_uniforms(base + albedo_factor_off, strength);
    const std::uint32_t idx[2] = {
        this->bound_texture->get_shader_resource_index(),
        this->bound_texture->get_sampler_shader_resource_index(),
    };
    Mgr::upload_to_all_frames_uniforms(base + albedo_idx_off, idx, sizeof(idx));
}

gearoenix::render::skybox::Skybox::~Skybox() = default;

void gearoenix::render::skybox::Skybox::set_strength(const math::Vec4<float>& v)
{
    strength = v;
    Mgr::upload_to_all_frames_uniforms(uniform.offset_from_frame_pointer + albedo_factor_off, v);
}