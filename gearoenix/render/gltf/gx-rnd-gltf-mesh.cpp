#include "../../core/ecs/gx-cr-ecs-entity.hpp"
#include "../../physics/gx-phs-transformation.hpp"
#include "../engine/gx-rnd-eng-engine.hpp"
#include "../material/gx-rnd-mat-pbr.hpp"
#include "../mesh/gx-rnd-msh-manager.hpp"
#include "../mesh/gx-rnd-msh-mesh.hpp"
#include "../model/gx-rnd-mdl-manager.hpp"
#include "gx-rnd-gltf-context.hpp"
#include "gx-rnd-gltf-transform.hpp"

#define GL_ARRAY_BUFFER 0x8892
#define GL_ELEMENT_ARRAY_BUFFER 0x8893

namespace gearoenix::render::gltf {
struct Mesh final {
    std::vector<std::shared_ptr<mesh::Mesh>> meshes;
};

void load_mesh(const Context& context, const int mesh_index, const int primitive_index, const core::job::EndCaller<>& end_callback)
{
    const auto& msh = context.data.meshes[mesh_index];
    const auto& primitive = msh.primitives[primitive_index];
    GX_ASSERT_D(TINYGLTF_MODE_TRIANGLES == primitive.mode);
    const auto& acs = context.data.accessors;
    const auto& bvs = context.data.bufferViews;
    const auto& bfs = context.data.buffers;
    auto pos_ai = -1;
    auto nrm_ai = -1;
    auto tng_ai = -1;
    auto tx0_ai = -1;
    auto bwt_ai = -1;
    auto bin_ai = -1;
    for (const auto& attrs = primitive.attributes; const auto& [a_name, ai] : attrs) {
        if ("POSITION" == a_name) {
            GX_ASSERT_D(pos_ai == -1);
            pos_ai = ai;
        } else if ("NORMAL" == a_name) {
            GX_ASSERT_D(nrm_ai == -1);
            nrm_ai = ai;
        } else if ("TANGENT" == a_name) {
            GX_ASSERT_D(tng_ai == -1);
            tng_ai = ai;
        } else if ("TEXCOORD_0" == a_name) {
            GX_ASSERT_D(tx0_ai == -1);
            tx0_ai = ai;
        } else if ("WEIGHTS_0" == a_name) {
            GX_ASSERT_D(bwt_ai == -1);
            bwt_ai = ai;
        } else if ("JOINTS_0" == a_name) {
            GX_ASSERT_D(bin_ai == -1);
            bin_ai = ai;
        } else {
            GX_UNEXPECTED;
        }
    }
    GX_ASSERT_D(pos_ai != -1);
    GX_ASSERT_D(nrm_ai != -1);
    const auto has_tangent = tng_ai != -1;
    const auto has_tex_coord0 = tx0_ai != -1;
    const bool is_animated = bwt_ai != -1;
    GX_ASSERT_D(!is_animated || bin_ai != -1);

    GX_ASSERT_D(-1 != primitive.indices);
    GX_ASSERT_D(-1 != primitive.material);

    const auto& pos_a = acs[pos_ai];
    const auto& nrm_a = acs[nrm_ai];
    const auto* const tng_a = has_tangent ? &acs[tng_ai] : nullptr;
    const auto* const tx0_a = has_tex_coord0 ? &acs[tx0_ai] : nullptr;
    const auto* const bwt_a = is_animated ? &acs[bwt_ai] : nullptr;
    const auto* const bin_a = is_animated ? &acs[bin_ai] : nullptr;
    const auto& ids_a = acs[primitive.indices];

    GX_ASSERT_D(pos_a.count > 0);
    GX_ASSERT_D(pos_a.count == nrm_a.count);
    GX_ASSERT_D(!tng_a || pos_a.count == tng_a->count);
    GX_ASSERT_D(!tx0_a || pos_a.count == tx0_a->count);
    GX_ASSERT_D(!is_animated || pos_a.count == bwt_a->count);
    GX_ASSERT_D(!is_animated || pos_a.count == bin_a->count);

    GX_ASSERT_D(pos_a.type == TINYGLTF_TYPE_VEC3);
    GX_ASSERT_D(nrm_a.type == TINYGLTF_TYPE_VEC3);
    GX_ASSERT_D(!tng_a || tng_a->type == TINYGLTF_TYPE_VEC4);
    GX_ASSERT_D(!tx0_a || tx0_a->type == TINYGLTF_TYPE_VEC2);
    GX_ASSERT_D(!is_animated || bwt_a->type == TINYGLTF_TYPE_VEC4);
    GX_ASSERT_D(!is_animated || bin_a->type == TINYGLTF_TYPE_VEC4);
    GX_ASSERT_D(ids_a.type == TINYGLTF_TYPE_SCALAR);

    GX_ASSERT_D(pos_a.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT);
    GX_ASSERT_D(nrm_a.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT);
    GX_ASSERT_D(!tng_a || tng_a->componentType == TINYGLTF_COMPONENT_TYPE_FLOAT);
    GX_ASSERT_D(!tx0_a || tx0_a->componentType == TINYGLTF_COMPONENT_TYPE_FLOAT);
    GX_ASSERT_D(!is_animated || bwt_a->componentType == TINYGLTF_COMPONENT_TYPE_FLOAT);

    const auto& pos_max = pos_a.maxValues;
    const auto& pos_min = pos_a.minValues;
    GX_ASSERT_D(pos_max.size() == 3);
    GX_ASSERT_D(pos_min.size() == 3);

    const math::Aabb3 box(math::Vec3(pos_max[0], pos_max[1], pos_max[2]), math::Vec3(pos_min[0], pos_min[1], pos_min[2]));

    std::vector<PbrVertex> vertices(pos_a.count);
    std::vector<PbrVertexAnimated> animated_vertices(is_animated ? pos_a.count : 0);
    std::vector<std::uint32_t> indices(ids_a.count);

    const auto& pos_bv = bvs[pos_a.bufferView];
    GX_ASSERT_D(pos_bv.target == GL_ARRAY_BUFFER);
    const auto& nrm_bv = bvs[nrm_a.bufferView];
    GX_ASSERT_D(nrm_bv.target == GL_ARRAY_BUFFER);
    const auto* const tng_bv = tng_a ? &bvs[tng_a->bufferView] : nullptr;
    GX_ASSERT_D(!tng_bv || tng_bv->target == GL_ARRAY_BUFFER);
    const auto* const tx0_bv = tx0_a ? &bvs[tx0_a->bufferView] : nullptr;
    GX_ASSERT_D(!tx0_bv || tx0_bv->target == GL_ARRAY_BUFFER);
    const auto* const bwt_bv = is_animated ? &bvs[bwt_a->bufferView] : nullptr;
    GX_ASSERT_D(!bwt_bv || bwt_bv->target == GL_ARRAY_BUFFER);
    const auto* const bin_bv = is_animated ? &bvs[bin_a->bufferView] : nullptr;
    GX_ASSERT_D(!bin_bv || bin_bv->target == GL_ARRAY_BUFFER);
    const auto& ids_bv = bvs[ids_a.bufferView];
    GX_ASSERT_D(ids_bv.target == GL_ELEMENT_ARRAY_BUFFER);

    {
        const auto& buff = bfs[pos_bv.buffer];
        auto bi = pos_bv.byteOffset + pos_a.byteOffset;
        const auto bi_inc = pos_a.ByteStride(pos_bv);
        for (auto& vertex : vertices) {
            vertex.position = *reinterpret_cast<const math::Vec3<float>*>(&buff.data[bi]);
            bi += bi_inc;
        }
        GX_ASSERT_D(buff.data.size() >= bi);
    }

    {
        const auto& buff = bfs[nrm_bv.buffer];
        auto bi = nrm_bv.byteOffset + nrm_a.byteOffset;
        const auto bi_inc = nrm_a.ByteStride(nrm_bv);
        for (auto& vertex : vertices) {
            vertex.normal = *reinterpret_cast<const math::Vec3<float>*>(&buff.data[bi]);
            bi += bi_inc;
        }
        GX_ASSERT_D(buff.data.size() >= bi);
    }

    if (tng_bv) {
        GX_ASSERT_D(tng_a);
        const auto& buff = bfs[tng_bv->buffer];
        auto bi = tng_bv->byteOffset + tng_a->byteOffset;
        GX_ASSERT_D(tng_bv->byteStride <= 0 || tng_bv->byteStride >= sizeof(math::Vec4<float>));
        const auto bi_inc = tng_a->ByteStride(*tng_bv);
        for (auto& vertex : vertices) {
            vertex.tangent = *reinterpret_cast<const math::Vec4<float>*>(&buff.data[bi]);
            bi += bi_inc;
        }
        GX_ASSERT_D(buff.data.size() >= bi);
    }

    if (tx0_bv) {
        GX_ASSERT_D(tx0_a);
        const auto& buff = bfs[tx0_bv->buffer];
        auto bi = tx0_bv->byteOffset + tx0_a->byteOffset;
        const auto bi_inc = tx0_a->ByteStride(*tx0_bv);
        for (auto& vertex : vertices) {
            vertex.uv = *reinterpret_cast<const math::Vec2<float>*>(&buff.data[bi]);
            vertex.uv.y = 1.0f - vertex.uv.y;
            bi += bi_inc;
        }
        GX_ASSERT_D(buff.data.size() >= bi);
    }

    if (bwt_bv) {
        const auto& buff = bfs[bwt_bv->buffer];
        auto bi = bwt_bv->byteOffset + bwt_a->byteOffset;
        const auto bi_inc = bwt_a->ByteStride(*bwt_bv);
        for (auto& vertex : animated_vertices) {
            vertex.bone_weights = *reinterpret_cast<const math::Vec4<float>*>(&buff.data[bi]);
            bi += bi_inc;
        }
        GX_ASSERT_D(buff.data.size() >= bi);
    }

    if (bin_bv) {
        const auto& buff = bfs[bin_bv->buffer];
        auto bi = bin_bv->byteOffset + bin_a->byteOffset;
        const auto bi_inc = bin_a->ByteStride(*bin_bv);
        const auto convert_indices = [&]<typename T>() {
            auto ret = math::Vec4<float>(*reinterpret_cast<const math::Vec4<T>*>(&buff.data[bi])) + math::Numeric::epsilon<float>;
            bi += bi_inc;
            return ret;
        };
        switch (bin_a->componentType) {
        case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
            for (auto& vertex : animated_vertices) {
                vertex.bone_indices = convert_indices.operator()<std::uint32_t>();
            }
            break;
        case TINYGLTF_COMPONENT_TYPE_INT:
            for (auto& vertex : animated_vertices) {
                vertex.bone_indices = convert_indices.operator()<std::int32_t>();
            }
            break;
        case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
            for (auto& vertex : animated_vertices) {
                vertex.bone_indices = convert_indices.operator()<std::uint16_t>();
            }
            break;
        case TINYGLTF_COMPONENT_TYPE_SHORT:
            for (auto& vertex : animated_vertices) {
                vertex.bone_indices = convert_indices.operator()<std::int16_t>();
            }
            break;
        case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
            for (auto& vertex : animated_vertices) {
                vertex.bone_indices = convert_indices.operator()<std::uint8_t>();
            }
            break;
        case TINYGLTF_COMPONENT_TYPE_BYTE:
            for (auto& vertex : animated_vertices) {
                vertex.bone_indices = convert_indices.operator()<std::int8_t>();
            }
            break;
        default:
            GX_UNEXPECTED;
        }
        GX_ASSERT_D(buff.data.size() >= bi);
    }
    {
        const auto& buff = bfs[ids_bv.buffer];
        auto bi = ids_bv.byteOffset + ids_a.byteOffset;
        const auto bi_inc = ids_a.ByteStride(ids_bv);
        switch (ids_a.componentType) {
        case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
        case TINYGLTF_COMPONENT_TYPE_INT:
            for (auto& i : indices) {
                i = *reinterpret_cast<const std::uint32_t*>(&buff.data[bi]);
                bi += bi_inc;
            }
            break;
        case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
        case TINYGLTF_COMPONENT_TYPE_SHORT:
            for (auto& i : indices) {
                i = *reinterpret_cast<const std::uint16_t*>(&buff.data[bi]);
                bi += bi_inc;
            }
            break;
        case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
        case TINYGLTF_COMPONENT_TYPE_BYTE:
            for (auto& i : indices) {
                i = *reinterpret_cast<const std::uint8_t*>(&buff.data[bi]);
                bi += bi_inc;
            }
            break;
        default:
            GX_UNEXPECTED;
        }
        GX_ASSERT_D(buff.data.size() >= bi);
    }

    if (!has_tangent) {
        calculate_tangents(vertices, indices);
    }

    for (auto vi = decltype(animated_vertices.size()) { 0 }; vi < animated_vertices.size(); ++vi) {
        animated_vertices[vi].base = vertices[vi];
    }

    core::job::EndCallerShared<mesh::Mesh> mesh_end_callback([ctx = &context, mesh_index, primitive_index, end_callback](std::shared_ptr<mesh::Mesh>&& m) {
        ctx->meshes.meshes[mesh_index]->meshes[primitive_index] = std::move(m);
        (void)end_callback;
    });

    auto material = context.materials.materials[primitive.material];

    auto name = msh.name + "-primitive:" + std::to_string(primitive_index);
    if (is_animated) {
        mesh::Manager::get().build(std::move(name), std::move(animated_vertices), std::move(indices), box, std::move(material), std::move(mesh_end_callback));
    } else {
        mesh::Manager::get().build(std::move(name), std::move(vertices), std::move(indices), box, std::move(material), std::move(mesh_end_callback));
    }
}

void load_mesh(const Context& context, const int mesh_index, const core::job::EndCaller<>& end_callback)
{
    const auto& msh = context.data.meshes[mesh_index];
    GX_LOG_D("Loading mesh: " << msh.name);
    context.meshes.meshes[mesh_index]->meshes.resize(msh.primitives.size());
    for (int primitive_index = 0; primitive_index < msh.primitives.size(); ++primitive_index) {
        core::job::send_job_to_pool([&context, mesh_index, primitive_index, end_callback] { load_mesh(context, mesh_index, primitive_index, end_callback); });
    }
}
}

