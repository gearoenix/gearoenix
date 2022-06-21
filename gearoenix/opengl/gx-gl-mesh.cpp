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
    m->indices_count = static_cast<sizei>(indices.size());
    const auto vs_size = static_cast<sizeiptr>(sizeof(std::remove_reference_t<decltype(vertices)>::value_type) * vertices.size());
    const auto is_size = static_cast<sizeiptr>(sizeof(std::uint32_t) * indices.size());
    eng.todos.load([c = std::move(c), m, vs = std::move(vertices), is = std::move(indices), vs_size, is_size] {
        GX_GL_CHECK_D;
        glGenVertexArrays(1, &(m->vertex_object));
        glBindVertexArray(m->vertex_object);
        glGenBuffers(1, &(m->vertex_buffer));
        glBindBuffer(GL_ARRAY_BUFFER, m->vertex_buffer);
        glBufferData(GL_ARRAY_BUFFER, vs_size, vs.data(), GL_STATIC_DRAW);
        glEnableVertexAttribArray(GEAROENIX_GL_VERTEX_BUFFER_ATTRIBUTE_INDEX_POSITION);
        glEnableVertexAttribArray(GEAROENIX_GL_VERTEX_BUFFER_ATTRIBUTE_INDEX_NORMAL);
        glEnableVertexAttribArray(GEAROENIX_GL_VERTEX_BUFFER_ATTRIBUTE_INDEX_TANGENT);
        glEnableVertexAttribArray(GEAROENIX_GL_VERTEX_BUFFER_ATTRIBUTE_INDEX_UV);
        glVertexAttribPointer(GEAROENIX_GL_VERTEX_BUFFER_ATTRIBUTE_INDEX_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(std::remove_reference_t<decltype(vertices)>::value_type), reinterpret_cast<void*>(0 * sizeof(float)));
        glVertexAttribPointer(GEAROENIX_GL_VERTEX_BUFFER_ATTRIBUTE_INDEX_NORMAL, 3, GL_FLOAT, GL_FALSE, sizeof(std::remove_reference_t<decltype(vertices)>::value_type), reinterpret_cast<void*>(3 * sizeof(float)));
        glVertexAttribPointer(GEAROENIX_GL_VERTEX_BUFFER_ATTRIBUTE_INDEX_TANGENT, 4, GL_FLOAT, GL_FALSE, sizeof(std::remove_reference_t<decltype(vertices)>::value_type), reinterpret_cast<void*>(6 * sizeof(float)));
        glVertexAttribPointer(GEAROENIX_GL_VERTEX_BUFFER_ATTRIBUTE_INDEX_UV, 2, GL_FLOAT, GL_FALSE, sizeof(std::remove_reference_t<decltype(vertices)>::value_type), reinterpret_cast<void*>(10 * sizeof(float)));
        glGenBuffers(1, &(m->index_buffer));
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m->index_buffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, is_size, is.data(), GL_STATIC_DRAW);
        glBindVertexArray(0);
        GX_GL_CHECK_D;
    });
    return m;
}

std::shared_ptr<gearoenix::render::mesh::Mesh> gearoenix::gl::MeshManager::build(
    std::string&&,
    std::vector<render::PbrVertexAnimated>&& vertices,
    std::vector<std::uint32_t>&& indices,
    math::Aabb3<double>&& occlusion_box,
    core::sync::EndCallerIgnored&& c) noexcept
{
    auto& eng = dynamic_cast<Engine&>(e);
    auto m = std::make_shared<Mesh>(eng, occlusion_box);
    m->indices_count = static_cast<sizei>(indices.size());
    const auto vs_size = static_cast<sizeiptr>(sizeof(std::remove_reference_t<decltype(vertices)>::value_type) * vertices.size());
    const auto is_size = static_cast<sizeiptr>(sizeof(std::uint32_t) * indices.size());
    eng.todos.load([c = std::move(c), m, vs = std::move(vertices), is = std::move(indices), vs_size, is_size] {
        GX_GL_CHECK_D;
        glGenVertexArrays(1, &(m->vertex_object));
        glBindVertexArray(m->vertex_object);
        glGenBuffers(1, &(m->vertex_buffer));
        glBindBuffer(GL_ARRAY_BUFFER, m->vertex_buffer);
        glBufferData(GL_ARRAY_BUFFER, vs_size, vs.data(), GL_STATIC_DRAW);
        glEnableVertexAttribArray(GEAROENIX_GL_VERTEX_BUFFER_ATTRIBUTE_INDEX_POSITION);
        glEnableVertexAttribArray(GEAROENIX_GL_VERTEX_BUFFER_ATTRIBUTE_INDEX_NORMAL);
        glEnableVertexAttribArray(GEAROENIX_GL_VERTEX_BUFFER_ATTRIBUTE_INDEX_TANGENT);
        glEnableVertexAttribArray(GEAROENIX_GL_VERTEX_BUFFER_ATTRIBUTE_INDEX_UV);
        glEnableVertexAttribArray(GEAROENIX_GL_VERTEX_BUFFER_ATTRIBUTE_INDEX_BONE_WEIGHTS);
        glEnableVertexAttribArray(GEAROENIX_GL_VERTEX_BUFFER_ATTRIBUTE_INDEX_BONE_INDEDICES);
        glVertexAttribPointer(GEAROENIX_GL_VERTEX_BUFFER_ATTRIBUTE_INDEX_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(std::remove_reference_t<decltype(vertices)>::value_type), reinterpret_cast<void*>(0 * sizeof(float)));
        glVertexAttribPointer(GEAROENIX_GL_VERTEX_BUFFER_ATTRIBUTE_INDEX_NORMAL, 3, GL_FLOAT, GL_FALSE, sizeof(std::remove_reference_t<decltype(vertices)>::value_type), reinterpret_cast<void*>(3 * sizeof(float)));
        glVertexAttribPointer(GEAROENIX_GL_VERTEX_BUFFER_ATTRIBUTE_INDEX_TANGENT, 4, GL_FLOAT, GL_FALSE, sizeof(std::remove_reference_t<decltype(vertices)>::value_type), reinterpret_cast<void*>(6 * sizeof(float)));
        glVertexAttribPointer(GEAROENIX_GL_VERTEX_BUFFER_ATTRIBUTE_INDEX_UV, 2, GL_FLOAT, GL_FALSE, sizeof(std::remove_reference_t<decltype(vertices)>::value_type), reinterpret_cast<void*>(10 * sizeof(float)));
        glVertexAttribPointer(GEAROENIX_GL_VERTEX_BUFFER_ATTRIBUTE_INDEX_BONE_WEIGHTS, 4, GL_FLOAT, GL_FALSE, sizeof(std::remove_reference_t<decltype(vertices)>::value_type), reinterpret_cast<void*>(12 * sizeof(float)));
        glVertexAttribPointer(GEAROENIX_GL_VERTEX_BUFFER_ATTRIBUTE_INDEX_BONE_INDEDICES, 4, GL_FLOAT, GL_FALSE, sizeof(std::remove_reference_t<decltype(vertices)>::value_type), reinterpret_cast<void*>(16 * sizeof(float)));
        glGenBuffers(1, &(m->index_buffer));
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m->index_buffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, is_size, is.data(), GL_STATIC_DRAW);
        glBindVertexArray(0);
        GX_GL_CHECK_D;
    });
    return m;
}

#endif
