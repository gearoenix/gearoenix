#include "gx-gl-mesh.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED
#include "gx-gl-check.hpp"
#include "gx-gl-constants.hpp"
#include "gx-gl-engine.hpp"
#include "gx-gl-label.hpp"
#include "gx-gl-loader.hpp"

gearoenix::gl::Mesh::Mesh(Engine& e, const math::Aabb3<double>& box)
    : render::mesh::Mesh(box)
    , e(e)
{
}

gearoenix::gl::Mesh::~Mesh()
{
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
    core::job::EndCallerShared<render::mesh::Mesh>&& end_callback)
{
    auto m = std::make_shared<Mesh>(gl_e, occlusion_box);
    m->indices_count = static_cast<sizei>(indices.size());
    const auto vs_size = static_cast<sizeiptr>(core::bytes_count(vertices));
    const auto is_size = static_cast<sizeiptr>(sizeof(std::uint32_t) * indices.size());
    end_callback.set_return(m);
    core::job::send_job(e.get_jobs_thread_id(), [c = std::move(end_callback), m = std::move(m), vs = std::move(vertices), is = std::move(indices), vs_size, is_size, name = std::move(name)] {
        GX_GL_CHECK_D;
        glGenVertexArrays(1, &(m->vertex_object));
        glBindVertexArray(m->vertex_object);
        glGenBuffers(1, &(m->vertex_buffer));
        glBindBuffer(GL_ARRAY_BUFFER, m->vertex_buffer);
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
        glGenBuffers(1, &(m->index_buffer));
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m->index_buffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, is_size, is.data(), GL_STATIC_DRAW);
        glBindVertexArray(0);
        set_buffer_label(m->vertex_buffer, name + "-vertex-buffer");
        set_buffer_label(m->index_buffer, name + "-index-buffer");
        set_vertex_array_label(m->vertex_object, name + "-vertex-array");
        GX_GL_CHECK_D;
    });
}

#endif
