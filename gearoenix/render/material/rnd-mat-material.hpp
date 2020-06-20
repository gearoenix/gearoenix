#ifndef GEAROENIX_RENDER_MATERIAL_MATERIAL_HPP
#define GEAROENIX_RENDER_MATERIAL_MATERIAL_HPP
#include "../../core/cr-static.hpp"
#include "../../core/sync/cr-sync-end-caller.hpp"
#include "../../math/math-vector-4d.hpp"
#include "rnd-mat-translucency-mode.hpp"
#include "rnd-mat-type.hpp"
#include <optional>

namespace gearoenix::system::stream {
class Stream;
}

namespace gearoenix::render::engine {
class Engine;
}

namespace gearoenix::render::buffer {
class FramedUniform;
}

namespace gearoenix::render::texture {
class Texture2D;
class TextureCube;
}

namespace gearoenix::render::material {
/// Base class for materials in engine
///
/// \note The #translucency and #is_shadow_caster is for performance reason, while it is clear that all the materials may not need them.
class Material {
    GX_GET_CVAL_PRT(Type, material_type)
    GX_GET_UCPTR_PRT(buffer::FramedUniform, uniform_buffers)
    GX_GETSET_VAL_PRT(TranslucencyMode, translucency, TranslucencyMode::Opaque)
    GX_GET_VAL_PRT(bool, is_shadow_caster, true)
protected:
    engine::Engine* const e;
    Material(Type t, engine::Engine* e, std::size_t uniform_size) noexcept;
    Material(const Material& o) noexcept;

    [[nodiscard]] static float read_alpha(system::stream::Stream* f) noexcept;
    [[nodiscard]] std::pair<std::shared_ptr<texture::Texture2D>, std::optional<math::Vec4<float>>> read_t2d_v4(
        system::stream::Stream* f, const core::sync::EndCaller<core::sync::EndCallerIgnore>& end) const noexcept;
    [[nodiscard]] std::pair<std::shared_ptr<texture::Texture2D>, std::optional<math::Vec3<float>>> read_t2d_v3(
        system::stream::Stream* f, const core::sync::EndCaller<core::sync::EndCallerIgnore>& end) const noexcept;
    [[nodiscard]] std::pair<std::shared_ptr<texture::Texture2D>, std::optional<math::Vec2<float>>> read_t2d_v2(
        system::stream::Stream* f, const core::sync::EndCaller<core::sync::EndCallerIgnore>& end) const noexcept;

public:
    virtual ~Material() noexcept;
    virtual void update() noexcept = 0;
    [[nodiscard]] static Material* read(system::stream::Stream* f, engine::Engine* e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& end) noexcept;
    [[nodiscard]] virtual Material* clone() const noexcept;
};
}
#endif
