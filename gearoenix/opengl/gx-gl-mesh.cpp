#include "gx-gl-mesh.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED
#include "../core/allocator/gx-cr-alc-shared-array.hpp"
#include "../render/material/gx-rnd-mat-material.hpp"
#include "gx-gl-check.hpp"
#include "gx-gl-constants.hpp"
#include "gx-gl-engine.hpp"
#include "gx-gl-label.hpp"
#include "gx-gl-loader.hpp"
#include "material/gx-gl-material.hpp"

namespace {
const auto buffer_allocator = gearoenix::core::allocator::SharedArray<gearoenix::gl::Buffer, gearoenix::render::mesh::Buffer::MAX_COUNT>::construct();
const auto mesh_allocator = gearoenix::core::allocator::SharedArray<gearoenix::gl::Mesh, gearoenix::render::mesh::Mesh::MAX_COUNT>::construct();
}

gearoenix::gl::Buffer::Buffer(Engine& e, const math::Aabb3<double>& box)
    : render::mesh::Buffer(box)
    , e(e)
{
}

gearoenix::gl::Buffer::~Buffer()
{
    GX_ASSERT_D(static_cast<uint>(-1) != vertex_object);
    core::job::send_job(e.get_jobs_thread_id(), [vo = vertex_object, vb = vertex_buffer, ib = index_buffer] {
        GX_GL_CHECK_D;
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glDeleteVertexArrays(1, &vo);
        glDeleteBuffers(1, &vb);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glDeleteBuffers(1, &ib);
        GX_GL_CHECK_D;
    });
    vertex_object = vertex_buffer = index_buffer = static_cast<uint>(-1);
}

void gearoenix::gl::Buffer::construct(
    Engine& e,
    std::string&& name,
    render::Vertices&& vertices,
    std::vector<std::uint32_t>&& indices,
    const math::Aabb3<double>& occlusion_box,
    core::job::EndCallerShared<render::mesh::Buffer>&& end_callback)
{
    auto buffer = buffer_allocator->make_shared(e, occlusion_box);
    buffer->indices_count = static_cast<sizei>(indices.size());
    const auto vs_size = static_cast<sizeiptr>(core::bytes_count(vertices));
    const auto is_size = static_cast<sizeiptr>(sizeof(std::uint32_t) * indices.size());
    end_callback.set_return(buffer);
    core::job::send_job(e.get_jobs_thread_id(), [c = std::move(end_callback), b = std::move(buffer), vs = std::move(vertices), is = std::move(indices), vs_size, is_size, name = std::move(name)] {
        GX_GL_CHECK_D;
        glGenVertexArrays(1, &(b->vertex_object));
        glBindVertexArray(b->vertex_object);
        glGenBuffers(1, &(b->vertex_buffer));
        glBindBuffer(GL_ARRAY_BUFFER, b->vertex_buffer);
        glBufferData(GL_ARRAY_BUFFER, vs_size, render::get_data(vs), GL_STATIC_DRAW);
        glEnableVertexAttribArray(GEAROENIX_GL_VERTEX_BUFFER_ATTRIBUTE_INDEX_POSITION);
        glEnableVertexAttribArray(GEAROENIX_GL_VERTEX_BUFFER_ATTRIBUTE_INDEX_NORMAL);
        glEnableVertexAttribArray(GEAROENIX_GL_VERTEX_BUFFER_ATTRIBUTE_INDEX_TANGENT);
        glEnableVertexAttribArray(GEAROENIX_GL_VERTEX_BUFFER_ATTRIBUTE_INDEX_UV);
        if (render::has_bone_weights(vs)) {
            glEnableVertexAttribArray(GEAROENIX_GL_VERTEX_BUFFER_ATTRIBUTE_INDEX_BONE_WEIGHTS);
        }
        if (render::has_bone_indices(vs)) {
            glEnableVertexAttribArray(GEAROENIX_GL_VERTEX_BUFFER_ATTRIBUTE_INDEX_BONE_INDICES);
        }
        const auto stride = static_cast<sizei>(render::get_element_size(vs));
        std::size_t pointer = 0;
        glVertexAttribPointer(GEAROENIX_GL_VERTEX_BUFFER_ATTRIBUTE_INDEX_POSITION, 3, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<void*>(pointer));
        pointer += 3 * sizeof(float);
        glVertexAttribPointer(GEAROENIX_GL_VERTEX_BUFFER_ATTRIBUTE_INDEX_NORMAL, 3, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<void*>(pointer));
        pointer += 3 * sizeof(float);
        glVertexAttribPointer(GEAROENIX_GL_VERTEX_BUFFER_ATTRIBUTE_INDEX_TANGENT, 4, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<void*>(pointer));
        pointer += 4 * sizeof(float);
        glVertexAttribPointer(GEAROENIX_GL_VERTEX_BUFFER_ATTRIBUTE_INDEX_UV, 2, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<void*>(pointer));
        pointer += 2 * sizeof(float);
        if (render::has_bone_weights(vs)) {
            glVertexAttribPointer(GEAROENIX_GL_VERTEX_BUFFER_ATTRIBUTE_INDEX_BONE_WEIGHTS, 4, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<void*>(pointer));
            pointer += 4 * sizeof(float);
        }
        if (render::has_bone_indices(vs)) {
            glVertexAttribPointer(GEAROENIX_GL_VERTEX_BUFFER_ATTRIBUTE_INDEX_BONE_INDICES, 4, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<void*>(pointer));
        }
        glGenBuffers(1, &(b->index_buffer));
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, b->index_buffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, is_size, is.data(), GL_STATIC_DRAW);
        glBindVertexArray(0);
        set_buffer_label(b->vertex_buffer, name + "-vertex-buffer");
        set_buffer_label(b->index_buffer, name + "-index-buffer");
        set_vertex_array_label(b->vertex_object, name + "-vertex-array");
        GX_GL_CHECK_D;
    });
}

