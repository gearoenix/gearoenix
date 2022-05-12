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
        GX_GUARD_LOCK(meshes);
        if (auto search = meshes.find(name); meshes.end() != search)
            if (auto m = search->second.lock(); nullptr != m)
                return m;
    }
    std::vector<PbrVertex> vertices {
        PbrVertex(
            math::Vec3(0.0f, 0.0f, -1.0f),
            math::Vec3(8.129646857923944e-07f, 0.0f, -1.0f),
            math::Vec4(0.0f, 1.0f, 0.0f, 1.0f),
            math::Vec2(0.49999767541885376f, 0.9999938011169434f)),
        PbrVertex(
            math::Vec3(0.7235999703407288f, -0.5257200002670288f, -0.4472149908542633f),
            math::Vec3(0.7236069440841675f, -0.5257307291030884f, -0.44721388816833496f),
            math::Vec4(0.5877845287322998f, 0.8090175986289978f, -8.292501547657594e-07f, 1.0f),
            math::Vec2(0.19999584555625916f, 0.6666535139083862f)),
        PbrVertex(
            math::Vec3(-0.27638500928878784f, -0.8506399989128113f, -0.4472149908542633f),
            math::Vec3(-0.27638790011405945f, -0.8506532907485962f, -0.4472121298313141f),
            math::Vec4(0.9589834809303284f, -0.21364352107048035f, -0.18629835546016693f, 1.0f),
            math::Vec2(-5.453824996948242e-06f, 0.6666535139083862f)),
        PbrVertex(
            math::Vec3(0.7235999703407288f, 0.5257200002670288f, -0.4472149908542633f),
            math::Vec3(0.7236069440841675f, 0.5257307291030884f, -0.44721388816833496f),
            math::Vec4(-0.5877845287322998f, 0.8090175986289978f, 8.292501547657594e-07f, 1.0f),
            math::Vec2(0.39999711513519287f, 0.6666535139083862f)),
        PbrVertex(
            math::Vec3(-0.27638500928878784f, -0.8506399989128113f, -0.4472149908542633f),
            math::Vec3(-0.27638790011405945f, -0.8506532907485962f, -0.4472121298313141f),
            math::Vec4(0.8618077635765076f, -0.42531833052635193f, 0.2763904333114624f, 1.0f),
            math::Vec2(1.0000009536743164f, 0.6666535139083862f)),
        PbrVertex(
            math::Vec3(-0.8944249749183655f, 0.0f, -0.4472149908542633f),
            math::Vec3(-0.8944282531738281f, 0.0f, -0.44721153378486633f),
            math::Vec4(0.0f, -1.0f, 0.0f, 1.0f),
            math::Vec2(0.7999997735023499f, 0.6666535139083862f)),
        PbrVertex(
            math::Vec3(-0.27638500928878784f, 0.8506399989128113f, -0.4472149908542633f),
            math::Vec3(-0.27638790011405945f, 0.8506532907485962f, -0.4472121298313141f),
            math::Vec4(-0.951058566570282f, -0.30901074409484863f, 2.1097672231462639e-07f, 1.0f),
            math::Vec2(0.5999984741210938f, 0.6666535139083862f)),
        PbrVertex(
            math::Vec3(0.8944249749183655f, 0.0f, 0.4472149908542633f),
            math::Vec3(0.8944283127784729f, 0.0f, 0.44721153378486633f),
            math::Vec4(0.0f, 1.0f, 0.0f, 1.0f),
            math::Vec2(0.2999964952468872f, 0.3333156108856201f)),
        PbrVertex(
            math::Vec3(0.27638500928878784f, -0.8506399989128113f, 0.4472149908542633f),
            math::Vec3(0.27638792991638184f, -0.8506532907485962f, 0.44721218943595886f),
            math::Vec4(0.951058566570282f, 0.30901074409484863f, -2.270776633395144e-07f, 1.0f),
            math::Vec2(0.0999951958656311f, 0.3333156108856201f)),
        PbrVertex(
            math::Vec3(-0.7235999703407288f, -0.5257200002670288f, 0.4472149908542633f),
            math::Vec3(-0.7236069440841675f, -0.5257307291030884f, 0.44721388816833496f),
            math::Vec4(0.4995337128639221f, -0.8460252285003662f, -0.1862989217042923f, 1.0f),
            math::Vec2(0.900000274181366f, 0.3333156108856201f)),
        PbrVertex(
            math::Vec3(-0.7235999703407288f, 0.5257200002670288f, 0.4472149908542633f),
            math::Vec3(-0.7236069440841675f, 0.5257306694984436f, 0.4472138285636902f),
            math::Vec4(-0.5877845287322998f, -0.8090175986289978f, 8.145179890561849e-07f, 1.0f),
            math::Vec2(0.6999990940093994f, 0.3333156108856201f)),
        PbrVertex(
            math::Vec3(0.27638500928878784f, 0.8506399989128113f, 0.4472149908542633f),
            math::Vec3(0.2763878405094147f, 0.8506532311439514f, 0.4472121596336365f),
            math::Vec4(-0.951058566570282f, 0.30901068449020386f, 1.8758589703793405e-07f, 1.0f),
            math::Vec2(0.49999767541885376f, 0.3333156108856201f)),
        PbrVertex(
            math::Vec3(-0.7235999703407288f, -0.5257200002670288f, 0.4472149908542633f),
            math::Vec3(-0.7236069440841675f, -0.5257307291030884f, 0.44721388816833496f),
            math::Vec4(0.6708197593688965f, -0.6881924271583557f, 0.27639108896255493f, 1.0f),
            math::Vec2(-0.10000729560852051f, 0.3333156108856201f)),
        PbrVertex(
            math::Vec3(0.0f, 0.0f, 1.0f),
            math::Vec3(-8.022206543500943e-07f, 7.16268422351618e-09f, 1.0f),
            math::Vec4(-0.9510565996170044f, 0.30901655554771423f, -7.651706255273893e-07f, 1.0f),
            math::Vec2(0.39999711513519287f, -2.4557113647460938e-05f)),
        PbrVertex(
            math::Vec3(0.0f, 0.0f, 1.0f),
            math::Vec3(-8.022206543500943e-07f, 7.16268422351618e-09f, 1.0f),
            math::Vec4(0.9510550498962402f, -0.3090214729309082f, 7.651694886590121e-07f, 1.0f),
            math::Vec2(0.39999714493751526f, -2.4557113647460938e-05f)),
    };

    std::vector<std::uint32_t> indices {
        0, 1, 2, // 0
        1, 0, 3, // 1
        0, 4, 5, // 2
        0, 5, 6, // 3
        0, 6, 3, // 4
        1, 3, 7, // 5
        2, 1, 8, // 6
        5, 4, 9, // 7
        6, 5, 10, // 8
        3, 6, 11, // 9
        1, 7, 8, // 10
        2, 8, 12, // 11
        5, 9, 10, // 12
        6, 10, 11, // 13
        3, 11, 7, // 14
        8, 7, 13, // 15
        12, 8, 14, // 16
        10, 9, 13, // 17
        11, 10, 13, // 18
        7, 11, 13, // 19
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
                v.tangent = math::Vec4(((p1.tangent.xyz() + p2.tangent.xyz()) * 0.5f).normalised(), 1.0f);
                v.uv = (p1.uv + p2.uv) * 0.5f;
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
        GX_GUARD_LOCK(meshes);
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
        GX_GUARD_LOCK(meshes);
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
        GX_GUARD_LOCK(meshes);
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
        GX_GUARD_LOCK(meshes);
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
