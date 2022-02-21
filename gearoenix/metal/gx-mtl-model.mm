#include "gx-mtl-model.hpp"
#ifdef GX_RENDER_METAL_ENABLED
#import "../core/ecs/gx-cr-ecs-entity.hpp"
#import "gx-mtl-mesh.hpp"
#import "gx-mtl-engine.hpp"
#import "../render/model/gx-rnd-mdl-model.hpp"
#import "gx-mtl-math-helper.hpp"

gearoenix::metal::Model::Model(
    Engine& e,
    std::shared_ptr<Mesh>&& bound_mesh,
    const NSUInteger buffer_size,
    const std::string& name) noexcept
    : core::ecs::Component(this)
    , bound_mesh(std::move(bound_mesh))
{
    for(std::size_t uniform_index = 0; uniform_index < uniforms.size(); ++uniform_index)
    {
        auto& uniform = uniforms[uniform_index];
        uniform = [e.get_device() newBufferWithLength:buffer_size options:MTLResourceStorageModeShared];
        uniform.label = [NSString stringWithFormat:@"Gearoenix-ModelUniform-%s-%lu", name.c_str(), uniform_index];
    }
}

gearoenix::metal::Model::~Model() noexcept {
    for(auto& uniform: uniforms) if(nil != uniform) [uniform release];
}

gearoenix::metal::Model::Model(Model&& o) noexcept: core::ecs::Component(this) {
    for(std::size_t uniform_index = 0; uniform_index < uniforms.size(); ++uniform_index) {
        auto& uniform = o.uniforms[uniform_index];
        uniforms[uniform_index] = uniform;
        uniform = nil;
    }
}

gearoenix::metal::ModelBuilder::ModelBuilder(
    Engine& e,
    const std::string& name,
    std::shared_ptr<render::mesh::Mesh>&& bound_mesh,
    bool is_transformable) noexcept
    : render::model::Builder(e, name, std::move(bound_mesh), is_transformable)
    , e(e)
{
}

void gearoenix::metal::ModelBuilder::set_material(const render::material::Pbr& mat) noexcept
{
    render::model::Builder::set_material(mat);
    auto& b = entity_builder->get_builder();
    Model m(
        e,
        std::dynamic_pointer_cast<Mesh>(b.get_component<render::model::Model>()->bound_mesh),
        sizeof(ModelUniform),
        *b.get_name());
    for (auto& uniform : m.uniforms) {
        auto& u = *reinterpret_cast<ModelUniform*>(uniform.contents);
        u.colour_factor = simd_make_float4(mat.get_albedo_factor());
        u.emission_factor__alpha_cutoff = simd_make_float4(mat.get_emission_factor(), mat.get_alpha_cutoff());
        u.normal_scale__occlusion_strength = simd_make_float4(mat.get_normal_scale(), mat.get_occlusion_strength());
        u.metallic_factor__roughness_factor__radiance_lod_coefficient = simd_make_float4(mat.get_metallic_factor(), mat.get_roughness_factor(), mat.get_radiance_lod_coefficient(), 0.0f);
        //const auto& at = *dynamic_cast<const Texture2D*>(mat.get_albedo().get());
        //u.sampler_albedo_normal_emission = vector_uint4(
        //    at.get_sampler_index(),
        //    at.get_descriptor().resource_index,
        //    dynamic_cast<const Texture2D*>(mat.get_normal__metallic().get())->get_descriptor().resource_index,
         //   dynamic_cast<const Texture2D*>(mat.get_emission__roughness().get())->get_descriptor().resource_index);
    }
    b.add_component(std::move(m));
}

gearoenix::metal::ModelBuilder::~ModelBuilder() noexcept = default;

std::shared_ptr<gearoenix::render::model::Builder> gearoenix::metal::ModelManager::build(
    std::string&& name,
    std::shared_ptr<render::mesh::Mesh>&& mesh,
    core::sync::EndCallerIgnored&&,
    const bool is_transformable) noexcept
{
    return std::shared_ptr<render::model::Builder>(new ModelBuilder(
        dynamic_cast<Engine&>(e), name, std::move(mesh), is_transformable));
}

gearoenix::metal::ModelManager::ModelManager(Engine& e) noexcept
    : render::model::Manager(e)
{
}

gearoenix::metal::ModelManager::~ModelManager() noexcept = default;

#endif
