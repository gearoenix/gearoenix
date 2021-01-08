#ifndef GEAROENIX_RENDER_MATERIAL_MATERIAL_HPP
#define GEAROENIX_RENDER_MATERIAL_MATERIAL_HPP
#include "../../core/gx-cr-static.hpp"
#include "../../core/sync/gx-cr-sync-end-caller.hpp"
#include "../../math/gx-math-vector-4d.hpp"
#include "gx-rnd-mat-translucency-mode.hpp"
#include "gx-rnd-mat-type.hpp"
#include <optional>

namespace gearoenix::platform::stream {
struct Stream;
}

namespace gearoenix::render::engine {
struct Engine;
}

namespace gearoenix::render::buffer {
struct FramedUniform;
}

namespace gearoenix::render::texture {
struct Texture2D;
struct TextureCube;
}

namespace gearoenix::render::material {
/// Base struct for materials in engine
///
/// \note The #translucency and #is_shadow_caster is for performance reason, while it is clear that all the materials may not need them.
struct Material {
    GX_GET_CVAL_PRT(Type, material_type)
    GX_GET_UCPTR_PRT(buffer::FramedUniform, uniform_buffers)
    GX_GETSET_VAL_PRT(TranslucencyMode, translucency, TranslucencyMode::Opaque)
    GX_GET_VAL_PRT(bool, is_shadow_caster, true)
protected:
    engine::Engine* const e;
    Material(Type t, engine::Engine* e, std::size_t uniform_size) noexcept;
    Material(const Material& o) noexcept;

    [[nodiscard]] static float read_alpha(platform::stream::Stream* f) noexcept;
    [[nodiscard]] std::pair<std::shared_ptr<texture::Texture2D>, std::optional<math::Vec4<float>>> read_t2d_v4(
        platform::stream::Stream* f, const core::sync::EndCaller<core::sync::EndCallerIgnore>& end) const noexcept;
    [[nodiscard]] std::pair<std::shared_ptr<texture::Texture2D>, std::optional<math::Vec3<float>>> read_t2d_v3(
        platform::stream::Stream* f, const core::sync::EndCaller<core::sync::EndCallerIgnore>& end) const noexcept;
    [[nodiscard]] std::pair<std::shared_ptr<texture::Texture2D>, std::optional<math::Vec2<float>>> read_t2d_v2(
        platform::stream::Stream* f, const core::sync::EndCaller<core::sync::EndCallerIgnore>& end) const noexcept;

public:
    virtual ~Material() noexcept;
    virtual void update() noexcept = 0;
    [[nodiscard]] static Material* read(platform::stream::Stream* f, engine::Engine* e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& end) noexcept;
    [[nodiscard]] virtual Material* clone() const noexcept;
};
}
#endif
