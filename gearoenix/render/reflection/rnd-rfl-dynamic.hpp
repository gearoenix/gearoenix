#ifndef GEAROENIX_RENDER_REFLECTION_DYNAMIC_HPP
#define GEAROENIX_RENDER_REFLECTION_DYNAMIC_HPP
#include "rnd-rfl-reflection.hpp"

namespace gearoenix::render::camera {
class Camera;
}

namespace gearoenix::render::texture {
class TextureCube;
}

namespace gearoenix::render::reflection {
class Dynamic : public Reflection {
    GX_GET_CREF_PRV(std::shared_ptr<texture::TextureCube const>, diffuse)
    GX_GET_CREF_PRV(std::shared_ptr<texture::TextureCube const>, specular)
    GX_GET_ARRC_PRV(std::shared_ptr<camera::Camera>, cameras, 6)
public:
    Dynamic(core::Id id, engine::Engine* e) noexcept;
    ~Dynamic() noexcept override;
};
}
#endif
