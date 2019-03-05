#ifndef GEAROEMIX_RENDER_PIPELINE_RESOURCE_SET_HPP
#define GEAROEMIX_RENDER_PIPELINE_RESOURCE_SET_HPP

#include <memory>

namespace gearoenix {
namespace render {
    namespace buffer {
        class Uniform;
    }
    namespace camera {
        class Camera;
    }
    namespace command {
        class Buffer;
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
    namespace pipeline {
        class Resource;
        class ResourceSet {
        public:
            virtual void set_scene(const std::shared_ptr<scene::Scene>& s) = 0;
            virtual void set_camera(const std::shared_ptr<camera::Camera>& c) = 0;
            virtual void set_light(const std::shared_ptr<light::Light>& l) = 0;
            virtual void set_model(const std::shared_ptr<model::Model>& m) = 0;
            virtual void set_mesh(const std::shared_ptr<mesh::Mesh>& m) = 0;
            virtual void set_material(const std::shared_ptr<material::Material>& m) = 0;
            virtual void set_effect(const std::shared_ptr<buffer::Uniform>& u, const std::shared_ptr<Resource>& r) = 0;
            virtual void record(const std::shared_ptr<command::Buffer>& c) = 0;
        };
    }
}
}

#endif