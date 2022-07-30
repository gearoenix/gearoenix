#include "gx-rnd-msh-manager.hpp"
#include <boost/container/flat_map.hpp>

gearoenix::render::mesh::Manager::Manager(engine::Engine& e) noexcept
    : e(e)
{
}

gearoenix::render::mesh::Manager::~Manager() noexcept = default;

std::shared_ptr<gearoenix::render::mesh::Mesh> gearoenix::render::mesh::Manager::build_icosphere(std::size_t subdivisions, core::sync::EndCallerIgnored&& end_callback) noexcept
{
    std::string name = "default-icosphere-" + std::to_string(subdivisions);
    {
        std::lock_guard<std::mutex> _lg(meshes_lock);
        if (auto search = meshes.find(name); meshes.end() != search)
            if (auto m = search->second.lock(); nullptr != m)
                return m;
    }
    std::vector<PbrVertex> vertices {
        PbrVertex(
            math::Vec3(0.0f, 0.0f, -1.0f),
            math::Vec3(8.129646857923944e-07f, 0.0f, -1.0f),
            math::Vec4(1.0f, 0.0f, 0.0f, -1.0f),
            math::Vec2(0.4999999701976776f, 0.4999900460243225f)),
        PbrVertex(
            math::Vec3(0.7235999703407288f, -0.5257200002670288f, -0.4472149908542633f),
            math::Vec3(0.7236069440841675f, -0.5257307291030884f, -0.44721388816833496f),
            math::Vec4(1.0f, 0.0f, 0.0f, -1.0f),
            math::Vec2(0.4999999701976776f, 0.4999900460243225f)),
        PbrVertex(
            math::Vec3(-0.27638500928878784f, -0.8506399989128113f, -0.4472149908542633f),
            math::Vec3(-0.27638790011405945f, -0.8506532907485962f, -0.4472121298313141f),
            math::Vec4(1.0f, 0.0f, 0.0f, -1.0f),
            math::Vec2(0.4999999701976776f, 0.4999900460243225f)),
        PbrVertex(
            math::Vec3(0.7235999703407288f, 0.5257200002670288f, -0.4472149908542633f),
            math::Vec3(0.7236069440841675f, 0.5257307291030884f, -0.44721388816833496f),
            math::Vec4(1.0f, 0.0f, 0.0f, -1.0f),
            math::Vec2(0.4999999701976776f, 0.4999900460243225f)),
        PbrVertex(
            math::Vec3(-0.8944249749183655f, 0.0f, -0.4472149908542633f),
            math::Vec3(-0.8944282531738281f, 0.0f, -0.44721153378486633f),
            math::Vec4(1.0f, 0.0f, 0.0f, -1.0f),
            math::Vec2(0.4999999701976776f, 0.4999900460243225f)),
        PbrVertex(
            math::Vec3(-0.27638500928878784f, 0.8506399989128113f, -0.4472149908542633f),
            math::Vec3(-0.27638790011405945f, 0.8506532907485962f, -0.4472121298313141f),
            math::Vec4(1.0f, 0.0f, 0.0f, -1.0f),
            math::Vec2(0.4999999701976776f, 0.4999900460243225f)),
        PbrVertex(
            math::Vec3(0.8944249749183655f, 0.0f, 0.4472149908542633f),
            math::Vec3(0.8944283127784729f, 0.0f, 0.44721153378486633f),
            math::Vec4(1.0f, 0.0f, 0.0f, -1.0f),
            math::Vec2(0.4999999701976776f, 0.4999900460243225f)),
        PbrVertex(
            math::Vec3(0.27638500928878784f, -0.8506399989128113f, 0.4472149908542633f),
            math::Vec3(0.27638792991638184f, -0.8506532907485962f, 0.44721218943595886f),
            math::Vec4(1.0f, 0.0f, 0.0f, -1.0f),
            math::Vec2(0.4999999701976776f, 0.4999900460243225f)),
        PbrVertex(
            math::Vec3(-0.7235999703407288f, -0.5257200002670288f, 0.4472149908542633f),
            math::Vec3(-0.7236069440841675f, -0.5257307291030884f, 0.44721388816833496f),
            math::Vec4(1.0f, 0.0f, 0.0f, -1.0f),
            math::Vec2(0.4999999701976776f, 0.4999900460243225f)),
        PbrVertex(
            math::Vec3(-0.7235999703407288f, 0.5257200002670288f, 0.4472149908542633f),
            math::Vec3(-0.7236069440841675f, 0.5257306694984436f, 0.4472138285636902f),
            math::Vec4(1.0f, 0.0f, 0.0f, -1.0f),
            math::Vec2(0.4999999701976776f, 0.4999900460243225f)),
        PbrVertex(
            math::Vec3(0.27638500928878784f, 0.8506399989128113f, 0.4472149908542633f),
            math::Vec3(0.2763878405094147f, 0.8506532311439514f, 0.4472121596336365f),
            math::Vec4(1.0f, 0.0f, 0.0f, -1.0f),
            math::Vec2(0.4999999701976776f, 0.4999900460243225f)),
        PbrVertex(
            math::Vec3(0.0f, 0.0f, 1.0f),
            math::Vec3(-8.022206543500943e-07f, 7.16268422351618e-09f, 1.0f),
            math::Vec4(1.0f, 0.0f, 0.0f, -1.0f),
            math::Vec2(0.4999999701976776f, 0.4999900460243225f)),
    };

    std::vector<std::uint32_t> indices {
        0, 1, 2, // 0
        1, 0, 3, // 1
        0, 2, 4, // 2
        0, 4, 5, // 3
        0, 5, 3, // 4
        1, 3, 6, // 5
        2, 1, 7, // 6
        4, 2, 8, // 7
        5, 4, 9, // 8
        3, 5, 10, // 9
        1, 6, 7, // 10
        2, 7, 8, // 11
        4, 8, 9, // 12
        5, 9, 10, // 13
        3, 10, 6, // 14
        7, 6, 11, // 15
        8, 7, 11, // 16
        9, 8, 11, // 17
        10, 9, 11, // 18
        6, 10, 11, // 19
    };
    boost::container::flat_map<std::pair<std::uint32_t, std::uint32_t>, std::uint32_t> cached_vertices;

    for (std::size_t subdivision_index = 1; subdivision_index < subdivisions; ++subdivision_index) {
        const std::size_t last_index_index = indices.size();
        std::vector<std::uint32_t> new_indices;
        new_indices.reserve(indices.size() << 2);
        for (std::size_t index_index = 0; index_index < last_index_index;) {
            const auto insert = [&](const std::uint32_t i1, const std::uint32_t i2) noexcept {
                if (auto search = cached_vertices.find(std::make_pair(i1, i2)); cached_vertices.end() != search)
                    return search->second;
                if (auto search = cached_vertices.find(std::make_pair(i2, i1)); cached_vertices.end() != search)
                    return search->second;
                const auto& p1 = vertices[i1];
                const auto& p2 = vertices[i2];
                PbrVertex v;
                v.position = ((p1.position + p2.position) * 0.5f).normalised();
                v.normal = v.position;
                v.uv = p1.uv;
                const auto index = static_cast<std::uint32_t>(vertices.size());
                vertices.push_back(v);
                cached_vertices.emplace(std::make_pair(i1, i2), index);
                return index;
            };

            const auto i1 = indices[index_index];
            ++index_index;
            const auto i2 = indices[index_index];
            ++index_index;
            const auto i3 = indices[index_index];
            ++index_index;

            const auto i12 = insert(i1, i2);
            const auto i13 = insert(i1, i3);
            const auto i23 = insert(i2, i3);

            new_indices.push_back(i1);
            new_indices.push_back(i12);
            new_indices.push_back(i13);

            new_indices.push_back(i12);
            new_indices.push_back(i23);
            new_indices.push_back(i13);

            new_indices.push_back(i12);
            new_indices.push_back(i2);
            new_indices.push_back(i23);

            new_indices.push_back(i13);
            new_indices.push_back(i23);
            new_indices.push_back(i3);
        }

        std::swap(new_indices, indices);
    }

    for (auto& v : vertices) {
        auto tng = std::abs(math::X3D<float>.dot(v.normal)) < 0.1f ? math::X3D<float> : math::Y3D<float>;
        tng -= v.normal * tng.dot(v.normal);
        tng.normalise();
        v.tangent = math::Vec4(tng, 1.0f);
    }

    return build(
        std::move(name),
        std::move(vertices),
        std::move(indices),
        std::move(end_callback));
}

