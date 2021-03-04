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
    GX_CHECK_NOT_EQUAL_D(pos_ai, static_cast<std::size_t>(-1))
    GX_CHECK_NOT_EQUAL_D(nrm_ai, static_cast<std::size_t>(-1))
    GX_CHECK_NOT_EQUAL_D(tng_ai, static_cast<std::size_t>(-1))
    GX_CHECK_NOT_EQUAL_D(txc_ai, static_cast<std::size_t>(-1))

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

    const auto* const pos_b = &bfs[pos_bv.buffer].data[pos_bv.byteOffset];
    const std::size_t pos_bi_inc = pos_bv.byteStride > 0 ? pos_bv.byteStride : sizeof(math::Vec3<float>);
    std::size_t bi = 0;
    for (auto& vertex : vertices) {
        vertex.position = *reinterpret_cast<const math::Vec3<float>*>(&pos_b[bi]);
        bi += pos_bi_inc;
    }

    const auto* const nrm_b = &bfs[nrm_bv.buffer].data[nrm_bv.byteOffset];
    const std::size_t nrm_bi_inc = nrm_bv.byteStride > 0 ? nrm_bv.byteStride : sizeof(math::Vec3<float>);
    bi = 0;
    for (auto& vertex : vertices) {
        vertex.normal = *reinterpret_cast<const math::Vec3<float>*>(&nrm_b[bi]);
        bi += nrm_bi_inc;
    }

    const auto* const tng_b = &bfs[tng_bv.buffer].data[tng_bv.byteOffset];
    const std::size_t tng_bi_inc = tng_bv.byteStride > 0 ? tng_bv.byteStride : sizeof(math::Vec4<float>);
    bi = 0;
    for (auto& vertex : vertices) {
        vertex.tangent = *reinterpret_cast<const math::Vec4<float>*>(&tng_b[bi]);
        bi += tng_bi_inc;
    }

    const auto* const txc_b = &bfs[txc_bv.buffer].data[txc_bv.byteOffset];
    const std::size_t txc_bi_inc = txc_bv.byteStride > 0 ? txc_bv.byteStride : sizeof(math::Vec2<float>);
    bi = 0;
    for (auto& vertex : vertices) {
        vertex.uv = *reinterpret_cast<const math::Vec2<float>*>(&txc_b[bi]);
        bi += txc_bi_inc;
    }

    const auto* const ids_b = &bfs[ids_bv.buffer].data[ids_bv.byteOffset];
    const std::size_t ids_bi_inc = [&]() noexcept {
        if (ids_bv.byteStride > 0)
            return ids_bv.byteStride;
        switch (ids_a.componentType) {
        case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
        case TINYGLTF_COMPONENT_TYPE_INT:
            return sizeof(std::uint32_t);
        case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
        case TINYGLTF_COMPONENT_TYPE_SHORT:
            return sizeof(std::uint16_t);
        case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
        case TINYGLTF_COMPONENT_TYPE_BYTE:
            return sizeof(std::uint8_t);
        default:
            GX_UNEXPECTED
        }
    }();
    bi = 0;
    switch (ids_a.componentType) {
    case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
    case TINYGLTF_COMPONENT_TYPE_INT:
        for (auto& i : indices) {
            i = *reinterpret_cast<const std::uint32_t*>(&ids_b[bi]);
            bi += ids_bi_inc;
        }
        break;
    case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
    case TINYGLTF_COMPONENT_TYPE_SHORT:
        for (auto& i : indices) {
            i = *reinterpret_cast<const std::uint16_t*>(&ids_b[bi]);
            bi += ids_bi_inc;
        }
        break;
    case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
    case TINYGLTF_COMPONENT_TYPE_BYTE:
        for (auto& i : indices) {
            i = *reinterpret_cast<const std::uint16_t*>(&ids_b[bi]);
            bi += ids_bi_inc;
        }
        break;
    default:
        GX_UNEXPECTED
    }

    core::sync::EndCaller<mesh::Mesh> end([c, b { std::move(builder) }](const std::shared_ptr<mesh::Mesh>& m) noexcept {
        m->set_component(b);
    });
    loader.get_e()->create_mesh(mesh.name, vertices, indices, end);
}
