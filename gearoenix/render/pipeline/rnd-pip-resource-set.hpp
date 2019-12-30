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
    public:
        virtual ~ResourceSet() noexcept = default;
        virtual void clean() noexcept = 0;
    };
}
}

#endif