gearoenix::render::gltf::Meshes::Meshes(const Context& context)
    : context(context)
{
}

gearoenix::render::gltf::Meshes::~Meshes() = default;

void gearoenix::render::gltf::Meshes::load(core::job::EndCaller<>&& end)
{
    GX_ASSERT_D(meshes.empty());
    meshes.resize(context.data.meshes.size());
    for (int node_index = 0; node_index < context.data.nodes.size(); ++node_index) {
        if (!is_mesh(node_index)) {
            continue;
        }
        const auto mesh_index = context.data.nodes[node_index].mesh;
        core::job::send_job_to_pool([this, mesh_index, end] {
            meshes[mesh_index] = std::make_unique<Mesh>();
            load_mesh(context, mesh_index, end);
        });
    }
}

bool gearoenix::render::gltf::Meshes::is_mesh(const int node_index) const
{
    if (const auto& node = context.data.nodes[node_index]; node.mesh != -1) {
        GX_ASSERT_D(node.children.empty());
        GX_ASSERT_D(node.camera == -1);
        GX_ASSERT_D(node.emitter == -1);
        GX_ASSERT_D(node.light == -1);
        return true;
    }
    return false;
}

bool gearoenix::render::gltf::Meshes::process(const int node_index, core::ecs::Entity* const parent) const
{
    const auto& node = context.data.nodes[node_index];
    if (!is_mesh(node_index)) {
        return false;
    }
    model::meshes_set_t meshes_set;
    for (const auto& m : meshes[node.mesh]->meshes) {
        meshes_set.insert(m);
    }
    auto entity = model::Manager::get().build(std::string(node.name), parent, std::move(meshes_set), true);
    apply_transform(node_index, context, *entity->get_component<physics::Transformation>());
    return true;
}
