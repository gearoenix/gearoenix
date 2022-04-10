#include "gx-rnd-msh-manager.hpp"
#include "../../core/asset/gx-cr-asset-manager.hpp"

gearoenix::render::mesh::Manager::Manager(engine::Engine& e) noexcept
    : e(e)
{
}

gearoenix::render::mesh::Manager::~Manager() noexcept = default;

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

// std::shared_ptr<gearoenix::render::mesh::Mesh> gearoenix::render::mesh::Manager::create_inward_cube(core::sync::EndCaller<Mesh>& c) noexcept
//{
//     if (std::shared_ptr<Mesh> m = inward_cube.lock()) {
//         c.set_data(m);
//         return m;
//     }
//     const std::vector<math::BasicVertex> vertices = {
//         PbrVertex(
//             math::Vec3(-1.f, -1.0f, 1.0f),
//             math::Vec3(0.0f, 0.0f, 1.0f),
//             math::Vec4(1.0f, 0.0f, 0.0f, 1.0f),
//             math::Vec2(0.0f, 0.0f),
//         },
//         PbrVertex(
//             math::Vec3(1.0f, -1.0f, 1.0f),
//             math::Vec3(0.0f, 0.0f, 1.0f),
//             math::Vec4(1.0f, 0.0f, 0.0f, 1.0f),
//             math::Vec2(1.0f, 0.0f),
//         },
//         PbrVertex(
//             math::Vec3(-1.0f, 1.0f, 1.0f),
//             math::Vec3(0.0f, 0.0f, 1.0f),
//             math::Vec4(1.0f, 0.0f, 0.0f, 1.0f),
//             math::Vec2(0.0f, 1.0f),
//         },
//         PbrVertex(
//             math::Vec3(1.0f, 1.0f, 1.0f),
//             math::Vec3(0.0f, 0.0f, 1.0f),
//             math::Vec4(1.0f, 0.0f, 0.0f, 1.0f),
//             math::Vec2(1.0f, 1.0f),
//         },
//         //------------------------------------------------------------------------------------------------
//         PbrVertex(
//             math::Vec3(-1.f, -1.0f, -1.0f),
//             math::Vec3(0.0f, 0.0f, -1.0f),
//             math::Vec4(-1.0f, 0.0f, 0.0f, 1.0f),
//             math::Vec2(0.0f, 0.0f),
//         },
//         PbrVertex(
//             math::Vec3(1.0f, -1.0f, -1.0f),
//             math::Vec3(0.0f, 0.0f, -1.0f),
//             math::Vec4(-1.0f, 0.0f, 0.0f, 1.0f),
//             math::Vec2(1.0f, 0.0f),
//         },
//         PbrVertex(
//             math::Vec3(-1.0f, 1.0f, -1.0f),
//             math::Vec3(0.0f, 0.0f, -1.0f),
//             math::Vec4(-1.0f, 0.0f, 0.0f, 1.0f),
//             math::Vec2(0.0f, 1.0f),
//         },
//         PbrVertex(
//             math::Vec3(1.0f, 1.0f, -1.0f),
//             math::Vec3(0.0f, 0.0f, -1.0f),
//             math::Vec4(-1.0f, 0.0f, 0.0f, 1.0f),
//             math::Vec2(1.0f, 1.0f),
//         },
//         //------------------------------------------------------------------------------------------------
//         PbrVertex(
//             math::Vec3(-1.f, -1.0f, -1.0f),
//             math::Vec3(-1.0f, 0.0f, 0.0f),
//             math::Vec4(0.0f, 0.0f, 1.0f, 1.0f),
//             math::Vec2(0.0f, 0.0f),
//         },
//         PbrVertex(
//             math::Vec3(-1.0f, 1.0f, -1.0f),
//             math::Vec3(-1.0f, 0.0f, 0.0f),
//             math::Vec4(0.0f, 0.0f, 1.0f, 1.0f),
//             math::Vec2(1.0f, 0.0f),
//         },
//         PbrVertex(
//             math::Vec3(-1.0f, -1.0f, 1.0f),
//             math::Vec3(-1.0f, 0.0f, 0.0f),
//             math::Vec4(0.0f, 0.0f, 1.0f, 1.0f),
//             math::Vec2(0.0f, 1.0f),
//         },
//         PbrVertex(
//             math::Vec3(-1.0f, 1.0f, 1.0f),
//             math::Vec3(-1.0f, 0.0f, 0.0f),
//             math::Vec4(0.0f, 0.0f, 1.0f, 1.0f),
//             math::Vec2(1.0f, 1.0f),
//         },
//         //------------------------------------------------------------------------------------------------
//         PbrVertex(
//             math::Vec3(1.f, -1.0f, -1.0f),
//             math::Vec3(1.0f, 0.0f, 0.0f),
//             math::Vec4(0.0f, 0.0f, -1.0f, 1.0f),
//             math::Vec2(0.0f, 0.0f),
//         },
//         PbrVertex(
//             math::Vec3(1.0f, 1.0f, -1.0f),
//             math::Vec3(1.0f, 0.0f, 0.0f),
//             math::Vec4(0.0f, 0.0f, -1.0f, 1.0f),
//             math::Vec2(1.0f, 0.0f),
//         },
//         PbrVertex(
//             math::Vec3(1.0f, -1.0f, 1.0f),
//             math::Vec3(1.0f, 0.0f, 0.0f),
//             math::Vec4(0.0f, 0.0f, -1.0f, 1.0f),
//             math::Vec2(0.0f, 1.0f),
//         },
//         PbrVertex(
//             math::Vec3(1.0f, 1.0f, 1.0f),
//             math::Vec3(1.0f, 0.0f, 0.0f),
//             math::Vec4(0.0f, 0.0f, -1.0f, 1.0f),
//             math::Vec2(1.0f, 1.0f),
//         },
//         //------------------------------------------------------------------------------------------------
//         PbrVertex(
//             math::Vec3(-1.f, -1.0f, -1.0f),
//             math::Vec3(0.0f, -1.0f, 0.0f),
//             math::Vec4(0.0f, 0.0f, -1.0f, 1.0f),
//             math::Vec2(0.0f, 0.0f),
//         },
//         PbrVertex(
//             math::Vec3(1.0f, -1.0f, -1.0f),
//             math::Vec3(0.0f, -1.0f, 0.0f),
//             math::Vec4(0.0f, 0.0f, -1.0f, 1.0f),
//             math::Vec2(1.0f, 0.0f),
//         },
//         PbrVertex(
//             math::Vec3(-1.0f, -1.0f, 1.0f),
//             math::Vec3(0.0f, -1.0f, 0.0f),
//             math::Vec4(0.0f, 0.0f, -1.0f, 1.0f),
//             math::Vec2(0.0f, 1.0f),
//         },
//         PbrVertex(
//             math::Vec3(1.0f, -1.0f, 1.0f),
//             math::Vec3(0.0f, -1.0f, 0.0f),
//             math::Vec4(0.0f, 0.0f, -1.0f, 1.0f),
//             math::Vec2(1.0f, 1.0f),
//         },
//         //------------------------------------------------------------------------------------------------
//         PbrVertex(
//             math::Vec3(-1.f, 1.0f, -1.0f),
//             math::Vec3(0.0f, 1.0f, 0.0f),
//             math::Vec4(0.0f, 0.0f, -1.0f, 1.0f),
//             math::Vec2(0.0f, 0.0f),
//         },
//         PbrVertex(
//             math::Vec3(1.0f, 1.0f, -1.0f),
//             math::Vec3(0.0f, 1.0f, 0.0f),
//             math::Vec4(0.0f, 0.0f, -1.0f, 1.0f),
//             math::Vec2(1.0f, 0.0f),
//         },
//         PbrVertex(
//             math::Vec3(-1.0f, 1.0f, 1.0f),
//             math::Vec3(0.0f, 1.0f, 0.0f),
//             math::Vec4(0.0f, 0.0f, -1.0f, 1.0f),
//             math::Vec2(0.0f, 1.0f),
//         },
//         PbrVertex(
//             math::Vec3(1.0f, 1.0f, 1.0f),
//             math::Vec3(0.0f, 1.0f, 0.0f),
//             math::Vec4(0.0f, 0.0f, -1.0f, 1.0f),
//             math::Vec2(1.0f, 1.0f),
//         },
//     };
//     const std::vector<std::uint32_t> indices = {
//         0, 2, 1, // - 1
//         1, 2, 3, // - 2
//         4, 5, 6, // - 3
//         5, 7, 6, // - 4
//         8, 9, 10, // - 5
//         9, 11, 10, // - 6
//         12, 14, 13, // - 7
//         13, 14, 15, // - 8
//         16, 18, 17, // - 9
//         17, 18, 19, // - 10
//         20, 21, 22, // - 11
//         21, 23, 22, // - 12
//     };
//     const auto id = core::asset::Manager::create_id();
//     std::shared_ptr<Mesh> m(new Mesh(
//         id, "inward-cube", vertices, indices,
//         math::Aabb3(math::Vec3(1.0), math::Vec3(-1.0)),
//         e, core::sync::EndCaller<core::sync::EndCallerIgnore>([c] {})));
//     c.set_data(m);
//     inward_cube = m;
//     cache.get_cacher().get_cacheds()[id] = m;
//     return m;
// }

