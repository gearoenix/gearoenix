#include "gx-rnd-gltf-mesh-manager.hpp"
#include "../../core/ecs/gx-cr-ecs-entity.hpp"
#include "../../math/gx-math-vertex.hpp"
#include "../../physics/gx-phs-boundary.hpp"
#include "../engine/gx-rnd-eng-engine.hpp"
#include "../mesh/gx-rnd-msh-mesh.hpp"
#include "gx-rnd-gltf-loader.hpp"
#include <tiny_gltf.h>

gearoenix::render::gltf::MeshManager::MeshManager(Loader& loader) noexcept
    : loader(loader)
{
}

gearoenix::render::gltf::MeshManager::~MeshManager() noexcept = default;

void gearoenix::render::gltf::MeshManager::get(
    const std::size_t index,
    std::shared_ptr<core::ecs::EntitySharedBuilder> builder,
    const gearoenix::core::sync::EndCallerIgnored& c) noexcept
{
    {
        auto search = loaded_meshes.find(index);
        if (loaded_meshes.end() != search) {
            if (auto mesh = search->second.lock()) {
                mesh->set_component(builder);
                return;
            }
        }
    }

    const auto& data = *loader.get_data();
    const auto& mesh = data.meshes[data.nodes[index].mesh];
    const auto& pris = mesh.primitives;
    const auto& acs = data.accessors;
    const auto& bvs = data.bufferViews;
    const auto& bfs = data.buffers;
    auto mesh_data_index = 0;
    GX_CHECK_EQUAL_D(pris.size(), 1) // Only one primitive for a mesh is acceptable
    const auto& pri = pris[0];
    auto pos_ai = static_cast<std::size_t>(-1);
    auto nrm_ai = static_cast<std::size_t>(-1);
    auto tng_ai = static_cast<std::size_t>(-1);
    auto txc_ai = static_cast<std::size_t>(-1);
    const auto& attrs = pri.attributes;
    GX_CHECK_EQUAL_D(attrs.size(), 4)
    for (const auto& att : attrs) {
        if ("POSITION" == att.first) {
            pos_ai = static_cast<std::size_t>(att.second);
        } else if ("NORMAL" == att.first) {
            nrm_ai = static_cast<std::size_t>(att.second);
        } else if ("TANGENT" == att.first) {
            tng_ai = static_cast<std::size_t>(att.second);
        } else if ("TEXCOORD_0" == att.first) {
            txc_ai = static_cast<std::size_t>(att.second);
        } else
            GX_UNEXPECTED
    }
    GX_CHECK_EQUAL_D(pos_ai, static_cast<std::size_t>(-1))
    GX_CHECK_EQUAL_D(nrm_ai, static_cast<std::size_t>(-1))
    GX_CHECK_EQUAL_D(tng_ai, static_cast<std::size_t>(-1))
    GX_CHECK_EQUAL_D(txc_ai, static_cast<std::size_t>(-1))

    GX_CHECK_NOT_EQUAL_D(-1, pri.indices)
    GX_CHECK_NOT_EQUAL_D(-1, pri.material)

    const auto& pos_a = acs[pos_ai];
    const auto& nrm_a = acs[nrm_ai];
    const auto& tng_a = acs[tng_ai];
    const auto& txc_a = acs[txc_ai];
    const auto& ids_a = acs[pri.indices];

    GX_CHECK_EQUAL_D(pos_a.count, nrm_a.count)
    GX_CHECK_EQUAL_D(pos_a.count, tng_a.count)
    GX_CHECK_EQUAL_D(pos_a.count, txc_a.count)

    GX_CHECK_EQUAL_D(pos_a.type, TINYGLTF_TYPE_VEC3)
    GX_CHECK_EQUAL_D(nrm_a.type, TINYGLTF_TYPE_VEC3)
    GX_CHECK_EQUAL_D(tng_a.type, TINYGLTF_TYPE_VEC4)
    GX_CHECK_EQUAL_D(txc_a.type, TINYGLTF_TYPE_VEC2)
    GX_CHECK_EQUAL_D(ids_a.type, TINYGLTF_TYPE_SCALAR)

    GX_CHECK_EQUAL_D(pos_a.componentType, TINYGLTF_COMPONENT_TYPE_FLOAT)
    GX_CHECK_EQUAL_D(nrm_a.componentType, TINYGLTF_COMPONENT_TYPE_FLOAT)
    GX_CHECK_EQUAL_D(tng_a.componentType, TINYGLTF_COMPONENT_TYPE_FLOAT)
    GX_CHECK_EQUAL_D(txc_a.componentType, TINYGLTF_COMPONENT_TYPE_FLOAT)
    GX_CHECK_EQUAL_D(ids_a.componentType, TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT)

    const auto& pos_max = pos_a.maxValues;
    const auto& pos_min = pos_a.maxValues;
    GX_CHECK_EQUAL_D(pos_max.size(), 3)
    GX_CHECK_EQUAL_D(pos_min.size(), 3)

    builder->get_builder().add_component(physics::Boundary(
        math::Vec3(pos_max[0], pos_max[1], pos_max[2]),
        math::Vec3(pos_min[0], pos_min[1], pos_min[2])));

    std::vector<math::BasicVertex> vertices(pos_a.count);
    std::vector<std::uint32_t> indices(ids_a.count);

    const auto& pos_bv = bvs[pos_a.bufferView];
    const auto& nrm_bv = bvs[nrm_a.bufferView];
    const auto& tng_bv = bvs[tng_a.bufferView];
    const auto& txc_bv = bvs[txc_a.bufferView];
    const auto& ids_bv = bvs[ids_a.bufferView];

    const auto* const pos_b = reinterpret_cast<const math::Vec3<float>*>(bfs[pos_bv.buffer].data.data());
    const auto* const nrm_b = reinterpret_cast<const math::Vec3<float>*>(bfs[nrm_bv.buffer].data.data());
    const auto* const tng_b = reinterpret_cast<const math::Vec4<float>*>(bfs[tng_bv.buffer].data.data());
    const auto* const txc_b = reinterpret_cast<const math::Vec2<float>*>(bfs[txc_bv.buffer].data.data());
    const auto* const ids_b = reinterpret_cast<const std::uint32_t*>(bfs[ids_bv.buffer].data.data());

    std::size_t bi = 0;

    for (auto& vertex : vertices) {
        vertex.position = pos_b[bi];
        vertex.normal = nrm_b[bi];
        vertex.tangent = tng_b[bi];
        vertex.uv = txc_b[bi];
        ++bi;
    }
    std::memcpy(indices.data(), ids_b, indices.size() * sizeof(std::remove_reference_t<decltype(indices)>::value_type));
    ++mesh_data_index;
    core::sync::EndCaller<mesh::Mesh> end([c, b { std::move(builder) }](const std::shared_ptr<mesh::Mesh>& m) noexcept {
        m->set_component(b);
    });
    loader.get_e()->create_mesh(std::move(vertices), std::move(indices), end);
}
