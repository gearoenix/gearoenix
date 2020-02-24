#ifndef GEAROENIX_RENDER_REFLECTION_RUNTIME_HPP
#define GEAROENIX_RENDER_REFLECTION_RUNTIME_HPP
#include "rnd-rfl-reflection.hpp"

namespace gearoenix::render::camera {
class Camera;
}

namespace gearoenix::render::reflection {
class Runtime final : public Reflection {
    GX_GET_CREF_PRV(std::shared_ptr<texture::TextureCube const>, environment)
    GX_GET_ARRC_PRV(std::shared_ptr<camera::Camera>, cameras, 6)
public:
    Runtime(core::Id id, engine::Engine* e) noexcept;
    ~Runtime() noexcept final;
};
}
#endif
