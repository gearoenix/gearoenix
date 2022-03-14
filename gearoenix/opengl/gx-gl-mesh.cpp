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
{
}

gearoenix::gl::MeshManager::~MeshManager() noexcept = default;

std::shared_ptr<gearoenix::render::mesh::Mesh> gearoenix::gl::MeshManager::build(
    std::string&&,
    std::vector<render::PbrVertex>&& vertices,
    std::vector<std::uint32_t>&& indices,
    math::Aabb3<double>&& occlusion_box,
    core::sync::EndCallerIgnored&& c) noexcept
{
    auto& eng = dynamic_cast<Engine&>(e);
    auto m = std::make_shared<Mesh>(eng, occlusion_box);
    const auto vs_size = static_cast<gl::sizeiptr>(sizeof(render::PbrVertex) * vertices.size());
    const auto is_size = static_cast<gl::sizeiptr>(sizeof(std::uint32_t) * indices.size());
    eng.todos.load([c = std::move(c), m, vs = std::move(vertices), is = std::move(indices), vs_size, is_size] {
        GX_GL_CHECK_D;
        glGenVertexArrays(1, &m->vertex_object);
        glBindVertexArray(m->vertex_object);
        glGenBuffers(1, &m->vertex_buffer);
        glBindBuffer(GL_ARRAY_BUFFER, m->vertex_buffer);
        glBufferData(GL_ARRAY_BUFFER, vs_size, vs.data(), GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(render::PbrVertex), reinterpret_cast<void*>(0));
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(render::PbrVertex), reinterpret_cast<void*>(3 * sizeof(float)));
        glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(render::PbrVertex), reinterpret_cast<void*>(6 * sizeof(float)));
        glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(render::PbrVertex), reinterpret_cast<void*>(10 * sizeof(float)));
        glGenBuffers(1, &m->index_buffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m->index_buffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, is_size, is.data(), GL_STATIC_DRAW);
        glBindVertexArray(0);
        GX_GL_CHECK_D;
    });
    return m;
}

#endif
