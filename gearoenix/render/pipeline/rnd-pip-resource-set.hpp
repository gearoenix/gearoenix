#ifndef GEAROENIX_RENDER_PIPELINE_RESOURCE_SET_HPP
#define GEAROENIX_RENDER_PIPELINE_RESOURCE_SET_HPP

namespace gearoenix::render {
namespace buffer {
    class Uniform;
}
namespace camera {
    class Camera;
}
namespace command {
    class Buffer;
}
namespace graph::node {
    class Node;
}
namespace light {
    class Light;
}
namespace material {
    class Material;
}
namespace mesh {
    class Mesh;
}
namespace model {
    class Model;
}
namespace scene {
    class Scene;
}
namespace texture {
    class Texture2D;
    class Cube;
}
namespace pipeline {
    class ResourceSet {
    protected:
        /// It is not owner of any of these objects

        const buffer::Uniform* scene_uniform_buffer = nullptr;
        const buffer::Uniform* camera_uniform_buffer = nullptr;
        const buffer::Uniform* model_uniform_buffer = nullptr;
        const buffer::Uniform* material_uniform_buffer = nullptr;
        const buffer::Uniform* node_uniform_buffer = nullptr;

        const mesh::Mesh* msh = nullptr;

        texture::Texture2D* color = nullptr;
        texture::Texture2D* metallic_roughness = nullptr;
        texture::Texture2D* normal = nullptr;
        texture::Texture2D* emissive = nullptr;

    public:
        virtual ~ResourceSet() noexcept = default;
#define GX_HELPER(c, cc) virtual void set_##c(const c::cc* o) noexcept
        GX_HELPER(scene, Scene);
        GX_HELPER(camera, Camera);
        GX_HELPER(model, Model);
        GX_HELPER(mesh, Mesh);
        GX_HELPER(material, Material);
#undef GX_HELPER
        void set_node_uniform_buffer(buffer::Uniform* node_uniform_buffer) noexcept;
        virtual void clean() noexcept;
    };
}
}

#endif