gearoenix::gl::Mesh::Mesh(
    Engine& e,
    std::shared_ptr<render::mesh::Buffer>&& buffer,
    std::shared_ptr<render::material::Material>&& material)
    : render::mesh::Mesh(std::move(buffer), std::move(material))
    , e(e)
    , gl_buffer(std::dynamic_pointer_cast<Buffer>(this->buffer))
    , gl_material(std::dynamic_pointer_cast<material::Material>(bound_material))
    , cached_vertex_object(gl_buffer->get_vertex_object())
    , cached_vertex_buffer(gl_buffer->get_vertex_buffer())
    , cached_index_buffer(gl_buffer->get_index_buffer())
    , cached_indices_count(gl_buffer->get_indices_count())
{
}

gearoenix::gl::Mesh::~Mesh() = default;

void gearoenix::gl::Mesh::construct(
    Engine& e,
    std::shared_ptr<render::mesh::Buffer>&& buffer,
    std::shared_ptr<render::material::Material>&& material,
    const core::job::EndCallerShared<render::mesh::Mesh>& end_callback)
{
    end_callback.set_return(mesh_allocator->make_shared(e, std::move(buffer), std::move(material)));
}

gearoenix::gl::MeshManager::MeshManager(Engine& e)
    : render::mesh::Manager(e)
    , gl_e(e)
{
}

gearoenix::gl::MeshManager::~MeshManager() = default;

void gearoenix::gl::MeshManager::build(
    std::string&& name,
    render::Vertices&& vertices,
    std::vector<std::uint32_t>&& indices,
    const math::Aabb3<double>& occlusion_box,
    core::job::EndCallerShared<render::mesh::Buffer>&& end_callback)
{
    Buffer::construct(gl_e, std::move(name), std::move(vertices), std::move(indices), occlusion_box, std::move(end_callback));
}

void gearoenix::gl::MeshManager::build(
    std::shared_ptr<render::mesh::Buffer>&& buffer,
    std::shared_ptr<render::material::Material>&& material,
    core::job::EndCallerShared<render::mesh::Mesh>&& end_callback)
{
    Mesh::construct(gl_e, std::move(buffer), std::move(material), std::move(end_callback));
}

#endif
