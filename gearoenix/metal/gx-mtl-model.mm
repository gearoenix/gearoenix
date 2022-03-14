#include "gx-mtl-model.hpp"
#ifdef GX_RENDER_METAL_ENABLED
#import "../core/ecs/gx-cr-ecs-entity.hpp"
#import "../render/model/gx-rnd-mdl-model.hpp"
#import "gx-mtl-engine.hpp"
#import "gx-mtl-math-helper.hpp"
#import "gx-mtl-mesh.hpp"
#import "gx-mtl-pipeline.hpp"
#import "gx-mtl-texture.hpp"
#import "shaders/gx-mtl-shd-common.hpp"

gearoenix::metal::Model::Model(
    Engine& e,
    std::shared_ptr<Mesh>&& bound_mesh,
    const NSUInteger buffer_size,
    const std::string& name) noexcept
    : core::ecs::Component(this)
    , uniform(e.get_buffer_manager()->create_uniform(sizeof(ModelUniform)))
    , gbuffers_filler_args(e, e.get_pipeline_manager()->get_gbuffers_filler_vertex_shader(), GEAROENIX_METAL_GBUFFERS_FILLER_ARGUMENT_BUFFER_BIND_INDEX, name)
    , bound_mesh(std::move(bound_mesh))
{
}

gearoenix::metal::Model::~Model() noexcept = default;

gearoenix::metal::Model::Model(Model&& o) noexcept = default;

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
        sizeof(ModelUniform), *b.get_name());
    for (std::size_t frame_index = 0; frame_index < GEAROENIX_METAL_FRAMES_COUNT; ++frame_index) {
        auto& u = *reinterpret_cast<ModelUniform*>(m.uniform.data[frame_index]);
        u.colour_factor = simd_make_float4(mat.get_albedo_factor());
        u.emission_factor__alpha_cutoff = simd_make_float4(mat.get_emission_factor(), mat.get_alpha_cutoff());
        u.normal_scale__occlusion_strength = simd_make_float4(mat.get_normal_scale(), mat.get_occlusion_strength());
        u.metallic_factor__roughness_factor__radiance_lod_coefficient = simd_make_float4(mat.get_metallic_factor(), mat.get_roughness_factor(), mat.get_radiance_lod_coefficient(), 0.0f);
    }
    [m.gbuffers_filler_args.encoder
        setBuffer:e.get_buffer_manager()->uniforms_gpu
           offset:m.uniform.gpu_offset
          atIndex:GEAROENIX_METAL_GBUFFERS_FILLER_MODEL_UNIFORM_BIND_ID];
    const Texture2D& at = *dynamic_cast<const Texture2D*>(mat.get_albedo().get());
    [m.gbuffers_filler_args.encoder
        setSamplerState:at.sampler
                atIndex:GEAROENIX_METAL_GBUFFERS_FILLER_SAMPLER_BIND_ID];
    [m.gbuffers_filler_args.encoder
        setTexture:at.resource
           atIndex:GEAROENIX_METAL_GBUFFERS_FILLER_ALBEDO_TEXTURE_BIND_ID];
    [m.gbuffers_filler_args.encoder
        setTexture:dynamic_cast<const Texture2D*>(mat.get_normal__metallic().get())->resource
           atIndex:GEAROENIX_METAL_GBUFFERS_FILLER_NORMAL_METALLIC_TEXTURE_BIND_ID];
    [m.gbuffers_filler_args.encoder
        setTexture:dynamic_cast<const Texture2D*>(mat.get_emission__roughness().get())->resource
           atIndex:GEAROENIX_METAL_GBUFFERS_FILLER_EMISSION_ROUGHNESS_TEXTURE_BIND_ID];
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
