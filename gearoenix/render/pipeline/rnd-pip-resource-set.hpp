#ifndef GEAROENIX_RENDER_PIPELINE_RESOURCE_SET_HPP
#define GEAROENIX_RENDER_PIPELINE_RESOURCE_SET_HPP
#include "../../core/cr-static.hpp"

namespace gearoenix::render::buffer {
class Uniform;
}

namespace gearoenix::render::camera {
class Camera;
}

namespace gearoenix::render::command {
class Buffer;
}

namespace gearoenix::render::graph::node {
class Node;
}

namespace gearoenix::render::light {
class Light;
}

namespace gearoenix::render::material {
class Material;
}

namespace gearoenix::render::mesh {
class Mesh;
}

namespace gearoenix::render::model {
class Model;
}

namespace gearoenix::render::scene {
class Scene;
}

namespace gearoenix::render::texture {
class Texture2D;
class TextureCube;
}

namespace gearoenix::render::pipeline {
class Pipeline;
class ResourceSet {
    GX_GET_CREF_PRV(std::shared_ptr<Pipeline const>, pip)
protected:
    explicit ResourceSet(std::shared_ptr<Pipeline const> pip) noexcept;

public:
    virtual ~ResourceSet() noexcept;
    virtual void clean() noexcept = 0;
};
}

#endif