// std::shared_ptr<gearoenix::render::mesh::Mesh> gearoenix::render::mesh::Manager::create_face_square(
//     const texture::Face f,
//     core::sync::EndCaller<Mesh>& c) noexcept
//{
//     const auto search = face_squares.find(f);
//     if (search != face_squares.end()) {
//         if (std::shared_ptr<Mesh> m = search->second.lock()) {
//             c.set_data(m);
//             return m;
//         }
//     }
//     std::vector<math::BasicVertex> vertices = {
//         PbrVertex(
//             math::Vec3(-1.f, -1.0f, 0.5f),
//             math::Vec3(0.0f),
//             math::Vec4(0.0f),
//             math::Vec2(0.0f),
//         },
//         PbrVertex(
//             math::Vec3(1.0f, -1.0f, 0.5f),
//             math::Vec3(0.0f),
//             math::Vec4(0.0f),
//             math::Vec2(0.0f),
//         },
//         PbrVertex(
//             math::Vec3(-1.0f, 1.0f, 0.5f),
//             math::Vec3(0.0f),
//             math::Vec4(0.0f),
//             math::Vec2(0.0f),
//         },
//         PbrVertex(
//             math::Vec3(1.0f, 1.0f, 0.5f),
//             math::Vec3(0.0f),
//             math::Vec4(0.0f),
//             math::Vec2(0.0f),
//         },
//     };
//     switch (f) {
//     case texture::Face::PositiveX:
//         vertices[0].normal = math::Vec3(1.0f, 1.0f, 1.0f);
//         vertices[1].normal = math::Vec3(1.0f, 1.0f, -1.0f);
//         vertices[2].normal = math::Vec3(1.0f, -1.0f, 1.0f);
//         vertices[3].normal = math::Vec3(1.0f, -1.0f, -1.0f);
//         break;
//     case texture::Face::NegativeX:
//         vertices[0].normal = math::Vec3(-1.0f, 1.0f, -1.0f);
//         vertices[1].normal = math::Vec3(-1.0f, 1.0f, 1.0f);
//         vertices[2].normal = math::Vec3(-1.0f, -1.0f, -1.0f);
//         vertices[3].normal = math::Vec3(-1.0f, -1.0f, 1.0f);
//         break;
//     case texture::Face::PositiveY:
//         vertices[0].normal = math::Vec3(-1.0f, 1.0f, -1.0f);
//         vertices[1].normal = math::Vec3(1.0f, 1.0f, -1.0f);
//         vertices[2].normal = math::Vec3(-1.0f, 1.0f, 1.0f);
//         vertices[3].normal = math::Vec3(1.0f, 1.0f, 1.0f);
//         break;
//     case texture::Face::NegativeY:
//         vertices[0].normal = math::Vec3(-1.0f, -1.0f, 1.0f);
//         vertices[1].normal = math::Vec3(1.0f, -1.0f, 1.0f);
//         vertices[2].normal = math::Vec3(-1.0f, -1.0f, -1.0f);
//         vertices[3].normal = math::Vec3(1.0f, -1.0f, -1.0f);
//         break;
//     case texture::Face::PositiveZ:
//         vertices[0].normal = math::Vec3(-1.0f, 1.0f, 1.0f);
//         vertices[1].normal = math::Vec3(1.0f, 1.0f, 1.0f);
//         vertices[2].normal = math::Vec3(-1.0f, -1.0f, 1.0f);
//         vertices[3].normal = math::Vec3(1.0f, -1.0f, 1.0f);
//         break;
//     case texture::Face::NegativeZ:
//         vertices[0].normal = math::Vec3(1.0f, 1.0f, -1.0f);
//         vertices[1].normal = math::Vec3(-1.0f, 1.0f, -1.0f);
//         vertices[2].normal = math::Vec3(1.0f, -1.0f, -1.0f);
//         vertices[3].normal = math::Vec3(-1.0f, -1.0f, -1.0f);
//         break;
//     }
//     const std::vector<std::uint32_t> indices = {
//         0, 1, 2, // - 1
//         1, 3, 2, // - 2
//     };
//     const auto id = core::asset::Manager::create_id();
//     std::shared_ptr<Mesh> m(new Mesh(
//         id, std::string("mesh-face-") + std::to_string(f), vertices, indices,
//         math::Aabb3(math::Vec3(1.0), math::Vec3(-1.0)),
//         e, core::sync::EndCaller<core::sync::EndCallerIgnore>([c] {})));
//     c.set_data(m);
//     cache.get_cacher().get_cacheds()[id] = m;
//     face_squares[f] = m;
//     return m;
// }

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