std::shared_ptr<gearoenix::render::mesh::Mesh> gearoenix::render::mesh::Manager::build_plate(core::sync::EndCallerIgnored&& c) noexcept
{
    std::string name = "default-plate-mesh";
    {
        std::lock_guard<std::mutex> _lg(meshes_lock);
        if (auto search = meshes.find(name); meshes.end() != search)
            if (auto m = search->second.lock(); nullptr != m)
                return m;
    }
    return build(
        std::move(name),
        std::vector<PbrVertex> {
            PbrVertex(
                math::Vec3(-1.f, -1.0f, 0.0f),
                math::Vec3(0.0f, 0.0f, 1.0f),
                math::Vec4(1.0f, 0.0f, 0.0f, 1.0f),
                math::Vec2(0.0f, 0.0f)),
            PbrVertex(
                math::Vec3(1.0f, -1.0f, 0.0f),
                math::Vec3(0.0f, 0.0f, 1.0f),
                math::Vec4(1.0f, 0.0f, 0.0f, 1.0f),
                math::Vec2(1.0f, 0.0f)),
            PbrVertex(
                math::Vec3(-1.0f, 1.0f, 0.0f),
                math::Vec3(0.0f, 0.0f, 1.0f),
                math::Vec4(1.0f, 0.0f, 0.0f, 1.0f),
                math::Vec2(0.0f, 1.0f)),
            PbrVertex(
                math::Vec3(1.0f, 1.0f, 0.0f),
                math::Vec3(0.0f, 0.0f, 1.0f),
                math::Vec4(1.0f, 0.0f, 0.0f, 1.0f),
                math::Vec2(1.0f, 1.0f)),
        },
        std::vector<std::uint32_t> { 0, 1, 2, 1, 3, 2 },
        std::move(c));
}

