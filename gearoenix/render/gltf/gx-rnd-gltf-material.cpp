#include "../engine/gx-rnd-eng-engine.hpp"
#include "../material/gx-rnd-mat-manager.hpp"
#include "../material/gx-rnd-mat-pbr.hpp"
#include "gx-rnd-gltf-context.hpp"

gearoenix::render::gltf::Materials::Materials(const Context& context)
    : context(context)
{
}

gearoenix::render::gltf::Materials::~Materials() = default;

void gearoenix::render::gltf::Materials::initialise(const int index) const
{
    auto& mat = context.data.materials[index];
    auto& gx_mat = *materials[index];
    GX_LOG_D("Loading material: " << mat.name);
    GX_COMPLAIN_D(!mat.doubleSided, "We don't support double sided object yet! In material: " << mat.name);
    const auto& albedo_factor = mat.pbrMetallicRoughness.baseColorFactor;
    GX_ASSERT(albedo_factor.size() == 4);
    gx_mat.get_alpha_cutoff_occlusion_strength_reserved_reserved().x = static_cast<float>(mat.alphaCutoff);
    if ("OPAQUE" == mat.alphaMode) {
        gx_mat.set_transparency(material::Transparency::Opaque);
    } else if ("TRANSPARENT" == mat.alphaMode) {
        gx_mat.set_transparency(material::Transparency::Transparent);
    } else {
        GX_UNEXPECTED;
    }
    GX_ASSERT(mat.emissiveFactor.size() == 3);
    gx_mat.get_emission_roughness_factor().x = static_cast<float>(mat.emissiveFactor[0]);
    gx_mat.get_emission_roughness_factor().y = static_cast<float>(mat.emissiveFactor[1]);
    gx_mat.get_emission_roughness_factor().z = static_cast<float>(mat.emissiveFactor[2]);
    gx_mat.set_emission(std::shared_ptr(context.textures.get(mat.emissiveTexture.index, gx_mat.get_emission())));
    gx_mat.set_normal(std::shared_ptr(context.textures.get(mat.normalTexture.index, gx_mat.get_normal())));
    gx_mat.get_normal_metallic_factor().x = static_cast<float>(mat.normalTexture.scale);
    gx_mat.get_normal_metallic_factor().y = static_cast<float>(mat.normalTexture.scale);
    gx_mat.get_normal_metallic_factor().z = 1.0f; // strange but tinygltf docs says that
    gx_mat.set_occlusion(std::shared_ptr(context.textures.get(mat.occlusionTexture.index, gx_mat.get_occlusion())));
    gx_mat.get_alpha_cutoff_occlusion_strength_reserved_reserved().y = static_cast<float>(mat.occlusionTexture.strength);
    gx_mat.set_albedo(std::shared_ptr(context.textures.get(mat.pbrMetallicRoughness.baseColorTexture.index, gx_mat.get_albedo())));
    gx_mat.get_albedo_factor() = math::Vec4(
        static_cast<float>(albedo_factor[0]),
        static_cast<float>(albedo_factor[1]),
        static_cast<float>(albedo_factor[2]),
        static_cast<float>(albedo_factor[3]));
    gx_mat.set_metallic_roughness(std::shared_ptr(context.textures.get(mat.pbrMetallicRoughness.metallicRoughnessTexture.index, gx_mat.get_metallic_roughness())));
    gx_mat.get_emission_roughness_factor().w = static_cast<float>(mat.pbrMetallicRoughness.roughnessFactor);
    gx_mat.get_normal_metallic_factor().w = static_cast<float>(mat.pbrMetallicRoughness.metallicFactor);
}

void gearoenix::render::gltf::Materials::load(const int index, core::job::EndCaller<>&& end)
{
    engine::Engine::get()->get_material_manager()->get_pbr(
        std::string(context.data.materials[index].name),
        core::job::EndCallerShared<material::Pbr>([this, end = std::move(end), index](std::shared_ptr<material::Pbr>&& m) {
            materials[index] = std::move(m);
            initialise(index);
            (void)end;
        }));
}

void gearoenix::render::gltf::Materials::load(core::job::EndCaller<>&& end)
{
    materials.resize(context.data.materials.size());
    for (int material_id = 0; material_id < context.data.materials.size(); ++material_id) {
        core::job::send_job_to_pool([this, material_id, end]() mutable {
            load(material_id, std::move(end));
        });
    }
}
