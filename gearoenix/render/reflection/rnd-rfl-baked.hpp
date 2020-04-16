#ifndef GEAROENIX_RENDER_REFLECTION_BAKED_HPP
#define GEAROENIX_RENDER_REFLECTION_BAKED_HPP
#include "rnd-rfl-reflection.hpp"

namespace gearoenix::render::reflection {
class Baked final : public Reflection {
public:
    /// This function can be used for creating skybox baked reflection probe
    Baked(std::shared_ptr<texture::TextureCube> irradiance, std::shared_ptr<texture::TextureCube> radiance) noexcept;
    ~Baked() noexcept final;
};
}
#endif