std::shared_ptr<gearoenix::render::mesh::Mesh> gearoenix::render::mesh::Manager::build_cube(core::sync::EndCallerIgnored&& c) noexcept
{
    std::string name = "default-cube-mesh";
    {
        std::lock_guard<std::mutex> _lg(meshes_lock);
        if (auto search = meshes.find(name); meshes.end() != search)
            if (auto m = search->second.lock(); nullptr != m)
                return m;
    }
    return build(
        std::move(name),
        std::vector<PbrVertex> {
            PbrVertex(
                math::Vec3(-1.f, -1.0f, 1.0f),
                math::Vec3(0.0f, 0.0f, 1.0f),
                math::Vec4(1.0f, 0.0f, 0.0f, 1.0f),
                math::Vec2(0.0f, 0.0f)),
            PbrVertex(
                math::Vec3(1.0f, -1.0f, 1.0f),
                math::Vec3(0.0f, 0.0f, 1.0f),
                math::Vec4(1.0f, 0.0f, 0.0f, 1.0f),
                math::Vec2(1.0f, 0.0f)),
            PbrVertex(
                math::Vec3(-1.0f, 1.0f, 1.0f),
                math::Vec3(0.0f, 0.0f, 1.0f),
                math::Vec4(1.0f, 0.0f, 0.0f, 1.0f),
                math::Vec2(0.0f, 1.0f)),
            PbrVertex(
                math::Vec3(1.0f, 1.0f, 1.0f),
                math::Vec3(0.0f, 0.0f, 1.0f),
                math::Vec4(1.0f, 0.0f, 0.0f, 1.0f),
                math::Vec2(1.0f, 1.0f)),
            //------------------------------------------------------------------------------------------------
            PbrVertex(
                math::Vec3(-1.f, -1.0f, -1.0f),
                math::Vec3(0.0f, 0.0f, -1.0f),
                math::Vec4(-1.0f, 0.0f, 0.0f, 1.0f),
                math::Vec2(0.0f, 0.0f)),
            PbrVertex(
                math::Vec3(1.0f, -1.0f, -1.0f),
                math::Vec3(0.0f, 0.0f, -1.0f),
                math::Vec4(-1.0f, 0.0f, 0.0f, 1.0f),
                math::Vec2(1.0f, 0.0f)),
            PbrVertex(
                math::Vec3(-1.0f, 1.0f, -1.0f),
                math::Vec3(0.0f, 0.0f, -1.0f),
                math::Vec4(-1.0f, 0.0f, 0.0f, 1.0f),
                math::Vec2(0.0f, 1.0f)),
            PbrVertex(
                math::Vec3(1.0f, 1.0f, -1.0f),
                math::Vec3(0.0f, 0.0f, -1.0f),
                math::Vec4(-1.0f, 0.0f, 0.0f, 1.0f),
                math::Vec2(1.0f, 1.0f)),
            //------------------------------------------------------------------------------------------------
            PbrVertex(
                math::Vec3(-1.f, -1.0f, -1.0f),
                math::Vec3(-1.0f, 0.0f, 0.0f),
                math::Vec4(0.0f, 0.0f, 1.0f, 1.0f),
                math::Vec2(0.0f, 0.0f)),
            PbrVertex(
                math::Vec3(-1.0f, 1.0f, -1.0f),
                math::Vec3(-1.0f, 0.0f, 0.0f),
                math::Vec4(0.0f, 0.0f, 1.0f, 1.0f),
                math::Vec2(1.0f, 0.0f)),
            PbrVertex(
                math::Vec3(-1.0f, -1.0f, 1.0f),
                math::Vec3(-1.0f, 0.0f, 0.0f),
                math::Vec4(0.0f, 0.0f, 1.0f, 1.0f),
                math::Vec2(0.0f, 1.0f)),
            PbrVertex(
                math::Vec3(-1.0f, 1.0f, 1.0f),
                math::Vec3(-1.0f, 0.0f, 0.0f),
                math::Vec4(0.0f, 0.0f, 1.0f, 1.0f),
                math::Vec2(1.0f, 1.0f)),
            //------------------------------------------------------------------------------------------------
            PbrVertex(
                math::Vec3(1.f, -1.0f, -1.0f),
                math::Vec3(1.0f, 0.0f, 0.0f),
                math::Vec4(0.0f, 0.0f, -1.0f, 1.0f),
                math::Vec2(0.0f, 0.0f)),
            PbrVertex(
                math::Vec3(1.0f, 1.0f, -1.0f),
                math::Vec3(1.0f, 0.0f, 0.0f),
                math::Vec4(0.0f, 0.0f, -1.0f, 1.0f),
                math::Vec2(1.0f, 0.0f)),
            PbrVertex(
                math::Vec3(1.0f, -1.0f, 1.0f),
                math::Vec3(1.0f, 0.0f, 0.0f),
                math::Vec4(0.0f, 0.0f, -1.0f, 1.0f),
                math::Vec2(0.0f, 1.0f)),
            PbrVertex(
                math::Vec3(1.0f, 1.0f, 1.0f),
                math::Vec3(1.0f, 0.0f, 0.0f),
                math::Vec4(0.0f, 0.0f, -1.0f, 1.0f),
                math::Vec2(1.0f, 1.0f)),
            //------------------------------------------------------------------------------------------------
            PbrVertex(
                math::Vec3(-1.f, -1.0f, -1.0f),
                math::Vec3(0.0f, -1.0f, 0.0f),
                math::Vec4(0.0f, 0.0f, -1.0f, 1.0f),
                math::Vec2(0.0f, 0.0f)),
            PbrVertex(
                math::Vec3(1.0f, -1.0f, -1.0f),
                math::Vec3(0.0f, -1.0f, 0.0f),
                math::Vec4(0.0f, 0.0f, -1.0f, 1.0f),
                math::Vec2(1.0f, 0.0f)),
            PbrVertex(
                math::Vec3(-1.0f, -1.0f, 1.0f),
                math::Vec3(0.0f, -1.0f, 0.0f),
                math::Vec4(0.0f, 0.0f, -1.0f, 1.0f),
                math::Vec2(0.0f, 1.0f)),
            PbrVertex(
                math::Vec3(1.0f, -1.0f, 1.0f),
                math::Vec3(0.0f, -1.0f, 0.0f),
                math::Vec4(0.0f, 0.0f, -1.0f, 1.0f),
                math::Vec2(1.0f, 1.0f)),
            //------------------------------------------------------------------------------------------------
            PbrVertex(
                math::Vec3(-1.f, 1.0f, -1.0f),
                math::Vec3(0.0f, 1.0f, 0.0f),
                math::Vec4(0.0f, 0.0f, -1.0f, 1.0f),
                math::Vec2(0.0f, 0.0f)),
            PbrVertex(
                math::Vec3(1.0f, 1.0f, -1.0f),
                math::Vec3(0.0f, 1.0f, 0.0f),
                math::Vec4(0.0f, 0.0f, -1.0f, 1.0f),
                math::Vec2(1.0f, 0.0f)),
            PbrVertex(
                math::Vec3(-1.0f, 1.0f, 1.0f),
                math::Vec3(0.0f, 1.0f, 0.0f),
                math::Vec4(0.0f, 0.0f, -1.0f, 1.0f),
                math::Vec2(0.0f, 1.0f)),
            PbrVertex(
                math::Vec3(1.0f, 1.0f, 1.0f),
                math::Vec3(0.0f, 1.0f, 0.0f),
                math::Vec4(0.0f, 0.0f, -1.0f, 1.0f),
                math::Vec2(1.0f, 1.0f)),
        },
        std::vector<std::uint32_t> {
            0, 1, 2, // 1
            1, 3, 2, // 2
            4, 6, 5, // 3
            5, 6, 7, // 4
            8, 10, 9, // 5
            9, 10, 11, // 6
            12, 13, 14, // 7
            13, 15, 14, // 8
            16, 17, 18, // 9
            17, 19, 18, // 10
            20, 22, 21, // 11
            21, 22, 23, // 12
        },
        std::move(c));
}

