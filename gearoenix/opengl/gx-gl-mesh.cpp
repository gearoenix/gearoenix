#include "gx-gl-mesh.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED
#include "gx-gl-check.hpp"
#include "gx-gl-constants.hpp"
#include "gx-gl-engine.hpp"
#include "gx-gl-loader.hpp"

gearoenix::gl::Mesh::Mesh(Engine& e, const math::Aabb3<double>& box) noexcept
    : render::mesh::Mesh(box)
    , e(e)
{
}

gearoenix::gl::Mesh::~Mesh() noexcept
{
    e.todos.load([=] {
        GX_GL_CHECK_D;
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glDeleteVertexArrays(1, &vertex_object);
        glDeleteBuffers(1, &vertex_buffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glDeleteBuffers(1, &index_buffer);
        GX_GL_CHECK_D;
    });
}

gearoenix::gl::MeshManager::MeshManager(Engine& e) noexcept
    : render::mesh::Manager(e)
    , gl_e(e)
{
}

gearoenix::gl::MeshManager::~MeshManager() noexcept = default;

std::shared_ptr<gearoenix::render::mesh::Mesh> gearoenix::gl::MeshManager::build(
    std::string&& name,
    render::Vertices&& vertices,
    std::vector<std::uint32_t>&& indices,
    math::Aabb3<double>&& occlusion_box,
    const core::sync::EndCaller& end_callback) noexcept
{
    auto m = std::make_shared<Mesh>(gl_e, occlusion_box);
    m->indices_count = static_cast<sizei>(indices.size());
    const auto vs_size = static_cast<sizeiptr>(core::bytes_count(vertices));
    const auto is_size = static_cast<sizeiptr>(sizeof(std::uint32_t) * indices.size());
    gl_e.todos.load([c = end_callback, m, vs = std::move(vertices), is = std::move(indices), vs_size, is_size] {
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
        GX_GL_CHECK_D;
    });
    return m;
}

#endif