std::shared_ptr<gearoenix::render::mesh::Mesh> gearoenix::render::mesh::Manager::build_inward_cube(core::sync::EndCallerIgnored&& c) noexcept
{
    std::string name = "default-cube-mesh";
    {
        std::lock_guard<std::mutex> _lg(meshes_lock);
        if (auto search = meshes.find(name); meshes.end() != search)
            if (auto m = search->second.lock(); nullptr != m)
                return m;
    }
    return build(
        std::move(name),
        std::vector<PbrVertex> {
            PbrVertex(
                math::Vec3(-1.f, -1.0f, 1.0f),
                math::Vec3(0.0f, 0.0f, -1.0f),
                math::Vec4(1.0f, 0.0f, 0.0f, 1.0f),
                math::Vec2(0.0f, 0.0f)),
            PbrVertex(
                math::Vec3(1.0f, -1.0f, 1.0f),
                math::Vec3(0.0f, 0.0f, -1.0f),
                math::Vec4(1.0f, 0.0f, 0.0f, 1.0f),
                math::Vec2(1.0f, 0.0f)),
            PbrVertex(
                math::Vec3(-1.0f, 1.0f, 1.0f),
                math::Vec3(0.0f, 0.0f, -1.0f),
                math::Vec4(1.0f, 0.0f, 0.0f, 1.0f),
                math::Vec2(0.0f, 1.0f)),
            PbrVertex(
                math::Vec3(1.0f, 1.0f, 1.0f),
                math::Vec3(0.0f, 0.0f, -1.0f),
                math::Vec4(1.0f, 0.0f, 0.0f, 1.0f),
                math::Vec2(1.0f, 1.0f)),
            //------------------------------------------------------------------------------------------------
            PbrVertex(
                math::Vec3(-1.f, -1.0f, -1.0f),
                math::Vec3(0.0f, 0.0f, 1.0f),
                math::Vec4(-1.0f, 0.0f, 0.0f, 1.0f),
                math::Vec2(0.0f, 0.0f)),
            PbrVertex(
                math::Vec3(1.0f, -1.0f, -1.0f),
                math::Vec3(0.0f, 0.0f, 1.0f),
                math::Vec4(-1.0f, 0.0f, 0.0f, 1.0f),
                math::Vec2(1.0f, 0.0f)),
            PbrVertex(
                math::Vec3(-1.0f, 1.0f, -1.0f),
                math::Vec3(0.0f, 0.0f, 1.0f),
                math::Vec4(-1.0f, 0.0f, 0.0f, 1.0f),
                math::Vec2(0.0f, 1.0f)),
            PbrVertex(
                math::Vec3(1.0f, 1.0f, -1.0f),
                math::Vec3(0.0f, 0.0f, 1.0f),
                math::Vec4(-1.0f, 0.0f, 0.0f, 1.0f),
                math::Vec2(1.0f, 1.0f)),
            //------------------------------------------------------------------------------------------------
            PbrVertex(
                math::Vec3(-1.f, -1.0f, -1.0f),
                math::Vec3(1.0f, 0.0f, 0.0f),
                math::Vec4(0.0f, 0.0f, 1.0f, 1.0f),
                math::Vec2(0.0f, 0.0f)),
            PbrVertex(
                math::Vec3(-1.0f, 1.0f, -1.0f),
                math::Vec3(1.0f, 0.0f, 0.0f),
                math::Vec4(0.0f, 0.0f, 1.0f, 1.0f),
                math::Vec2(1.0f, 0.0f)),
            PbrVertex(
                math::Vec3(-1.0f, -1.0f, 1.0f),
                math::Vec3(1.0f, 0.0f, 0.0f),
                math::Vec4(0.0f, 0.0f, 1.0f, 1.0f),
                math::Vec2(0.0f, 1.0f)),
            PbrVertex(
                math::Vec3(-1.0f, 1.0f, 1.0f),
                math::Vec3(1.0f, 0.0f, 0.0f),
                math::Vec4(0.0f, 0.0f, 1.0f, 1.0f),
                math::Vec2(1.0f, 1.0f)),
            //------------------------------------------------------------------------------------------------
            PbrVertex(
                math::Vec3(1.f, -1.0f, -1.0f),
                math::Vec3(-1.0f, 0.0f, 0.0f),
                math::Vec4(0.0f, 0.0f, -1.0f, 1.0f),
                math::Vec2(0.0f, 0.0f)),
            PbrVertex(
                math::Vec3(1.0f, 1.0f, -1.0f),
                math::Vec3(-1.0f, 0.0f, 0.0f),
                math::Vec4(0.0f, 0.0f, -1.0f, 1.0f),
                math::Vec2(1.0f, 0.0f)),
            PbrVertex(
                math::Vec3(1.0f, -1.0f, 1.0f),
                math::Vec3(-1.0f, 0.0f, 0.0f),
                math::Vec4(0.0f, 0.0f, -1.0f, 1.0f),
                math::Vec2(0.0f, 1.0f)),
            PbrVertex(
                math::Vec3(1.0f, 1.0f, 1.0f),
                math::Vec3(-1.0f, 0.0f, 0.0f),
                math::Vec4(0.0f, 0.0f, -1.0f, 1.0f),
                math::Vec2(1.0f, 1.0f)),
            //------------------------------------------------------------------------------------------------
            PbrVertex(
                math::Vec3(-1.f, -1.0f, -1.0f),
                math::Vec3(0.0f, 1.0f, 0.0f),
                math::Vec4(0.0f, 0.0f, -1.0f, 1.0f),
                math::Vec2(0.0f, 0.0f)),
            PbrVertex(
                math::Vec3(1.0f, -1.0f, -1.0f),
                math::Vec3(0.0f, 1.0f, 0.0f),
                math::Vec4(0.0f, 0.0f, -1.0f, 1.0f),
                math::Vec2(1.0f, 0.0f)),
            PbrVertex(
                math::Vec3(-1.0f, -1.0f, 1.0f),
                math::Vec3(0.0f, 1.0f, 0.0f),
                math::Vec4(0.0f, 0.0f, -1.0f, 1.0f),
                math::Vec2(0.0f, 1.0f)),
            PbrVertex(
                math::Vec3(1.0f, -1.0f, 1.0f),
                math::Vec3(0.0f, 1.0f, 0.0f),
                math::Vec4(0.0f, 0.0f, -1.0f, 1.0f),
                math::Vec2(1.0f, 1.0f)),
            //------------------------------------------------------------------------------------------------
            PbrVertex(
                math::Vec3(-1.f, 1.0f, -1.0f),
                math::Vec3(0.0f, -1.0f, 0.0f),
                math::Vec4(0.0f, 0.0f, -1.0f, 1.0f),
                math::Vec2(0.0f, 0.0f)),
            PbrVertex(
                math::Vec3(1.0f, 1.0f, -1.0f),
                math::Vec3(0.0f, -1.0f, 0.0f),
                math::Vec4(0.0f, 0.0f, -1.0f, 1.0f),
                math::Vec2(1.0f, 0.0f)),
            PbrVertex(
                math::Vec3(-1.0f, 1.0f, 1.0f),
                math::Vec3(0.0f, -1.0f, 0.0f),
                math::Vec4(0.0f, 0.0f, -1.0f, 1.0f),
                math::Vec2(0.0f, 1.0f)),
            PbrVertex(
                math::Vec3(1.0f, 1.0f, 1.0f),
                math::Vec3(0.0f, -1.0f, 0.0f),
                math::Vec4(0.0f, 0.0f, -1.0f, 1.0f),
                math::Vec2(1.0f, 1.0f)),
        },
        std::vector<std::uint32_t> {
            0, 2, 1, // 1
            1, 2, 3, // 2
            4, 5, 6, // 3
            5, 7, 6, // 4
            8, 9, 10, // 5
            9, 11, 10, // 6
            12, 14, 13, // 7
            13, 14, 15, // 8
            16, 18, 17, // 9
            17, 18, 19, // 10
            20, 21, 22, // 11
            21, 23, 22, // 12
        },
        std::move(c));
}

std::shared_ptr<gearoenix::render::mesh::Mesh> gearoenix::render::mesh::Manager::build(
    std::string&& name,
    std::vector<PbrVertex>&& vertices,
    std::vector<std::uint32_t>&& indices,
    core::sync::EndCallerIgnored&& end_callback) noexcept
{
    {
        std::lock_guard<std::mutex> _lg(meshes_lock);
        if (auto search = meshes.find(name); meshes.end() != search)
            if (auto m = search->second.lock(); nullptr != m)
                return m;
    }
    math::Aabb3<double> occlusion_box;
    for (const PbrVertex& vertex : vertices) {
        occlusion_box.put_without_update(math::Vec3<double>(vertex.position));
    }
    occlusion_box.update();
    return build(std::move(name), std::move(vertices), std::move(indices), std::move(occlusion_box), std::move(end_callback));
}

std::shared_ptr<gearoenix::render::mesh::Mesh> gearoenix::render::mesh::Manager::build(
    std::string&& name,
    std::vector<PbrVertexAnimated>&& vertices,
    std::vector<std::uint32_t>&& indices,
    core::sync::EndCallerIgnored&& end_callback) noexcept
{
    {
        std::lock_guard<std::mutex> _lg(meshes_lock);
        if (auto search = meshes.find(name); meshes.end() != search)
            if (auto m = search->second.lock(); nullptr != m)
                return m;
    }
    math::Aabb3<double> occlusion_box;
    for (const auto& vertex : vertices) {
        occlusion_box.put_without_update(math::Vec3<double>(vertex.base.position));
    }
    occlusion_box.update();
    return build(std::move(name), std::move(vertices), std::move(indices), std::move(occlusion_box), std::move(end_callback));
}
